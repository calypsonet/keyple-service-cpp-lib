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

/* Keyple Core Util */
#include "IllegalStateException.h"

/* Mock */
#include "AbstractReaderAdapterMock.h"
#include "CardRequestSpiMock.h"
#include "CardResponseApiMock.h"
#include "ReaderSpiMock.h"

using namespace testing;

using namespace calypsonet::terminal::card::spi;
using namespace keyple::core::common;
using namespace keyple::core::plugin::spi::reader;
using namespace keyple::core::service;
using namespace keyple::core::util::cpp::exception;

static const std::string PLUGIN_NAME = "plugin";
static const std::string READER_NAME = "reader";

static std::shared_ptr<ReaderSpiMock> readerSpi;
static std::shared_ptr<CardRequestSpiMock> cardRequestSpi;
static std::shared_ptr<AbstractReaderAdapterMock> readerAdapter;

static void setUp()
{
    readerSpi = std::make_shared<ReaderSpiMock>();
    cardRequestSpi = std::make_shared<CardRequestSpiMock>();
    readerAdapter =
        std::make_shared<AbstractReaderAdapterMock>(
            READER_NAME,
            std::dynamic_pointer_cast<KeypleReaderExtension>(readerSpi),
            PLUGIN_NAME);
}

static void tearDown()
{
    readerAdapter.reset();
    cardRequestSpi.reset();
    readerSpi.reset();
}

TEST(AbstractReaderAdapterTest, getPluginName_shouldReturnPluginName)
{
    setUp();

    ASSERT_EQ(readerAdapter->getPluginName(), PLUGIN_NAME);

    tearDown();
}

TEST(AbstractReaderAdapterTest, getName_shouldReturnReaderName)
{
    setUp();

    ASSERT_EQ(readerAdapter->getName(), READER_NAME);

    tearDown();
}

TEST(AbstractReaderAdapterTest, getExtension_whenReaderIsRegistered_shouldReturnExtension)
{
    setUp();

    readerAdapter->doRegister();

    const std::type_info& classInfo = typeid(ReaderSpiMock);

    ASSERT_EQ(readerAdapter->getExtension(classInfo),
              std::dynamic_pointer_cast<KeypleReaderExtension>(readerSpi));

    tearDown();
}

TEST(AbstractReaderAdapterTest, getExtension_whenReaderIsNotRegistered_shouldISE)
{
    setUp();

    const std::type_info& classInfo = typeid(ReaderSpiMock);

    EXPECT_THROW(readerAdapter->getExtension(classInfo), IllegalStateException);

    tearDown();
}

TEST(AbstractReaderAdapterTest, transmitCardRequest_whenReaderIsNotRegistered_shouldISE)
{
    setUp();

    EXPECT_CALL(*readerAdapter.get(), processCardRequest(_,_))
        .Times(AtMost(1));

    EXPECT_THROW(readerAdapter->transmitCardRequest(cardRequestSpi, ChannelControl::KEEP_OPEN),
                 IllegalStateException);

    tearDown();
}

TEST(AbstractReaderAdapterTest, transmitCardRequest_shouldInvoke_processCardRequest)
{
    setUp();

    readerAdapter->doRegister();

    auto response = std::make_shared<CardResponseApiMock>();
    EXPECT_CALL(*response.get(), isLogicalChannelOpen()).WillRepeatedly(Return(true));
    const std::vector<std::shared_ptr<ApduResponseApi>> empty;
    EXPECT_CALL(*response.get(), getApduResponses()).WillRepeatedly(ReturnRef(empty));

    EXPECT_CALL(*readerAdapter.get(), processCardRequest(_,_)).Times(1).WillOnce(Return(response));

    readerAdapter->transmitCardRequest(cardRequestSpi, ChannelControl::KEEP_OPEN);

    response.reset();

    tearDown();
}
