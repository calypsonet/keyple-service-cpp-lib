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
#include "UnexpectedStatusWordException.h"

/* Calypsonet Terminal Reader */
#include "ReaderCommunicationException.h"
#include "ReaderProtocolNotSupportedException.h"

/* Keyple Core Commons */
#include "KeypleReaderExtension.h"

/* Keyple Core Util */
#include "ByteArrayUtil.h"

/* Keyple Core Service */
#include "LocalReaderAdapter.h"
#include "LocalConfigurableReaderAdapter.h"
#include "MultiSelectionProcessing.h"

/* Mock */
#include "ApduRequestSpiMock.h"
#include "CardRequestSpiMock.h"
#include "CardSelectionRequestSpiMock.h"
#include "CardSelectorSpiMock.h"
#include "ConfigurableReaderSpiMock.h"

/* Utils */
#include "ReaderAdapterTestUtils.h"


using namespace testing;

using namespace calypsonet::terminal::card;
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
static std::shared_ptr<ApduRequestSpiMock> apduRequestSpi;
static std::shared_ptr<CardRequestSpiMock> cardRequestSpi;

static const std::vector<int> successfulStatusWords({0x9000});
static const std::string powerOnData = "";
static const std::string protocol = "";
static const std::vector<uint8_t> selectResponseApdu1 = ByteArrayUtil::fromHex("123456789000");
static const std::vector<uint8_t> selectResponseApdu2 = ByteArrayUtil::fromHex("123456786283");
static const std::vector<int> statusWords({0x9000, 0x6283});

/* ? */
const std::vector<int> dummy;
std::vector<std::shared_ptr<ApduRequestSpi>> apduRequests;

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

    apduRequestSpi = std::make_shared<ApduRequestSpiMock>();
    EXPECT_CALL(*apduRequestSpi.get(), getSuccessfulStatusWords()).WillRepeatedly(ReturnRef(dummy));
    apduRequests.push_back(apduRequestSpi);

    cardRequestSpi = std::make_shared<CardRequestSpiMock>();
    EXPECT_CALL(*cardRequestSpi.get(), getApduRequests()).WillRepeatedly(ReturnRef(apduRequests));
    EXPECT_CALL(*cardRequestSpi.get(), stopOnUnsuccessfulStatusWord()).WillRepeatedly(Return(false));
}

static void tearDown()
{
    cardSelector.reset();
    readerSpi.reset();
    cardSelectionRequestSpi.reset();
    apduRequestSpi.reset();
    cardRequestSpi.reset();
    apduRequests.clear();
}

TEST(LocalReaderAdapterTest, getReaderSpi_shouldReturnReaderSpi)
{
    setUp();

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);

    ASSERT_EQ(localReaderAdapter.getReaderSpi(), readerSpi);

    tearDown();
}

/*
 * Transmit card selections operations
 */

TEST(LocalReaderAdapterTest,
     transmitCardSelectionRequests_withPermissiveCardSelector_shouldReturnMatchingResponseAndOpenChannel)
{
    setUp();

    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector()).WillRepeatedly(Return(cardSelector));

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

    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector()).WillRepeatedly(Return(cardSelector));

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
    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector()).WillRepeatedly(Return(cardSelector));

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

    EXPECT_CALL(*cardSelector.get(), getAid()).WillRepeatedly(Return(ByteArrayUtil::fromHex("1122334455")));
    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector()).WillRepeatedly(Return(cardSelector));

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
    EXPECT_CALL(*cardSelector.get(), getAid()).WillRepeatedly(Return(ByteArrayUtil::fromHex("1122334455")));
    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector()).WillRepeatedly(Return(cardSelector));

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
    ASSERT_EQ(cardSelectionResponses[0]->getSelectApplicationResponse()->getApdu(), selectResponseApdu1);
    ASSERT_TRUE(cardSelectionResponses[0]->hasMatched());
    ASSERT_TRUE(localReaderAdapter.isLogicalChannelOpen());

    tearDown();
}

