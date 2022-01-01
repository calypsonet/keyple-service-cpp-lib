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
#include "CardIOException.h"
#include "ReaderIOException.h"
#include "ReaderSpi.h"

/* Calypsonet Terminal Card */
#include "CardBrokenCommunicationException.h"
#include "ChannelControl.h"
#include "ReaderBrokenCommunicationException.h"

/* Calypsonet Terminal Reader */
#include "ReaderCommunicationException.h"

/* Keyple Core Commons */
#include "KeypleReaderExtension.h"

/* Keyple Core Util */
#include "ByteArrayUtil.h"

/* Keyple Core Service */
#include "LocalReaderAdapter.h"
#include "LocalConfigurableReaderAdapter.h"
#include "MultiSelectionProcessing.h"

/* Mock */
#include "CardSelectionRequestSpiMock.h"
#include "CardSelectorSpiMock.h"
#include "ConfigurableReaderSpiMock.h"

using namespace testing;

using namespace calypsonet::terminal::reader;
using namespace keyple::core::common;
using namespace keyple::core::plugin;
using namespace keyple::core::plugin::spi::reader;
using namespace keyple::core::service;
using namespace keyple::core::util;

static const std::string PLUGIN_NAME = "plugin";
static const std::string READER_NAME = "reader";
static const std::string CARD_PROTOCOL = "cardProtocol";
static const std::string OTHER_CARD_PROTOCOL = "otherCardProtocol";
static const std::string POWER_ON_DATA = "12345678";

static std::shared_ptr<ConfigurableReaderSpiMock> readerSpi;
static std::shared_ptr<CardSelectorSpiMock> cardSelector;
static std::shared_ptr<CardSelectionRequestSpiMock> cardSelectionRequestSpi;

static const std::vector<int> successfulStatusWords({0x9000});
static const std::string powerOnData = "";
static const std::string protocol = "";
static const std::vector<uint8_t> selectResponseApdu1 = ByteArrayUtil::fromHex("123456789000");
static const std::vector<uint8_t> selectResponseApdu2 = ByteArrayUtil::fromHex("123456786283");
static const std::vector<int> statusWords({0x9000, 0x6283});

static bool mPhysicalChannelOpen;

