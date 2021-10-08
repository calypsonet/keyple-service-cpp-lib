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

#include <chrono>
#include <thread>

/* Keyple Core Service */
#include "AutonomousObservableLocalPluginAdapter.h"

/* Keyple Core Util */
#include "RuntimeException.h"

/* Mock */
#include "AutonomousObservablePluginSpiMock.h"
#include "PluginObservationExceptionHandlerMock.h"
#include "PluginObserverSpiMock.h"
#include "ReaderSpiMock.h"

using namespace testing;

using namespace keyple::core::plugin::spi;
using namespace keyple::core::service;
using namespace keyple::core::service::spi;
using namespace keyple::core::util::cpp::exception;

const std::string PLUGIN_NAME = "plugin";
const std::string READER_NAME_1 = "reader1";

static std::shared_ptr<AutonomousObservablePluginSpiMock> pluginSpi;
static std::shared_ptr<AutonomousObservableLocalPluginAdapter> plugin;
static std::shared_ptr<PluginObserverSpiMock> observer;
static std::shared_ptr<PluginObservationExceptionHandlerMock> exceptionHandler;
static std::shared_ptr<ReaderSpiMock> readerSpi1;

static void setUp()
{
    pluginSpi = std::make_shared<AutonomousObservablePluginSpiMock>();
    EXPECT_CALL(*pluginSpi.get(), getName()).WillRepeatedly(ReturnRef(PLUGIN_NAME));
    EXPECT_CALL(*pluginSpi.get(), connect(_)).WillRepeatedly(Return());
    EXPECT_CALL(*pluginSpi.get(), searchAvailableReaders())
        .WillRepeatedly(Return(std::vector<std::shared_ptr<ReaderSpi>>()));

    plugin = std::make_shared<AutonomousObservableLocalPluginAdapter>(pluginSpi);
    observer = std::make_shared<PluginObserverSpiMock>(nullptr);
    exceptionHandler = std::make_shared<PluginObservationExceptionHandlerMock>(nullptr);

    readerSpi1 = std::make_shared<ReaderSpiMock>(READER_NAME_1);
    EXPECT_CALL(*readerSpi1.get(), onUnregister()).WillRepeatedly(Return());

    plugin->doRegister();
    plugin->setPluginObservationExceptionHandler(exceptionHandler);
    plugin->addObserver(observer);
}

static void tearDown()
{
    pluginSpi.reset();
    plugin.reset();
    readerSpi1.reset();
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

    tearDown();
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

    tearDown();
}