TEST(LocalReaderAdapterTest,
     transmitCardSelectionRequests_withMatchingDFNameFilteringCardSelectorInvalidatedRejected_shouldReturnNotMatchingResponseAndNotOpenChannel)
{
    setUp();

    EXPECT_CALL(*readerSpi.get(), transmitApdu(_)).WillRepeatedly(Return(selectResponseApdu2));
    EXPECT_CALL(*cardSelector.get(), getAid()).WillRepeatedly(Return(ByteArrayUtil::fromHex("1122334455")));
    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector()).WillRepeatedly(Return(cardSelector));

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
    ASSERT_EQ(cardSelectionResponses[0]->getSelectApplicationResponse()->getApdu(), selectResponseApdu2);
    ASSERT_FALSE(cardSelectionResponses[0]->hasMatched());
    ASSERT_FALSE(localReaderAdapter.isLogicalChannelOpen());

    tearDown();
}

TEST(LocalReaderAdapterTest,
     transmitCardSelectionRequests_withMatchingDFNameFilteringCardSelectorInvalidatedAccepted_shouldReturnMatchingResponseAndOpenChannel)
{
    setUp();

    EXPECT_CALL(*readerSpi.get(), transmitApdu(_)).WillRepeatedly(Return(selectResponseApdu2));
    EXPECT_CALL(*cardSelector.get(), getAid()).WillRepeatedly(Return(ByteArrayUtil::fromHex("1122334455")));
    EXPECT_CALL(*cardSelector.get(), getSuccessfulSelectionStatusWords()).WillRepeatedly(ReturnRef(statusWords));
    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector()).WillRepeatedly(Return(cardSelector));

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
    ASSERT_EQ(cardSelectionResponses[0]->getSelectApplicationResponse()->getApdu(), selectResponseApdu2);
    ASSERT_TRUE(cardSelectionResponses[0]->hasMatched());
    ASSERT_TRUE(localReaderAdapter.isLogicalChannelOpen());

    tearDown();
}

TEST(LocalReaderAdapterTest,
     transmitCardSelectionRequests_withNonMatchingCardProtocolFilteringCardSelector_shouldReturnNotMatchingResponseAndNotOpenChannel)
{
    setUp();

    EXPECT_CALL(*cardSelector.get(), getCardProtocol()).WillRepeatedly(ReturnRef(OTHER_CARD_PROTOCOL));
    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector()).WillRepeatedly(Return(cardSelector));

    LocalConfigurableReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();
    localReaderAdapter.activateProtocol(CARD_PROTOCOL, CARD_PROTOCOL);

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    const auto& cardSelectionResponses =
        localReaderAdapter.transmitCardSelectionRequests(
            std::vector<std::shared_ptr<CardSelectionRequestSpi>>(
                {cardSelectionRequestSpi}),
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

    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector()).WillRepeatedly(Return(cardSelector));
    EXPECT_CALL(*readerSpi.get(), openPhysicalChannel()).Times(1).WillOnce(Throw(ReaderIOException("Reader IO Exception")));

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

    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector()).WillRepeatedly(Return(cardSelector));
    EXPECT_CALL(*readerSpi.get(), openPhysicalChannel()).Times(1).WillOnce(Throw(CardIOException("Card IO Exception")));

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

TEST(LocalReaderAdapterTest,
     transmitCardSelectionRequests_whenTransmitApduThrowsCardIOException_shouldCCE)
{
    setUp();

    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector()).WillRepeatedly(Return(cardSelector));
    EXPECT_CALL(*cardSelector.get(), getAid()).WillRepeatedly(Return(ByteArrayUtil::fromHex("12341234")));
    EXPECT_CALL(*readerSpi.get(), openPhysicalChannel()).Times(1).WillOnce(Throw(CardIOException("Card IO Exception")));

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

