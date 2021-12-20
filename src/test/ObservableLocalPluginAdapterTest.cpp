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
#include "ReaderSpi.h"

/* Keyple Core Service */
#include "ObservableLocalPluginAdapter.h"

/* Keyple Core Util */
#include "IllegalArgumentException.h"
#include "IllegalStateException.h"
#include "RuntimeException.h"

/* Mock */
#include "ObservableLocalPluginSpiMock.h"
#include "PluginObservationExceptionHandlerSpiMock.h"
#include "PluginObserverSpiMock.h"
#include "ReaderSpiMock.h"

using namespace testing;

using namespace keyple::core::plugin;
using namespace keyple::core::plugin::spi;
using namespace keyple::core::plugin::spi::reader;
using namespace keyple::core::service;
using namespace keyple::core::util::cpp::exception;

static const std::string PLUGIN_NAME = "plugin";
static const std::string READER_NAME_1 = "reader1";

static std::shared_ptr<ObservableLocalPluginSpiMock> observablePluginMock;
static std::shared_ptr<ObservableLocalPluginAdapter> pluginAdapter;
static std::shared_ptr<PluginObservationExceptionHandlerSpiMock> exceptionHandlerMock;
static std::shared_ptr<PluginObserverSpiMock> observerMock;

static void setUp()
{
    observablePluginMock = std::make_shared<ObservableLocalPluginSpiMock>(PLUGIN_NAME, nullptr);
    observerMock = std::make_shared<PluginObserverSpiMock>(nullptr);
    
    exceptionHandlerMock = std::make_shared<PluginObservationExceptionHandlerSpiMock>(nullptr);
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
    pluginAdapter.reset();
    observablePluginMock.reset();
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

TEST(ObservableLocalPluginAdapterTest, notifyObserver_throwException_isPassedTo_exceptionHandler)
{
    setUp();

    std::shared_ptr<RuntimeException> exception = std::make_shared<RuntimeException>();
    exceptionHandlerMock = std::make_shared<PluginObservationExceptionHandlerSpiMock>(
                               std::make_shared<RuntimeException>()); // Not used
    observerMock = std::make_shared<PluginObserverSpiMock>(exception);

    /* Start plugin */
    pluginAdapter->doRegister();
    pluginAdapter->setPluginObservationExceptionHandler(exceptionHandlerMock);
    pluginAdapter->addObserver(observerMock);

    ASSERT_EQ(pluginAdapter->countObservers(), 1);
    ASSERT_TRUE(pluginAdapter->isMonitoring());

    /* Add reader name */
    observablePluginMock->addReaderName({READER_NAME_1});

    /* Wait until handlerIsInvoked() event, should not take longer than 1 sec */
    std::this_thread::sleep_for(std::chrono::seconds(1));

    /* When exception handler fails, no error is thrown only logs */

    tearDown();
}

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

TEST(ObservableLocalPluginAdapterTest, clearObserver_shouldStopEventThread)
{
    setUp();

    pluginAdapter->doRegister();
    pluginAdapter->setPluginObservationExceptionHandler(exceptionHandlerMock);
    pluginAdapter->addObserver(observerMock);

    ASSERT_EQ(pluginAdapter->countObservers(), 1);
    ASSERT_TRUE(pluginAdapter->isMonitoring());

    pluginAdapter->clearObservers();

    ASSERT_EQ(pluginAdapter->countObservers(), 0);
    ASSERT_FALSE(pluginAdapter->isMonitoring());

    tearDown();
}

static inline void whileMonitoring_readerNames_appears_shouldNotify_andCreateReaders()
{
    /* Start plugin */
    addFirstObserver_shouldStartEventThread();

    /* Add reader name */
    observablePluginMock->addReaderName({READER_NAME_1});

    /* Wait until READER_CONNECTED event, should not take longer than 1 sec */
     std::this_thread::sleep_for(std::chrono::seconds(1));

    /* Check event is well formed */
    const std::shared_ptr<PluginEvent> event =
        observerMock->getLastEventOfType(PluginEvent::Type::READER_CONNECTED);
    const std::vector<std::string>& readerNames = event->getReaderNames();
    ASSERT_EQ(readerNames.size(), 1);
    ASSERT_TRUE(std::count(readerNames.begin(), readerNames.end(), READER_NAME_1));
    ASSERT_EQ(event->getPluginName(), PLUGIN_NAME);

    /* Check reader is created */
    const std::vector<std::string>& pluginReaderNames = pluginAdapter->getReaderNames();
    ASSERT_TRUE(std::count(pluginReaderNames.begin(), pluginReaderNames.end(), READER_NAME_1));
}

TEST(ObservableLocalPluginAdapterTest,
     whileMonitoring_readerNames_appears_shouldNotify_andCreateReaders)
{
    setUp();

    whileMonitoring_readerNames_appears_shouldNotify_andCreateReaders();

    tearDown();
}

TEST(ObservableLocalPluginAdapterTest,
     whileMonitoring_readerNames_disappears_shouldNotify_andRemoveReaders)
{
    setUp();

    whileMonitoring_readerNames_appears_shouldNotify_andCreateReaders();

    /* Remove reader name */
    observablePluginMock->removeReaderName({READER_NAME_1});

    /* Wait until READER_DISCONNECTED event, should not take longer than 1 sec */
    std::this_thread::sleep_for(std::chrono::seconds(1));

    /* Check event is well formed */
    const std::shared_ptr<PluginEvent> event =
        observerMock->getLastEventOfType(PluginEvent::Type::READER_DISCONNECTED);
    const std::vector<std::string>& readerNames = event->getReaderNames();
    ASSERT_EQ(readerNames.size(), 1);
    ASSERT_TRUE(std::count(readerNames.begin(), readerNames.end(), READER_NAME_1));
    ASSERT_EQ(event->getPluginName(), PLUGIN_NAME);

    tearDown();
}

TEST(ObservableLocalPluginAdapterTest,
     whileMonitoring_observerThrowException_isPassedTo_exceptionHandler)
{
    setUp();

    const auto exception = std::make_shared<RuntimeException>();
    observerMock = std::make_shared<PluginObserverSpiMock>(exception);

    /* Start plugin */
    addFirstObserver_shouldStartEventThread();

    /* Add reader name */
    observablePluginMock->addReaderName({READER_NAME_1});

    /* Wait until handlerIsInvoked() event, should not take longer than 1 sec */
    std::this_thread::sleep_for(std::chrono::seconds(1));

    /* Check if exception has been thrown */
    ASSERT_EQ(exceptionHandlerMock->getPluginName(), PLUGIN_NAME);
    ASSERT_EQ(*exceptionHandlerMock->getE().get(), *exception.get());

    tearDown();
}

TEST(ObservableLocalPluginAdapterTest,
     whileMonitoring_pluginThrowException_isPassedTo_exceptionHandler)
{
    setUp();

    const auto exception = std::make_shared<PluginIOException>("error");
    observablePluginMock = std::make_shared<ObservableLocalPluginSpiMock>(PLUGIN_NAME, exception);
    pluginAdapter = std::make_shared<ObservableLocalPluginAdapter>(observablePluginMock);

    /* Start plugin */
    pluginAdapter->doRegister();
    pluginAdapter->setPluginObservationExceptionHandler(exceptionHandlerMock);
    pluginAdapter->addObserver(observerMock);

    /* Wait until handlerIsInvoked() event, should not take longer than 1 sec */
    std::this_thread::sleep_for(std::chrono::seconds(1));

    /* Check if exception has been thrown */
    ASSERT_EQ(exceptionHandlerMock->getPluginName(), PLUGIN_NAME);
    ASSERT_EQ(*exceptionHandlerMock->getE()->getCause().get(), *exception.get());

    tearDown();
}