static void setUp()
{
    mPhysicalChannelOpen = false;

    readerSpi = std::make_shared<ConfigurableReaderSpiMock>();
    EXPECT_CALL(*readerSpi.get(), getName()).WillRepeatedly(ReturnRef(READER_NAME));
    EXPECT_CALL(*readerSpi.get(), checkCardPresence()).WillRepeatedly(Return(true));
    EXPECT_CALL(*readerSpi.get(), getPowerOnData()).WillRepeatedly(Return(POWER_ON_DATA));
    EXPECT_CALL(*readerSpi.get(), closePhysicalChannel()).WillRepeatedly([]() { mPhysicalChannelOpen = false; });
    EXPECT_CALL(*readerSpi.get(), openPhysicalChannel()).WillRepeatedly([]() { mPhysicalChannelOpen = true; });
    EXPECT_CALL(*readerSpi.get(), isPhysicalChannelOpen()).WillRepeatedly(Return(mPhysicalChannelOpen));
    EXPECT_CALL(*readerSpi.get(), isContactless()).WillRepeatedly(Return(true));
    EXPECT_CALL(*readerSpi.get(), transmitApdu(_)).WillRepeatedly(Return(ByteArrayUtil::fromHex("6D00")));
    EXPECT_CALL(*readerSpi.get(), isProtocolSupported(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(*readerSpi.get(), isCurrentProtocol(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(*readerSpi.get(), activateProtocol(_)).WillRepeatedly(Return());
    
    cardSelector = std::make_shared<CardSelectorSpiMock>();
    EXPECT_CALL(*cardSelector.get(), getPowerOnDataRegex()).WillRepeatedly(ReturnRef(powerOnData));
    EXPECT_CALL(*cardSelector.get(), getAid()).WillRepeatedly(Return(std::vector<uint8_t>()));
    EXPECT_CALL(*cardSelector.get(), getCardProtocol()).WillRepeatedly(ReturnRef(protocol));
    EXPECT_CALL(*cardSelector.get(), getFileOccurrence()).WillRepeatedly(Return(CardSelectorSpi::FileOccurrence::FIRST));
    EXPECT_CALL(*cardSelector.get(), getFileControlInformation()).WillRepeatedly(Return(CardSelectorSpi::FileControlInformation::FCI));
    EXPECT_CALL(*cardSelector.get(), getSuccessfulSelectionStatusWords()).WillRepeatedly(ReturnRef(successfulStatusWords));

    cardSelectionRequestSpi = std::make_shared<CardSelectionRequestSpiMock>();
    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardRequest()).WillRepeatedly(Return(nullptr));
}

static void tearDown()
{
    cardSelector.reset();
    readerSpi.reset();
    cardSelectionRequestSpi.reset();
}

TEST(LocalReaderAdapterTest, getReaderSpi_shouldReturnReaderSpi)
{
    setUp();

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);

    ASSERT_EQ(localReaderAdapter.getReaderSpi(), readerSpi);

    tearDown();
}

TEST(LocalReaderAdapterTest, isCardPresent_whenReaderSpiFails_shouldKRCE)
{
    setUp();

    EXPECT_CALL(*readerSpi.get(), checkCardPresence())
        .Times(1)
        .WillOnce(Throw(ReaderIOException("Reader IO Exception")));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    EXPECT_THROW(localReaderAdapter.isCardPresent(), ReaderCommunicationException);

    tearDown();
}

TEST(LocalReaderAdapterTest,
     transmitCardSelectionRequests_withPermissiveCardSelector_shouldReturnMatchingResponseAndOpenChannel)
{
    setUp();

    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector())
        .WillRepeatedly(Return(cardSelector));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    const auto& cardSelectionResponses =
        localReaderAdapter.transmitCardSelectionRequests(
            std::vector<std::shared_ptr<CardSelectionRequestSpi>>({cardSelectionRequestSpi}),
            MultiSelectionProcessing::FIRST_MATCH,
            ChannelControl::CLOSE_AFTER);

    ASSERT_EQ(cardSelectionResponses.size(), 1);
    ASSERT_EQ(cardSelectionResponses[0]->getPowerOnData(), POWER_ON_DATA);
    ASSERT_TRUE(cardSelectionResponses[0]->hasMatched());
    ASSERT_TRUE(localReaderAdapter.isLogicalChannelOpen());

    tearDown();
}

TEST(LocalReaderAdapterTest,
     transmitCardSelectionRequests_withPermissiveCardSelectorAndProcessALL_shouldReturnMatchingResponseAndNotOpenChannel)
{
    setUp();

    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector())
        .WillRepeatedly(Return(cardSelector));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    const auto& cardSelectionResponses =
        localReaderAdapter.transmitCardSelectionRequests(
            std::vector<std::shared_ptr<CardSelectionRequestSpi>>({cardSelectionRequestSpi}),
            MultiSelectionProcessing::PROCESS_ALL,
            ChannelControl::CLOSE_AFTER);

    ASSERT_EQ(cardSelectionResponses.size(), 1);
    ASSERT_EQ(cardSelectionResponses[0]->getPowerOnData(), POWER_ON_DATA);
    ASSERT_TRUE(cardSelectionResponses[0]->hasMatched());
    ASSERT_FALSE(localReaderAdapter.isLogicalChannelOpen());

    tearDown();
}

TEST(LocalReaderAdapterTest,
     transmitCardSelectionRequests_withNonMatchingPowerOnDataFilteringCardSelector_shouldReturnNotMatchingResponseAndNotOpenChannel)
{
    setUp();

    const std::string powerOnData = "FAILINGREGEX";
    EXPECT_CALL(*cardSelector.get(), getPowerOnDataRegex()).WillRepeatedly(ReturnRef(powerOnData));
    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector())
        .WillRepeatedly(Return(cardSelector));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    const auto& cardSelectionResponses =
        localReaderAdapter.transmitCardSelectionRequests(
            std::vector<std::shared_ptr<CardSelectionRequestSpi>>({cardSelectionRequestSpi}),
            MultiSelectionProcessing::FIRST_MATCH,
            ChannelControl::CLOSE_AFTER);

    ASSERT_EQ(cardSelectionResponses.size(), 1);
    ASSERT_EQ(cardSelectionResponses[0]->getPowerOnData(), POWER_ON_DATA);
    ASSERT_FALSE(cardSelectionResponses[0]->hasMatched());
    ASSERT_FALSE(localReaderAdapter.isLogicalChannelOpen());

    tearDown();
}

