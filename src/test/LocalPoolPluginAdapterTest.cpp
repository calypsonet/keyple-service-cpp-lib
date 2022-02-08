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
#include "DontWaitForCardRemovalDuringProcessingSpi.h"
#include "KeyplePluginException.h"
#include "ObservableReaderSpi.h"
#include "PluginIOException.h"
#include "PluginSpi.h"
#include "WaitForCardInsertionBlockingSpi.h"
#include "WaitForCardRemovalBlockingSpi.h"

/* Keyple Core Service */
#include "LocalPluginAdapter.h"
#include "LocalPoolPluginAdapter.h"
#include "LocalReaderAdapter.h"
#include "ObservableReader.h"
#include "ObservableLocalReaderAdapter.h"

/* Mock */
#include "ObservableReaderSpiMock.h"
#include "PluginSpiMock.h"
#include "PoolPluginSpiMock.h"
#include "ReaderSpiMock.h"

using namespace testing;

using namespace keyple::core::plugin;
using namespace keyple::core::plugin::spi;
using namespace keyple::core::plugin::spi::reader::observable;
using namespace keyple::core::plugin::spi::reader::observable::state::insertion;
using namespace keyple::core::plugin::spi::reader::observable::state::processing;
using namespace keyple::core::plugin::spi::reader::observable::state::removal;
using namespace keyple::core::service;

const std::string READER_NAME_1 = "reader1";
const std::string READER_NAME_2 = "reader2";
const std::string OBSERVABLE_READER_NAME = "observableReader";
const std::string PLUGIN_NAME = "plugin";
const std::string POOL_PLUGIN_NAME = "poolPlugin";
const std::string GROUP_1 = "group1";
const std::string GROUP_2 = "group2";
const std::string GROUP_3 = "group3";

static std::shared_ptr<ReaderSpiMock> readerSpi1;
static std::shared_ptr<ReaderSpiMock> readerSpi2;
static std::shared_ptr<ObservableReaderSpiMock> observableReader;
static std::shared_ptr<PoolPluginSpiMock> poolPluginSpi;
static std::vector<std::string> groupReference({GROUP_1, GROUP_2});

static void setUp()
{
    readerSpi1 = std::make_shared<ReaderSpiMock>(READER_NAME_1);
    EXPECT_CALL(*readerSpi1.get(), onUnregister()).WillRepeatedly(Return());

    readerSpi2 = std::make_shared<ReaderSpiMock>(READER_NAME_2);
    EXPECT_CALL(*readerSpi2.get(), onUnregister()).WillRepeatedly(Return());

    observableReader = std::make_shared<ObservableReaderSpiMock>(OBSERVABLE_READER_NAME);
    EXPECT_CALL(*observableReader.get(), onUnregister()).WillRepeatedly(Return());

    poolPluginSpi = std::make_shared<PoolPluginSpiMock>();
    EXPECT_CALL(*poolPluginSpi.get(), getName()).WillRepeatedly(ReturnRef(PLUGIN_NAME));
    EXPECT_CALL(*poolPluginSpi.get(), allocateReader(GROUP_1)).WillRepeatedly(Return(readerSpi1));
    EXPECT_CALL(*poolPluginSpi.get(), allocateReader(GROUP_2)).WillRepeatedly(Return(readerSpi2));
    EXPECT_CALL(*poolPluginSpi.get(), releaseReader(_)).WillRepeatedly(Return());
    EXPECT_CALL(*poolPluginSpi.get(), onUnregister()).WillRepeatedly(Return());
    EXPECT_CALL(*poolPluginSpi.get(), getReaderGroupReferences()).WillRepeatedly(Return(groupReference));
}

static void tearDown()
{
    observableReader.reset();
    poolPluginSpi.reset();
    readerSpi1.reset();
    readerSpi2.reset();
}

TEST(LocalPoolPluginAdapterTest, getReaderGroupReferences_whenGettingReferencesFails_shouldKPE)
{
    setUp();

    EXPECT_CALL(*poolPluginSpi.get(), getReaderGroupReferences())
        .Times(1)
        .WillOnce(Throw(PluginIOException("Plugin IO Exception")));

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    EXPECT_THROW(localPluginAdapter.getReaderGroupReferences(), KeyplePluginException);

    tearDown();
}

TEST(LocalPoolPluginAdapterTest, getReaderGroupReferences_whenNotRegistered_shouldISE)
{
    setUp();

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);

    EXPECT_THROW(localPluginAdapter.getReaderGroupReferences(), IllegalStateException);
}