TEST(LocalReaderAdapterTest,
     transmitCardSelectionRequests_whenTransmitApduThrowsReaderIOException_shouldRCE)
{
    setUp();

    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector()).WillRepeatedly(Return(cardSelector));
    EXPECT_CALL(*cardSelector.get(), getAid()).WillRepeatedly(Return(ByteArrayUtil::fromHex("12341234")));
    EXPECT_CALL(*readerSpi.get(), transmitApdu(_)).Times(1).WillOnce(Throw(ReaderIOException("Reader IO Exception")));

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
     transmitCardSelectionRequests_whenFirstMatchAndSecondSelectionFails_shouldNotMatch)
{
    setUp();

    EXPECT_CALL(*cardSelector.get(), getAid()).WillRepeatedly(Return(ByteArrayUtil::fromHex("1122334455")));
    EXPECT_CALL(*cardSelectionRequestSpi.get(), getCardSelector()).WillRepeatedly(Return(cardSelector));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    /* First successful selection */
    EXPECT_CALL(*readerSpi.get(), transmitApdu(_)).WillRepeatedly(Return(ByteArrayUtil::fromHex("AABBCCDDEE9000")));

    std::vector<std::shared_ptr<CardSelectionResponseApi>> cardSelectionResponses =
        localReaderAdapter.transmitCardSelectionRequests(
            std::vector<std::shared_ptr<CardSelectionRequestSpi>>({cardSelectionRequestSpi}),
            MultiSelectionProcessing::FIRST_MATCH,
            ChannelControl::KEEP_OPEN);

    ASSERT_EQ(cardSelectionResponses.size(), 1);
    ASSERT_TRUE(cardSelectionResponses[0]->hasMatched());
    ASSERT_TRUE(localReaderAdapter.isLogicalChannelOpen());

    /* Second not matching selection */
    EXPECT_CALL(*readerSpi.get(), transmitApdu(_)).WillRepeatedly(Return(ByteArrayUtil::fromHex("6B00")));

    cardSelectionResponses =
        localReaderAdapter.transmitCardSelectionRequests(
            std::vector<std::shared_ptr<CardSelectionRequestSpi>>({cardSelectionRequestSpi}),
            MultiSelectionProcessing::FIRST_MATCH,
            ChannelControl::KEEP_OPEN);

    ASSERT_EQ(cardSelectionResponses.size(), 1);
    ASSERT_FALSE(cardSelectionResponses[0]->hasMatched());
    ASSERT_FALSE(localReaderAdapter.isLogicalChannelOpen());

    tearDown();
}

// todo: selectByAid with AutonomousSelectionReaderSpi

/*
 * Transmit card request
 */

TEST(LocalReaderAdapterTest, transmitCardRequest_shouldReturnResponse)
{
    setUp();

    const std::vector<uint8_t> responseApdu = ByteArrayUtil::fromHex("123456786283");
    const std::vector<uint8_t> requestApdu = ByteArrayUtil::fromHex("0000");

    EXPECT_CALL(*readerSpi.get(), transmitApdu(_)).WillRepeatedly(Return(responseApdu));
    EXPECT_CALL(*apduRequestSpi.get(), getApdu()).WillRepeatedly(ReturnRef(requestApdu));

    LocalConfigurableReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();
    localReaderAdapter.activateProtocol(CARD_PROTOCOL, CARD_PROTOCOL);

    ASSERT_TRUE(localReaderAdapter.isCardPresent());

    auto cardResponse = localReaderAdapter.transmitCardRequest(cardRequestSpi,
                                                               ChannelControl::CLOSE_AFTER);

    ASSERT_EQ(cardResponse->getApduResponses()[0]->getApdu(), responseApdu);
    ASSERT_FALSE(cardResponse->isLogicalChannelOpen());

    tearDown();
}

