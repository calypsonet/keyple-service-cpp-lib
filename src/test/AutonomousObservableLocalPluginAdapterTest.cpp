/**************************************************************************************************
 * Copyright (c) 2021 Calypso Networks Association                                                *
 * https://www.calypsonet-asso.org/                                                               *
 *                                                                                                *
 * See the NOTICE file(s) distributed with this work for additional information regarding         *
 * copyright ownership.                                                                           *
 *                                                                                                *
 * This program and the accompanying materials are made available under the terms of the Eclipse  *
 * Public License 2.0 which is available at http://www.eclipse.org/legal/epl-2.0                  *
 *                                                                                                *
 * SPDX-License-Identifier: EPL-2.0                                                               *
 **************************************************************************************************/

#include "gmock/gmock.h"
#include "gtest/gtest.h"

#include <chrono>
#include <thread>

/* Keyple Core Plugin */
#include "AutonomousObservablePluginSpi.h"

/* Keyple Core Service */
#include "AutonomousObservableLocalPluginAdapter.h"
#include "PluginObserverSpi.h"
#include "PluginObservationExceptionHandlerSpi.h"

/* Keyple Core Util */
#include "RuntimeException.h"

using namespace testing;

using namespace keyple::core::plugin::spi;
using namespace keyple::core::service;
using namespace keyple::core::service::spi;
using namespace keyple::core::util::cpp::exception;

const std::string PLUGIN_NAME = "plugin";
const std::string READER_NAME_1 = "reader1";

class AOLPAT_AutonomousObservablePluginSpiMock final : public AutonomousObservablePluginSpi {
public:
    virtual void connect(AutonomousObservablePluginApi* autonomousObservablePluginSpi) override
        final
    {
        (void)autonomousObservablePluginSpi;
    }

    virtual const std::string& getName() const override final
    {
        return PLUGIN_NAME;
    }

    virtual const std::vector<std::shared_ptr<ReaderSpi>> searchAvailableReaders() override final
    {
        return {};
    }

    virtual void onUnregister() override final {}
};


class AOLPAT_PluginObserverSpiMock final : public PluginObserverSpi {
public:
    AOLPAT_PluginObserverSpiMock(const std::shared_ptr<RuntimeException> e) : mThrowEx(e) {}

    virtual void onPluginEvent(const std::shared_ptr<PluginEvent> pluginEvent) override final
    {
        mEventTypeReceived.insert({pluginEvent->getType(), pluginEvent});
        if (mThrowEx) {
            throw mThrowEx;
        }
    }

    bool hasReceived(const PluginEvent::Type eventType) const
    {
        const auto it = mEventTypeReceived.find(eventType);

        return it != mEventTypeReceived.end();
    }

    const std::shared_ptr<PluginEvent> getLastEventOfType(const PluginEvent::Type eventType) const
    {
        return mEventTypeReceived.at(eventType);
    }

private:
    std::map<PluginEvent::Type, std::shared_ptr<PluginEvent>> mEventTypeReceived;
    const std::shared_ptr<RuntimeException> mThrowEx;
};

class AOLPAT_PluginExceptionHandlerMock final : public PluginObservationExceptionHandlerSpi {
 public:
    AOLPAT_PluginExceptionHandlerMock(const std::shared_ptr<RuntimeException> throwEx)
    : mInvoked(true), mThrowEx(throwEx) {}

    virtual void onPluginObservationError(const std::string& pluginName, const std::exception& e)
        override final
    {
        mInvoked = true;
        if (mThrowEx) {
            throw mThrowEx;
        }
        mPluginName = pluginName;
        mE = e;
    }

    bool isInvoked() const
    {
        return mInvoked;
    }

    const std::string& getPluginName() const
    {
        return mPluginName;
    }

    const std::exception& getE() const
    {
        return mE;
    }

private:
    bool mInvoked = false;
    std::string mPluginName;
    std::exception mE;
    const std::shared_ptr<RuntimeException> mThrowEx;
};

class AOLPAT_ReaderSpiMock final : public KeypleReaderExtension, public ReaderSpi {
public:
    virtual const std::string& getName() const override final
    {
        return READER_NAME_1;
    }

    virtual bool isProtocolSupported(const std::string& readerProtocol) const override final
    {
        (void)readerProtocol;
        return true;
    }

    virtual void activateProtocol(const std::string& readerProtocol) override final
    {
        (void)readerProtocol;
    }

    virtual void deactivateProtocol(const std::string& readerProtocol) override final
    {
        (void)readerProtocol;
    }

    virtual bool isCurrentProtocol(const std::string& readerProtocol) const override final
    {
        (void)readerProtocol;
        return true;
    }