TEST(LocalReaderAdapterTest,
     transmitCardSelectionRequests_withNonMatchingDFNameFilteringCardSelector_shouldReturnNotMatchingResponseAndNotOpenChannel)
{
    setUp();

    EXPECT_CALL(*cardSelector.get(), getAid())
        .WillRepeatedly(Return(ByteArrayUtil::fromHex("1122334455")));
    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector())
        .WillRepeatedly(Return(cardSelector));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    const auto& cardSelectionResponses =
        localReaderAdapter.transmitCardSelectionRequests(
            std::vector<std::shared_ptr<CardSelectionRequestSpi>>({cardSelectionRequestSpi}),
            MultiSelectionProcessing::FIRST_MATCH,
            ChannelControl::CLOSE_AFTER);

    ASSERT_EQ(cardSelectionResponses.size(), 1);
    ASSERT_EQ(cardSelectionResponses[0]->getPowerOnData(), POWER_ON_DATA);
    ASSERT_FALSE(cardSelectionResponses[0]->hasMatched());
    ASSERT_FALSE(localReaderAdapter.isLogicalChannelOpen());

    tearDown();
}

TEST(LocalReaderAdapterTest,
     transmitCardSelectionRequests_withMatchingDFNameFilteringCardSelector_shouldReturnMatchingResponseAndOpenChannel)
{
    setUp();

    EXPECT_CALL(*readerSpi.get(), transmitApdu(_)).WillRepeatedly(Return(selectResponseApdu1));
    EXPECT_CALL(*cardSelector.get(), getAid())
        .WillRepeatedly(Return(ByteArrayUtil::fromHex("1122334455")));
    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector())
        .WillRepeatedly(Return(cardSelector));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    const auto& cardSelectionResponses =
        localReaderAdapter.transmitCardSelectionRequests(
            std::vector<std::shared_ptr<CardSelectionRequestSpi>>({cardSelectionRequestSpi}),
            MultiSelectionProcessing::FIRST_MATCH,
            ChannelControl::CLOSE_AFTER);

    ASSERT_EQ(cardSelectionResponses.size(), 1);
    ASSERT_EQ(cardSelectionResponses[0]->getPowerOnData(), POWER_ON_DATA);
    ASSERT_EQ(cardSelectionResponses[0]->getSelectApplicationResponse()->getApdu(),
              selectResponseApdu1);
    ASSERT_TRUE(cardSelectionResponses[0]->hasMatched());
    ASSERT_TRUE(localReaderAdapter.isLogicalChannelOpen());

    tearDown();
}

TEST(LocalReaderAdapterTest,
     transmitCardSelectionRequests_withMatchingDFNameFilteringCardSelectorInvalidatedRejected_shouldReturnNotMatchingResponseAndNotOpenChannel)
{
    setUp();

    EXPECT_CALL(*readerSpi.get(), transmitApdu(_)).WillRepeatedly(Return(selectResponseApdu2));
    EXPECT_CALL(*cardSelector.get(), getAid())
        .WillRepeatedly(Return(ByteArrayUtil::fromHex("1122334455")));
    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector())
        .WillRepeatedly(Return(cardSelector));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    const auto& cardSelectionResponses =
        localReaderAdapter.transmitCardSelectionRequests(
            std::vector<std::shared_ptr<CardSelectionRequestSpi>>({cardSelectionRequestSpi}),
            MultiSelectionProcessing::FIRST_MATCH,
            ChannelControl::CLOSE_AFTER);

    ASSERT_EQ(cardSelectionResponses.size(), 1);
    ASSERT_EQ(cardSelectionResponses[0]->getPowerOnData(), POWER_ON_DATA);
    ASSERT_EQ(cardSelectionResponses[0]->getSelectApplicationResponse()->getApdu(),
              selectResponseApdu2);
    ASSERT_FALSE(cardSelectionResponses[0]->hasMatched());
    ASSERT_FALSE(localReaderAdapter.isLogicalChannelOpen());

    tearDown();
}