TEST(LocalReaderAdapterTest, transmitCardRequest_isCase4)
{
    setUp();

    const std::vector<uint8_t> requestApdu = ByteArrayUtil::fromHex("11223344041234567803");
    const std::vector<uint8_t> responseApdu = ByteArrayUtil::fromHex("9000");
    const std::vector<uint8_t> responseCase4Apdu = ByteArrayUtil::fromHex("0000");
    const std::vector<uint8_t> APDU_GET_RESPONSE = {0x00, 0xC0, 0x00, 0x00, 0x00};

    EXPECT_CALL(*apduRequestSpi.get(), getApdu()).WillRepeatedly(ReturnRef(requestApdu));
    EXPECT_CALL(*readerSpi.get(), transmitApdu(requestApdu)).WillRepeatedly(Return(responseApdu));
    EXPECT_CALL(*readerSpi.get(), transmitApdu(APDU_GET_RESPONSE)).WillRepeatedly(Return(responseCase4Apdu));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();
    auto response = localReaderAdapter.transmitCardRequest(cardRequestSpi,
                                                           ChannelControl::CLOSE_AFTER);

    ASSERT_EQ(response->getApduResponses()[0]->getApdu(), responseCase4Apdu);

    tearDown();
}

TEST(LocalReaderAdapterTest, transmitCardRequest_withUnsuccessfulStatusWord_shouldThrow_USW)
{
    setUp();

    const std::vector<uint8_t> responseApdu = ByteArrayUtil::fromHex("123456789000");
    const std::vector<uint8_t> requestApdu = ByteArrayUtil::fromHex("0000");
    const std::vector<int> resp = {0x9001};

    EXPECT_CALL(*readerSpi.get(), transmitApdu(_)).WillRepeatedly(Return(responseApdu));
    EXPECT_CALL(*apduRequestSpi.get(), getApdu()).WillRepeatedly(ReturnRef(requestApdu));
    EXPECT_CALL(*apduRequestSpi.get(), getSuccessfulStatusWords()).WillRepeatedly(ReturnRef(resp));
    EXPECT_CALL(*cardRequestSpi.get(), stopOnUnsuccessfulStatusWord()).WillRepeatedly(Return(true));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    EXPECT_THROW(localReaderAdapter.transmitCardRequest(cardRequestSpi, ChannelControl::CLOSE_AFTER),
                 UnexpectedStatusWordException);

    tearDown();
}

TEST(LocalReaderAdapterTest, transmitCardRequest_withCardExceptionOnTransmit_shouldThrow_CBCE)
{
    setUp();

    const std::vector<uint8_t> requestApdu = ByteArrayUtil::fromHex("0000");

    EXPECT_CALL(*readerSpi.get(), transmitApdu(_)).WillRepeatedly(Throw(CardIOException("")));
    EXPECT_CALL(*apduRequestSpi.get(), getApdu()).WillRepeatedly(ReturnRef(requestApdu));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    EXPECT_THROW(localReaderAdapter.transmitCardRequest(cardRequestSpi, ChannelControl::CLOSE_AFTER),
                 CardBrokenCommunicationException);

    tearDown();
}

TEST(LocalReaderAdapterTest, transmitCardRequest_withCardExceptionOnTransmit_shouldThrow_RBCE)
{
    setUp();

    const std::vector<uint8_t> requestApdu = ByteArrayUtil::fromHex("0000");

    EXPECT_CALL(*readerSpi.get(), transmitApdu(_)).WillRepeatedly(Throw(ReaderIOException("")));
    EXPECT_CALL(*apduRequestSpi.get(), getApdu()).WillRepeatedly(ReturnRef(requestApdu));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    EXPECT_THROW(localReaderAdapter.transmitCardRequest(cardRequestSpi, ChannelControl::CLOSE_AFTER),
                 ReaderBrokenCommunicationException);

    tearDown();
}

/*
 * active protocol operations
 */

