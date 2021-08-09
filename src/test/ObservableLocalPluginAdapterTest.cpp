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
#include "IllegalArgumentException.h"
#include "IllegalStateException.h"
#include "RuntimeException.h"

using namespace testing;

using namespace keyple::core::plugin;
using namespace keyple::core::plugin::spi;
using namespace keyple::core::plugin::spi::reader;
using namespace keyple::core::service;
using namespace keyple::core::util::cpp::exception;

static const std::string PLUGIN_NAME = "plugin";
static const std::string READER_NAME_1 = "reader1";

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

class OLPAT_PluginExceptionHandlerMock final : public PluginObservationExceptionHandlerSpi {
public:
    OLPAT_PluginExceptionHandlerMock(const std::shared_ptr<RuntimeException> throwEx)
    : mInvoked(false), mThrowEx(throwEx) {}

    virtual void onPluginObservationError(const std::string& pluginName,
                                          const std::shared_ptr<Exception> e) override final
    {
        mInvoked = true;
        if (mThrowEx != nullptr) {
            throw *mThrowEx.get();
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

    const std::shared_ptr<Exception> getE() const
    {
        return mE;
    }

private:
    bool mInvoked = false;
    std::string mPluginName;
    std::shared_ptr<Exception> mE;
    const std::shared_ptr<RuntimeException> mThrowEx;
};

static std::shared_ptr<OLPAT_ObservableLocalPluginSpiMock> observablePluginMock;
static std::shared_ptr<ObservableLocalPluginAdapter> pluginAdapter;
static std::shared_ptr<OLPAT_PluginExceptionHandlerMock> exceptionHandlerMock;
static std::shared_ptr<OLPAT_PluginObserverSpiMock> observerMock;

static void setUp()
{
    observablePluginMock =
        std::make_shared<OLPAT_ObservableLocalPluginSpiMock>(PLUGIN_NAME, nullptr);
    observerMock = std::make_shared<OLPAT_PluginObserverSpiMock>(nullptr);
    exceptionHandlerMock = std::make_shared<OLPAT_PluginExceptionHandlerMock>(nullptr);
    pluginAdapter = std::make_shared<ObservableLocalPluginAdapter>(observablePluginMock);
}

static void tearDown()
{
    if (pluginAdapter->isMonitoring()) {
        pluginAdapter->doUnregister();
        ASSERT_FALSE(pluginAdapter->isMonitoring());
    }

    exceptionHandlerMock.reset();
    observerMock.reset();
    observablePluginMock.reset();
    pluginAdapter.reset();
}

TEST(ObservableLocalPluginAdapterTest, addObserver_onUnregisteredPlugin_throwISE)
{
    setUp();

    EXPECT_THROW(pluginAdapter->addObserver(observerMock), IllegalStateException);

    tearDown();
}

TEST(ObservableLocalPluginAdapterTest, addObserver_withNullObserver_throwIAE)
{
    setUp();

    pluginAdapter->doRegister();

    EXPECT_THROW(pluginAdapter->addObserver(nullptr), IllegalArgumentException);

    tearDown();
}

TEST(ObservableLocalPluginAdapterTest, addObserver_withoutExceptionHandler_throwISE)
{
    setUp();

    pluginAdapter->doRegister();

    EXPECT_THROW(pluginAdapter->addObserver(observerMock), IllegalStateException);

    tearDown();
}

// FIXME
// TEST(ObservableLocalPluginAdapterTest, notifyObservers_withEventNotificationExecutorService_isAsync)
// {
//     setUp();

//     pluginAdapter->doRegister();
//     pluginAdapter->setPluginObservationExceptionHandler(exceptionHandlerMock);
//     pluginAdapter->addObserver(observerMock);

//     ASSERT_EQ(pluginAdapter->countObservers(), 1);
//     ASSERT_TRUE(pluginAdapter->isMonitoring());

//     pluginAdapter->setEventNotificationExecutorService(std::make_shared<ExecutorService>());

//     /* Add reader name */
//     observablePluginMock->addReaderName({READER_NAME_1});

//     /* Wait until READER_CONNECTED event, should not take longer than 1 sec */
//     std::this_thread::sleep_for(std::chrono::seconds(1));

//     /* Check if exception has been thrown */
//     ASSERT_EQ(exceptionHandlerMock->getPluginName(), "");
//     ASSERT_EQ(exceptionHandlerMock->getE(), nullptr);

//     tearDown();
// }

// FIXME
// TEST(ObservableLocalPluginAdapterTest, notifyObserver_throwException_isPassedTo_exceptionHandler)
// {
//     setUp();

//     std::shared_ptr<RuntimeException> exception = std::make_shared<RuntimeException>();
//     //exceptionHandlerMock = new PluginExceptionHandlerMock(new RuntimeException()); // Not used
//     observerMock = std::make_shared<OLPAT_PluginObserverSpiMock>(exception);

//     /* Start plugin */
//     pluginAdapter->doRegister();
//     pluginAdapter->setPluginObservationExceptionHandler(exceptionHandlerMock);
//     pluginAdapter->addObserver(observerMock);

//     ASSERT_EQ(pluginAdapter->countObservers(), 1);
//     ASSERT_TRUE(pluginAdapter->isMonitoring());

//     /* Add reader name */
//     observablePluginMock->addReaderName({READER_NAME_1});

//     await().atMost(1, TimeUnit.SECONDS).until(handlerIsInvoked());
//     // when exception handler fails, no error is thrown only logs

//     tearDown();
// }

static inline void addFirstObserver_shouldStartEventThread()
{
    pluginAdapter->doRegister();
    pluginAdapter->setPluginObservationExceptionHandler(exceptionHandlerMock);
    pluginAdapter->addObserver(observerMock);

    ASSERT_EQ(pluginAdapter->countObservers(), 1);
    ASSERT_TRUE(pluginAdapter->isMonitoring());
}

TEST(ObservableLocalPluginAdapterTest, addFirstObserver_shouldStartEventThread)
{
    setUp();

    addFirstObserver_shouldStartEventThread();

    tearDown();
}

TEST(ObservableLocalPluginAdapterTest, removeLastObserver_shouldStopEventThread)
{
    setUp();

    pluginAdapter->doRegister();
    pluginAdapter->setPluginObservationExceptionHandler(exceptionHandlerMock);
    pluginAdapter->addObserver(observerMock);

    ASSERT_EQ(pluginAdapter->countObservers(), 1);
    ASSERT_TRUE(pluginAdapter->isMonitoring());

    pluginAdapter->removeObserver(observerMock);

    ASSERT_EQ(pluginAdapter->countObservers(), 0);
    ASSERT_FALSE(pluginAdapter->isMonitoring());

    tearDown();
}

// FIXME
// TEST(ObservableLocalPluginAdapterTest, clearObserver_shouldStopEventThread)
// {
//     setUp();

//     pluginAdapter->doRegister();
//     pluginAdapter->setPluginObservationExceptionHandler(exceptionHandlerMock);
//     pluginAdapter->addObserver(observerMock);

//     ASSERT_EQ(pluginAdapter->countObservers(), 1);
//     ASSERT_TRUE(pluginAdapter->isMonitoring());

//     pluginAdapter->clearObservers();

//     ASSERT_EQ(pluginAdapter->countObservers(), 0);
//     ASSERT_FALSE(pluginAdapter->isMonitoring());

//     tearDown();
// }

// static inline void whileMonitoring_readerNames_appears_shouldNotify_andCreateReaders()
// {
//     /* Start plugin */
//     addFirstObserver_shouldStartEventThread();

//     /* Add reader name */
//     observablePluginMock->addReaderName({READER_NAME_1});

//     /* Wait until READER_CONNECTED event, should not take longer than 1 sec */
//      std::this_thread::sleep_for(std::chrono::seconds(1));

//     /* Check event is well formed */
//     const std::shared_ptr<PluginEvent> event =
//         observerMock->getLastEventOfType(PluginEvent::Type::READER_CONNECTED);
//     const std::vector<std::string>& readerNames = event->getReaderNames();
//     ASSERT_EQ(readerNames.size(), 1);
//     ASSERT_TRUE(std::count(readerNames.begin(), readerNames.end(), READER_NAME_1));
//     ASSERT_EQ(event->getPluginName(), PLUGIN_NAME);

//     /* Check reader is created */
//     const std::vector<std::string>& pluginReaderNames = pluginAdapter->getReaderNames();
//     ASSERT_TRUE(std::count(pluginReaderNames.begin(), pluginReaderNames.end(), READER_NAME_1));
// }

// FIXME
// TEST(ObservableLocalPluginAdapterTest,
//      whileMonitoring_readerNames_appears_shouldNotify_andCreateReaders)
// {
//     setUp();

//     whileMonitoring_readerNames_appears_shouldNotify_andCreateReaders();

//     tearDown();
// }

// FIXME
// TEST(ObservableLocalPluginAdapterTest,
//      whileMonitoring_readerNames_disappears_shouldNotify_andRemoveReaders)
// {
//     setUp();

//     whileMonitoring_readerNames_appears_shouldNotify_andCreateReaders();

//     /* Remove reader name */
//     observablePluginMock->removeReaderName({READER_NAME_1});

//     /* Wait until READER_DISCONNECTED event, should not take longer than 1 sec */
//     std::this_thread::sleep_for(std::chrono::seconds(1));

//     /* Check event is well formed */
//     const std::shared_ptr<PluginEvent> event =
//         observerMock->getLastEventOfType(PluginEvent::Type::READER_DISCONNECTED);
//     const std::vector<std::string>& readerNames = event->getReaderNames();
//     ASSERT_EQ(readerNames.size(), 1);
//     ASSERT_TRUE(std::count(readerNames.begin(), readerNames.end(), READER_NAME_1));
//     ASSERT_EQ(event->getPluginName(), PLUGIN_NAME);

//     tearDown();
// }

// FIXME
// TEST(ObservableLocalPluginAdapterTest,
//      whileMonitoring_observerThrowException_isPassedTo_exceptionHandler)
// {
//     setUp();

//     const auto exception = std::make_shared<RuntimeException>();
//     observerMock = std::make_shared<OLPAT_PluginObserverSpiMock>(exception);

//     /* Start plugin */
//     addFirstObserver_shouldStartEventThread();

//     /* Add reader name */
//     observablePluginMock->addReaderName({READER_NAME_1});

//     /* Wait until handlerIsInvoked() event, should not take longer than 1 sec */
//     std::this_thread::sleep_for(std::chrono::seconds(1));

//     /* Check if exception has been thrown */
//     ASSERT_EQ(exceptionHandlerMock->getPluginName(), PLUGIN_NAME);
//     ASSERT_EQ(exceptionHandlerMock->getE(), exception);

//     tearDown();
// }

// FIXME
// TEST(ObservableLocalPluginAdapterTest,
//      whileMonitoring_pluginThrowException_isPassedTo_exceptionHandler)
// {
//     const auto exception = std::make_shared<PluginIOException>("error");
//     observablePluginMock =
//         std::make_shared<OLPAT_ObservableLocalPluginSpiMock>(PLUGIN_NAME, exception);
//     pluginAdapter = std::make_shared<ObservableLocalPluginAdapter>(observablePluginMock);

//     /* Start plugin */
//     pluginAdapter->doRegister();
//     pluginAdapter->setPluginObservationExceptionHandler(exceptionHandlerMock);
//     pluginAdapter->addObserver(observerMock);

//     /* Wait until handlerIsInvoked() event, should not take longer than 1 sec */
//     std::this_thread::sleep_for(std::chrono::seconds(1));

//     /* Check if exception has been thrown */
//     ASSERT_EQ(exceptionHandlerMock->getPluginName(), PLUGIN_NAME);
//     ASSERT_EQ(exceptionHandlerMock->getE()->getCause(), exception->getCause());

//     tearDown();
// }