TEST(LocalReaderAdapterTest,
     transmitCardSelectionRequests_withMatchingDFNameFilteringCardSelectorInvalidatedAccepted_shouldReturnMatchingResponseAndOpenChannel)
{
    setUp();

    EXPECT_CALL(*readerSpi.get(), transmitApdu(_)).WillRepeatedly(Return(selectResponseApdu2));
    EXPECT_CALL(*cardSelector.get(), getAid())
        .WillRepeatedly(Return(ByteArrayUtil::fromHex("1122334455")));
    EXPECT_CALL(*cardSelector.get(), getSuccessfulSelectionStatusWords())
        .WillRepeatedly(ReturnRef(statusWords));
    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector())
        .WillRepeatedly(Return(cardSelector));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    const auto& cardSelectionResponses =
        localReaderAdapter.transmitCardSelectionRequests(
            std::vector<std::shared_ptr<CardSelectionRequestSpi>>({cardSelectionRequestSpi}),
            MultiSelectionProcessing::FIRST_MATCH,
            ChannelControl::CLOSE_AFTER);

    ASSERT_EQ(cardSelectionResponses.size(), 1);
    ASSERT_EQ(cardSelectionResponses[0]->getPowerOnData(), POWER_ON_DATA);
    ASSERT_EQ(cardSelectionResponses[0]->getSelectApplicationResponse()->getApdu(),
              selectResponseApdu2);
    ASSERT_TRUE(cardSelectionResponses[0]->hasMatched());
    ASSERT_TRUE(localReaderAdapter.isLogicalChannelOpen());

    tearDown();
}

TEST(LocalReaderAdapterTest,
     transmitCardSelectionRequests_withNonMatchingCardProtocolFilteringCardSelector_shouldReturnNotMatchingResponseAndNotOpenChannel)
{
    setUp();

    EXPECT_CALL(*cardSelector.get(), getCardProtocol())
        .WillRepeatedly(ReturnRef(OTHER_CARD_PROTOCOL));
    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector())
        .WillRepeatedly(Return(cardSelector));

    LocalConfigurableReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();
    localReaderAdapter.activateProtocol(CARD_PROTOCOL, CARD_PROTOCOL);

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    const auto& cardSelectionResponses =
        localReaderAdapter.transmitCardSelectionRequests(
            std::vector<std::shared_ptr<CardSelectionRequestSpi>>({cardSelectionRequestSpi}),
            MultiSelectionProcessing::FIRST_MATCH,
            ChannelControl::CLOSE_AFTER);

    ASSERT_EQ(cardSelectionResponses.size(), 1);
    ASSERT_FALSE(cardSelectionResponses[0]->hasMatched());
    ASSERT_FALSE(localReaderAdapter.isLogicalChannelOpen());

    tearDown();
}

TEST(LocalReaderAdapterTest,
     transmitCardSelectionRequests_whenOpenPhysicalThrowsReaderIOException_shouldRCE)
{
    setUp();

    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector())
        .WillRepeatedly(Return(cardSelector));

    EXPECT_CALL(*readerSpi.get(), openPhysicalChannel())
        .Times(1)
        .WillOnce(Throw(ReaderIOException("Reader IO Exception")));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    EXPECT_THROW(localReaderAdapter.transmitCardSelectionRequests(
                     std::vector<std::shared_ptr<CardSelectionRequestSpi>>(
                         {cardSelectionRequestSpi}),
                     MultiSelectionProcessing::FIRST_MATCH,
                     ChannelControl::CLOSE_AFTER),
                 ReaderBrokenCommunicationException);

    tearDown();
}

TEST(LocalReaderAdapterTest,
     transmitCardSelectionRequests_whenOpenPhysicalThrowsCArdIOException_shouldCCE)
{
    setUp();

    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector())
        .WillRepeatedly(Return(cardSelector));

    EXPECT_CALL(*readerSpi.get(), openPhysicalChannel())
        .Times(1)
        .WillOnce(Throw(CardIOException("Card IO Exception")));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    EXPECT_THROW(localReaderAdapter.transmitCardSelectionRequests(
                     std::vector<std::shared_ptr<CardSelectionRequestSpi>>(
                         {cardSelectionRequestSpi}),
                     MultiSelectionProcessing::FIRST_MATCH,
                     ChannelControl::CLOSE_AFTER),
                 CardBrokenCommunicationException);

    tearDown();
}

TEST(LocalReaderAdapterTest, isContactless_whenSpiIsContactless_shouldReturnTrue)
{
    setUp();

    EXPECT_CALL(*readerSpi.get(), isContactless()).Times(1).WillOnce(Return(true));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_TRUE(localReaderAdapter.isContactless());

    tearDown();
}

TEST(LocalReaderAdapterTest, isContactless_whenSpiIsNotContactless_shouldReturnFalse)
{
    setUp();

    EXPECT_CALL(*readerSpi.get(), isContactless()).Times(1).WillOnce(Return(false));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_FALSE(localReaderAdapter.isContactless());

    tearDown();
}
