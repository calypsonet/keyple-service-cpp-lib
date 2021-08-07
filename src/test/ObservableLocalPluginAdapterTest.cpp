/**************************************************************************************************
 * Copyright (c) 2021 Calypso Networks Association https://calypsonet.org/                        *
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

/* Keyple Core Plugin */
#include "ObservablePluginSpi.h"
#include "PluginIOException.h"
#include "ReaderSpi.h"

/* Keyple Core Service */
#include "ObservableLocalPluginAdapter.h"

/* Keyple Core Util */
#include "RuntimeException.h"

using namespace testing;

using namespace keyple::core::plugin;
using namespace keyple::core::plugin::spi;
using namespace keyple::core::plugin::spi::reader;
using namespace keyple::core::service;
using namespace keyple::core::util::cpp::exception;

static const std::string PLUGIN_NAME = "plugin";

class OLPAT_ReaderSpiMock final : public ReaderSpi {
public:
    MOCK_METHOD((const std::string&),
                getName,
                (),
                (const, override, final));

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
        return std::vector<uint8_t>();
    }

    virtual bool isContactless() override final
    {
        return true;
    }

    virtual void onUnregister() override final {}
};

class OLPAT_ObservableLocalPluginSpiMock final : public ObservablePluginSpi {
public:
    OLPAT_ObservableLocalPluginSpiMock(const std::string& name,
                                       const std::shared_ptr<PluginIOException> pluginError)
    : mName(name), mPluginError(pluginError) {}

    virtual int getMonitoringCycleDuration() const override final
    {
        return mMonitoringCycleDuration;
    }

    virtual const std::vector<std::string> searchAvailableReaderNames() override final
    {
        if (mPluginError != nullptr) {
            throw *mPluginError.get();
        }

        std::vector<std::string> readerNames;
        for (const auto& reader : mStubReaders) {
            readerNames.push_back(reader.first);
        }

        return readerNames;
    }

    virtual std::shared_ptr<ReaderSpi> searchReader(const std::string& readerName) override final
    {
        if (mPluginError != nullptr) {
            throw *mPluginError.get();
        }

        const auto it = mStubReaders.find(readerName);
        if (it != mStubReaders.end()) {
            return it->second;
        }

        return nullptr;
    }

    virtual const std::string& getName() const override final
    {
        return mName;
    }

    virtual const std::vector<std::shared_ptr<ReaderSpi>> searchAvailableReaders() const override
        final
    {
        std::vector<std::shared_ptr<ReaderSpi>> readers;
        for (const auto& reader : mStubReaders) {
            readers.push_back(reader.second);
        }
        return readers;
    }

    virtual void onUnregister() override final {}

    void addReaderName(const std::vector<std::string>& names)
    {
        for (const auto& readerName : names) {
            auto readerSpi = std::make_shared<OLPAT_ReaderSpiMock>();
            EXPECT_CALL(*readerSpi.get(), getName())
                .WillRepeatedly(ReturnRef(readerName));
            mStubReaders.insert({readerName, readerSpi});
        }
    }

    void removeReaderName(const std::vector<std::string>& names)
    {
        for (const auto& readerName : names) {
            mStubReaders.erase(readerName);
        }
    }

private:
    const std::string mName;
    const int mMonitoringCycleDuration = 0;
    std::map<std::string, std::shared_ptr<ReaderSpi>> mStubReaders;
    const std::shared_ptr<PluginIOException> mPluginError;
};

class OLPAT_PluginObserverSpiMock final : public PluginObserverSpi {
public:
    OLPAT_PluginObserverSpiMock(const std::shared_ptr<RuntimeException> e) : mThrowEx(e) {}

    virtual void onPluginEvent(std::shared_ptr<PluginEvent> pluginEvent) override final
    {
        mEventTypeReceived.insert({pluginEvent->getType(), pluginEvent});
        if (mThrowEx != nullptr) {
            throw *mThrowEx.get();
        }
    }

    bool hasReceived(const PluginEvent::Type eventType)
    {
        return mEventTypeReceived.find(eventType) != mEventTypeReceived.end();
    }

    const std::shared_ptr<PluginEvent> getLastEventOfType(const PluginEvent::Type eventType)
    {
        return mEventTypeReceived.find(eventType) != mEventTypeReceived.end() ?
                   mEventTypeReceived.at(eventType) : nullptr;
    }

private:
    std::map<PluginEvent::Type, std::shared_ptr<PluginEvent>> mEventTypeReceived;
    std::shared_ptr<RuntimeException> mThrowEx;
};

static std::shared_ptr<ObservablePluginSpi> observablePluginMock;
static std::shared_ptr<ObservableLocalPluginAdapter> pluginAdapter;
static std::shared_ptr<PluginObserverSpi> observerMock;

static void setUp()
{
    observablePluginMock =
        std::make_shared<OLPAT_ObservableLocalPluginSpiMock>(PLUGIN_NAME, nullptr);
    observerMock = std::make_shared<OLPAT_PluginObserverSpiMock>(nullptr);
    //exceptionHandlerMock = new PluginExceptionHandlerMock(null);
    pluginAdapter = std::make_shared<ObservableLocalPluginAdapter>(observablePluginMock);
}

static void tearDown()
{
    if (pluginAdapter->isMonitoring()) {
        pluginAdapter->doUnregister();
        ASSERT_FALSE(pluginAdapter->isMonitoring());
    }
}

TEST(ObservableLocalPluginAdapterTest, addObserver_onUnregisteredPlugin_throwISE)
{
    setUp();

    EXPECT_THROW(pluginAdapter->addObserver(observerMock), IllegalStateException);

    tearDown();
}
