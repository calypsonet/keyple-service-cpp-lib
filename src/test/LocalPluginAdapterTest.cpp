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

/* Keyple Core Common */
#include "KeyplePluginExtension.h"

/* Keyple Core Plugin */
#include "DontWaitForCardRemovalDuringProcessingSpi.h"
#include "ObservableReaderSpi.h"
#include "PluginIOException.h"
#include "PluginSpi.h"
#include "WaitForCardInsertionBlockingSpi.h"
#include "WaitForCardRemovalBlockingSpi.h"

/* Keyple Core Service */
#include "LocalPluginAdapter.h"
#include "LocalReaderAdapter.h"
#include "ObservableReader.h"
#include "ObservableLocalReaderAdapter.h"

/* Mock */
#include "ObservableReaderSpiMock.h"
#include "PluginSpiMock.h"
#include "ReaderSpiMock.h"

using namespace testing;

using namespace keyple::core::common;
using namespace keyple::core::plugin;
using namespace keyple::core::plugin::spi;
using namespace keyple::core::plugin::spi::reader::observable;
using namespace keyple::core::plugin::spi::reader::observable::state::insertion;
using namespace keyple::core::plugin::spi::reader::observable::state::processing;
using namespace keyple::core::plugin::spi::reader::observable::state::removal;
using namespace keyple::core::service;

static const std::string PLUGIN_NAME = "plugin";
static const std::string READER_NAME_1 = "reader1";
static const std::string READER_NAME_2 = "reader2";
static const std::string OBSERVABLE_READER_NAME = "observableReader";

static std::shared_ptr<PluginSpiMock> pluginSpi;
static std::shared_ptr<ReaderSpiMock> readerSpi1;
static std::shared_ptr<ReaderSpiMock> readerSpi2;
static std::shared_ptr<ObservableReaderSpiMock> observableReader;

static void setUp()
{
    pluginSpi = std::make_shared<PluginSpiMock>();
    EXPECT_CALL(*pluginSpi.get(), getName()).WillRepeatedly(ReturnRef(PLUGIN_NAME));
    EXPECT_CALL(*pluginSpi.get(), onUnregister()).WillRepeatedly(Return());

    readerSpi1 = std::make_shared<ReaderSpiMock>(READER_NAME_1);
    EXPECT_CALL(*readerSpi1.get(), onUnregister()).WillRepeatedly(Return());

    readerSpi2 = std::make_shared<ReaderSpiMock>(READER_NAME_2);
    EXPECT_CALL(*readerSpi2.get(), onUnregister()).WillRepeatedly(Return());

    observableReader = std::make_shared<ObservableReaderSpiMock>(OBSERVABLE_READER_NAME);
    EXPECT_CALL(*observableReader.get(), onUnregister()).WillRepeatedly(Return());
}

static void tearDown()
{
    pluginSpi.reset();
    readerSpi1.reset();
    readerSpi2.reset();
    observableReader.reset();
}

TEST(LocalPluginAdapterTest, register_whenSearchReaderFails_shouldPIO)
{
    setUp();

    EXPECT_CALL(*pluginSpi.get(), searchAvailableReaders())
        .Times(1)
        .WillOnce(Throw(PluginIOException("Plugin IO Exception")));

    LocalPluginAdapter localPluginAdapter(pluginSpi);

    EXPECT_THROW(localPluginAdapter.doRegister(), PluginIOException);

    tearDown();
}

TEST(LocalPluginAdapterTest, register_whenSearchReaderReturnsReader_shouldRegisterReader)
{
    setUp();

    std::vector<std::shared_ptr<ReaderSpi>> readerSpis;
    readerSpis.push_back(readerSpi1);
    readerSpis.push_back(readerSpi2);

    EXPECT_CALL(*pluginSpi.get(), searchAvailableReaders()).WillRepeatedly(Return(readerSpis));

    LocalPluginAdapter localPluginAdapter(pluginSpi);
    ASSERT_EQ(localPluginAdapter.getName(), PLUGIN_NAME);

    localPluginAdapter.doRegister();
    localPluginAdapter.checkStatus();

    const std::vector<std::string> readerNames = localPluginAdapter.getReaderNames();
    auto it = std::find(readerNames.begin(), readerNames.end(), READER_NAME_1);
    ASSERT_NE(it, readerNames.end());
    it = std::find(readerNames.begin(), readerNames.end(), READER_NAME_2);
    ASSERT_NE(it, readerNames.end());

    const std::vector<std::shared_ptr<Reader>> readers = localPluginAdapter.getReaders();
    ASSERT_EQ(readers.size(), 2);
    auto itt = std::find(readers.begin(),
                         readers.end(),
                         localPluginAdapter.getReader(READER_NAME_1));
    ASSERT_NE(itt, readers.end());
    itt = std::find(readers.begin(), readers.end(), localPluginAdapter.getReader(READER_NAME_2));
    ASSERT_NE(itt, readers.end());

    const auto& reader1 = localPluginAdapter.getReader(READER_NAME_1);
    const auto reader1Class = std::dynamic_pointer_cast<Reader>(reader1);
    ASSERT_NE(reader1Class, nullptr);
    const auto localReader1Class = std::dynamic_pointer_cast<LocalReaderAdapter>(reader1);
    ASSERT_NE(localReader1Class, nullptr);

    const auto& reader2 = localPluginAdapter.getReader(READER_NAME_2);
    const auto reader2Class = std::dynamic_pointer_cast<Reader>(reader2);
    ASSERT_NE(reader2Class, nullptr);
    const auto localReader2Class = std::dynamic_pointer_cast<LocalReaderAdapter>(reader2);
    ASSERT_NE(localReader2Class, nullptr);

    ASSERT_NE(reader1, reader2);

    tearDown();
}