    virtual void openPhysicalChannel() override final {}

    virtual void closePhysicalChannel() override final {}

    virtual bool isPhysicalChannelOpen() const override final
    {
        return true;
    }

    virtual bool checkCardPresence() override final
    {
        return true;
    }

    virtual const std::string getPowerOnData() const override final
    {
        return "";
    }

    virtual const std::vector<uint8_t> transmitApdu(const std::vector<uint8_t>& apduIn) override
        final
    {
        (void)apduIn;
        return {};
    }

    virtual bool isContactless() override final
    {
        return true;
    }

    virtual void onUnregister() override final {}
};

static std::shared_ptr<AutonomousObservablePluginSpi> pluginSpi;
static std::shared_ptr<AutonomousObservableLocalPluginAdapter> plugin;
static std::shared_ptr<AOLPAT_PluginObserverSpiMock> observer;
static std::shared_ptr<PluginObservationExceptionHandlerSpi> exceptionHandler;
static std::shared_ptr<ReaderSpi> readerSpi1;

static void setUp()
{
    pluginSpi = std::make_shared<AOLPAT_AutonomousObservablePluginSpiMock>();
    plugin = std::make_shared<AutonomousObservableLocalPluginAdapter>(pluginSpi);
    observer = std::make_shared<AOLPAT_PluginObserverSpiMock>(nullptr);
    exceptionHandler = std::make_shared<AOLPAT_PluginExceptionHandlerMock>(nullptr);
    readerSpi1 = std::make_shared<AOLPAT_ReaderSpiMock>();

    plugin->doRegister();
    plugin->setPluginObservationExceptionHandler(exceptionHandler);
    plugin->addObserver(observer);
}

TEST(AutonomousObservableLocalPluginAdapterTest, onReaderConnected_shouldNotify_andCreateReaders)
{
    setUp();

    /* Start plugin */
    std::vector<std::shared_ptr<ReaderSpi>> readers;
    readers.push_back(readerSpi1);

    /* Register readers */
    plugin->onReaderConnected(readers);

    /* Wait until READER_CONNECTED event, should not take longer than 1 sec */
    std::this_thread::sleep_for(std::chrono::seconds(1));

    /* Check event is well formed */
    const std::shared_ptr<PluginEvent> event =
        observer->getLastEventOfType(PluginEvent::Type::READER_CONNECTED);
    const std::vector<std::string>& eventReaderNames = event->getReaderNames();
    ASSERT_EQ(eventReaderNames.size(), 1);
    ASSERT_TRUE(std::count(eventReaderNames.begin(),
                           eventReaderNames.end(),
                           READER_NAME_1));
    ASSERT_EQ(event->getPluginName(), PLUGIN_NAME);

    const std::vector<std::string>& pluginReaderNames = plugin->getReaderNames();
    ASSERT_EQ(pluginReaderNames.size(), 1);
    ASSERT_TRUE(std::count(pluginReaderNames.begin(),
                           pluginReaderNames.end(),
                           READER_NAME_1));
}

TEST(AutonomousObservableLocalPluginAdapterTest, onReaderDisconnected_shouldNotify_andRemoveReaders)
{
    setUp();

    /* Start plugin */
    std::vector<std::shared_ptr<ReaderSpi>> readers;
    readers.push_back(readerSpi1);

    /* Register readers */
    plugin->onReaderConnected(readers);

    /* Wait until READER_CONNECTED event, should not take longer than 1 sec */
    std::this_thread::sleep_for(std::chrono::seconds(1));

    /* Start plugin */
    std::vector<std::string> readerNames;
    readerNames.push_back(READER_NAME_1);

    /* Register readers */
    plugin->onReaderDisconnected(readerNames);

    /* Wait until READER_DISCONNECTED event, should not take longer than 1 sec */
    std::this_thread::sleep_for(std::chrono::seconds(1));

    /* Check event is well formed */
    const std::shared_ptr<PluginEvent> event =
        observer->getLastEventOfType(PluginEvent::Type::READER_DISCONNECTED);
    const std::vector<std::string>& eventReaderNames = event->getReaderNames();
    ASSERT_EQ(eventReaderNames.size(), 1);
    ASSERT_TRUE(std::count(eventReaderNames.begin(),
                           eventReaderNames.end(),
                           READER_NAME_1));
    ASSERT_EQ(event->getPluginName(), PLUGIN_NAME);

    const std::vector<std::string>& pluginReaderNames = plugin->getReaderNames();
    ASSERT_EQ(pluginReaderNames.size(), 0);
}