TEST(LocalReaderAdapterTest, deActivateProtocol_shouldInvoke_deActivateProcotol_OnReaderSpi)
{
    setUp();

    ReaderAdapterTestUtils utils;
    std::shared_ptr<ConfigurableReaderSpiMock> spy = utils.getReaderSpiSpy();
    EXPECT_CALL(*spy.get(), deactivateProtocol(CARD_PROTOCOL)).Times(1);
    EXPECT_CALL(*spy.get(), activateProtocol(_)).WillRepeatedly(Return());

    LocalConfigurableReaderAdapter localReaderAdapter(spy, PLUGIN_NAME);

    localReaderAdapter.doRegister();
    localReaderAdapter.activateProtocol(CARD_PROTOCOL, CARD_PROTOCOL);
    localReaderAdapter.deactivateProtocol(CARD_PROTOCOL);

    tearDown();
}

TEST(LocalReaderAdapterTest, activateProtocol_whileNotSupported_should_RPNS)
{
    ReaderAdapterTestUtils utils;
    std::shared_ptr<ConfigurableReaderSpiMock> spy = utils.getReaderSpiSpy();
    EXPECT_CALL(*spy.get(), isProtocolSupported(_)).WillRepeatedly(Return(false));
    EXPECT_CALL(*spy.get(), activateProtocol(_)).WillRepeatedly(Return());

    LocalConfigurableReaderAdapter localReaderAdapter(spy, PLUGIN_NAME);
    localReaderAdapter.doRegister();


    EXPECT_THROW(localReaderAdapter.activateProtocol(CARD_PROTOCOL, CARD_PROTOCOL),
                 ReaderProtocolNotSupportedException);
}

TEST(LocalReaderAdapterTest, deActivateProtocol_whileNotSupported_should_RPNS)
{
    ReaderAdapterTestUtils utils;
    std::shared_ptr<ConfigurableReaderSpiMock> spy = utils.getReaderSpiSpy();
    EXPECT_CALL(*spy.get(), isProtocolSupported(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(*spy.get(), activateProtocol(_)).WillRepeatedly(Return());
    EXPECT_CALL(*spy.get(), deactivateProtocol(_)).WillRepeatedly(Return());

    LocalConfigurableReaderAdapter localReaderAdapter(spy, PLUGIN_NAME);
    localReaderAdapter.doRegister();
    localReaderAdapter.activateProtocol(CARD_PROTOCOL, CARD_PROTOCOL);

    EXPECT_CALL(*spy.get(), isProtocolSupported(_)).WillRepeatedly(Return(false));

    EXPECT_THROW(localReaderAdapter.deactivateProtocol(CARD_PROTOCOL),
                 ReaderProtocolNotSupportedException);
}

/*
 * Misc operations
 */

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

TEST(LocalReaderAdapterTest, closeLogicalAndPhysicalChannelsSilently_withException_does_not_propagate)
{
    setUp();

    EXPECT_CALL(*readerSpi.get(), closePhysicalChannel()).WillRepeatedly(Throw(ReaderIOException("")));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.closeLogicalAndPhysicalChannelsSilently();

    /* No exception is propagated */

    tearDown();
}

TEST(LocalReaderAdapterTest, releaseChannel_withException_throwRBCE)
{
    setUp();

    EXPECT_CALL(*readerSpi.get(), closePhysicalChannel()).WillRepeatedly(Throw(ReaderIOException("")));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    /* Exception is thrown */
    EXPECT_THROW(localReaderAdapter.releaseChannel(), ReaderBrokenCommunicationException);

    tearDown();
}

TEST(LocalReaderAdapterTest, isCardPresent_whenReaderSpiFails_shouldKRCE)
{
    setUp();

    EXPECT_CALL(*readerSpi.get(), checkCardPresence()).WillRepeatedly(Throw(ReaderIOException("Reader IO Exception")));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    EXPECT_THROW(localReaderAdapter.isCardPresent(), ReaderCommunicationException);

    tearDown();
}