TEST(LocalPluginAdapterTest,
     register_whenSearchReaderReturnsObservableReader_shouldRegisterObservableReader)
{
    setUp();

    std::vector<std::shared_ptr<ReaderSpi>> readerSpis;
    readerSpis.push_back(observableReader);

    EXPECT_CALL(*pluginSpi.get(), searchAvailableReaders()).WillRepeatedly(Return(readerSpis));

    LocalPluginAdapter localPluginAdapter(pluginSpi);
    localPluginAdapter.doRegister();
    localPluginAdapter.checkStatus();

    const std::vector<std::string> readerNames = localPluginAdapter.getReaderNames();
    auto it = std::find(readerNames.begin(), readerNames.end(), OBSERVABLE_READER_NAME);
    ASSERT_NE(it, readerNames.end());

    const std::vector<std::shared_ptr<Reader>> readers = localPluginAdapter.getReaders();
    ASSERT_EQ(readers.size(), 1);

    const auto& reader = localPluginAdapter.getReader(OBSERVABLE_READER_NAME);
    const auto readerClass = std::dynamic_pointer_cast<ObservableReader>(reader);
    ASSERT_NE(readerClass, nullptr);
    const auto localReaderClass = std::dynamic_pointer_cast<ObservableLocalReaderAdapter>(reader);
    ASSERT_NE(localReaderClass, nullptr);

    tearDown();
}

TEST(LocalPluginAdapterTest, getReaders_whenNotRegistered_shouldISE)
{
    setUp();

    LocalPluginAdapter localPluginAdapter(pluginSpi);

    EXPECT_THROW(localPluginAdapter.getReaders(), IllegalStateException);

    tearDown();
}

TEST(LocalPluginAdapterTest, getReader_whenNotRegistered_shouldISE)
{
    setUp();

    LocalPluginAdapter localPluginAdapter(pluginSpi);

    EXPECT_THROW(localPluginAdapter.getReader(READER_NAME_1), IllegalStateException);

    tearDown();
}

TEST(LocalPluginAdapterTest, getReaderNames_whenNotRegistered_shouldISE)
{
    setUp();

    LocalPluginAdapter localPluginAdapter(pluginSpi);

    EXPECT_THROW(localPluginAdapter.getReaderNames(), IllegalStateException);

    tearDown();
}

TEST(LocalPluginAdapterTest, unregister_shouldDisableMethodsWithISE)
{
    setUp();

    std::vector<std::shared_ptr<ReaderSpi>> readerSpis;
    readerSpis.push_back(readerSpi1);

    EXPECT_CALL(*pluginSpi.get(), searchAvailableReaders()).WillRepeatedly(Return(readerSpis));

    LocalPluginAdapter localPluginAdapter(pluginSpi);
    localPluginAdapter.doRegister();
    localPluginAdapter.doUnregister();

    EXPECT_THROW(localPluginAdapter.getReaders(), IllegalStateException);

    tearDown();
}

TEST(LocalPluginAdapterTest, getExtension_whenNotRegistered_shouldISE)
{
    setUp();

    LocalPluginAdapter localPluginAdapter(pluginSpi);

    EXPECT_THROW(localPluginAdapter.getExtension(typeid(PluginSpiMock)),
                 IllegalStateException);

    tearDown();
}

TEST(LocalPluginAdapterTest, getExtension_whenRegistered_shouldReturnExtension)
{
    setUp();

    std::vector<std::shared_ptr<ReaderSpi>> readerSpis;
    EXPECT_CALL(*pluginSpi.get(), searchAvailableReaders()).WillRepeatedly(Return(readerSpis));

    LocalPluginAdapter localPluginAdapter(pluginSpi);
    localPluginAdapter.doRegister();

    const auto extension = localPluginAdapter.getExtension(typeid(PluginSpiMock));
    const auto pluginMock = std::static_pointer_cast<PluginSpiMock>(extension);
    ASSERT_NE(pluginMock, nullptr);

    tearDown();
}