TEST(LocalPoolPluginAdapterTest, getReaderGroupReferences_whenSucceeds_shouldReturnReferences)
{
    setUp();

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    const std::vector<std::string>& groupReferences = localPluginAdapter.getReaderGroupReferences();
    ASSERT_EQ(groupReferences.size(), 2);
    ASSERT_TRUE(std::count(groupReferences.begin(), groupReferences.end(), GROUP_1));
    ASSERT_TRUE(std::count(groupReferences.begin(), groupReferences.end(), GROUP_2));

    tearDown();
}

TEST(LocalPoolPluginAdapterTest, allocateReader_whenNotRegistered_shouldISE)
{
    setUp();

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);

    EXPECT_THROW(localPluginAdapter.allocateReader(GROUP_1), IllegalStateException);

    tearDown();
}

TEST(LocalPoolPluginAdapterTest, allocateReader_whenAllocatingReaderFails_shouldKPE)
{
    setUp();

    EXPECT_CALL(*poolPluginSpi.get(), allocateReader(_))
        .Times(1)
        .WillOnce(Throw(PluginIOException("Plugin IO Exception")));

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    EXPECT_THROW(localPluginAdapter.allocateReader(GROUP_1), KeyplePluginException);

    tearDown();
}

TEST(LocalPoolPluginAdapterTest, allocateReader_whenSucceeds_shouldReturnReader)
{
    setUp();

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    std::shared_ptr<Reader> reader = localPluginAdapter.allocateReader(GROUP_1);
    ASSERT_EQ(reader->getName(), READER_NAME_1);
    ASSERT_NE(std::dynamic_pointer_cast<Reader>(reader), nullptr);
    ASSERT_NE(std::dynamic_pointer_cast<LocalReaderAdapter>(reader), nullptr);

    const std::vector<std::string>& readerNames = localPluginAdapter.getReaderNames();
    ASSERT_EQ(readerNames.size(), 1);
    ASSERT_TRUE(std::count(readerNames.begin(),
                           readerNames.end(),
                           READER_NAME_1));

    const std::vector<std::shared_ptr<Reader>>& readers = localPluginAdapter.getReaders();
    ASSERT_EQ(readers.size(), 1);
    ASSERT_TRUE(std::count(readers.begin(),
                           readers.end(),
                           localPluginAdapter.getReader(READER_NAME_1)));

    tearDown();
}

TEST(LocalPoolPluginAdapterTest, allocateReader_whenReaderIsObservable_shouldReturnObservableReader)
{
    setUp();

    EXPECT_CALL(*poolPluginSpi.get(), allocateReader(GROUP_3))
        .Times(1)
        .WillOnce(Return(observableReader));

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    std::shared_ptr<Reader> reader = localPluginAdapter.allocateReader(GROUP_3);
    ASSERT_EQ(reader->getName(), OBSERVABLE_READER_NAME);
    ASSERT_NE(std::dynamic_pointer_cast<ObservableLocalReaderAdapter>(reader), nullptr);

    const std::vector<std::shared_ptr<Reader>>& readers = localPluginAdapter.getReaders();
    ASSERT_EQ(readers.size(), 1);
    ASSERT_TRUE(std::count(readers.begin(),
                           readers.end(),
                           localPluginAdapter.getReader(OBSERVABLE_READER_NAME)));

    tearDown();
}

TEST(LocalPoolPluginAdapterTest, releaseReader_whenNotRegistered_shouldISE)
{
    setUp();

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    std::shared_ptr<Reader> reader = localPluginAdapter.allocateReader(GROUP_1);
    localPluginAdapter.doUnregister();

    EXPECT_THROW(localPluginAdapter.releaseReader(reader), IllegalStateException);

    tearDown();
}

TEST(LocalPoolPluginAdapterTest, releaseReader_whenSucceeds_shouldRemoveReader)
{
    setUp();

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    std::shared_ptr<Reader> reader = localPluginAdapter.allocateReader(GROUP_1);
    localPluginAdapter.releaseReader(reader);

    ASSERT_EQ(localPluginAdapter.getReaderNames().size(), 0);
    ASSERT_EQ(localPluginAdapter.getReaders().size(), 0);

    tearDown();
}

TEST(LocalPoolPluginAdapterTest, releaseReader_whenReleaseReaderFails_shouldKPE_and_RemoveReader)
{
    setUp();

    EXPECT_CALL(*poolPluginSpi.get(), releaseReader(_))
        .Times(1)
        .WillOnce(Throw(PluginIOException("Plugin IO Exception")));

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    std::shared_ptr<Reader> reader = localPluginAdapter.allocateReader(GROUP_1);

    EXPECT_THROW(localPluginAdapter.releaseReader(reader), KeyplePluginException);
    ASSERT_EQ(localPluginAdapter.getReaderNames().size(), 0);
    ASSERT_EQ(localPluginAdapter.getReaders().size(), 0);

    tearDown();
}
