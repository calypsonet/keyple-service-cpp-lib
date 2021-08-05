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
#include "CardSelectionRequestSpi.h"
#include "CardSelectorSpi.h"
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
#include "MultiSelectionProcessing.h"

using namespace testing;

using namespace calypsonet::terminal::card::spi;
using namespace calypsonet::terminal::reader;
using namespace keyple::core::commons;
using namespace keyple::core::plugin;
using namespace keyple::core::plugin::spi::reader;
using namespace keyple::core::service;
using namespace keyple::core::util;

static const std::string PLUGIN_NAME = "plugin";
static const std::string READER_NAME = "reader";
static const std::string CARD_PROTOCOL = "cardProtocol";
static const std::string OTHER_CARD_PROTOCOL = "otherCardProtocol";
static const std::string POWER_ON_DATA = "12345678";

class LRAT_ReaderSpiMock final : public KeypleReaderExtension, public ReaderSpi {
public:
    MOCK_METHOD((const std::string&),
                getName,
                (),
                (const, override, final));

    MOCK_METHOD(bool,
                isProtocolSupported,
                (const std::string& readerProtocol),
                (const, override, final));

    virtual void activateProtocol(const std::string& readerProtocol) override final
    {
        (void)readerProtocol;
    }

    virtual void deactivateProtocol(const std::string& readerProtocol) override final
    {
        (void)readerProtocol;
    }

    MOCK_METHOD(bool,
                isCurrentProtocol,
                (const std::string& readerProtocol),
                (const, override, final));

    MOCK_METHOD(void,
                openPhysicalChannel,
                (),
                (override, final));

    virtual void closePhysicalChannel() override final {}

    virtual bool isPhysicalChannelOpen() const override final
    {
        return false;
    }

    MOCK_METHOD(bool,
                checkCardPresence,
                (),
                (override, final));

    MOCK_METHOD((const std::string),
                getPowerOnData,
                (),
                (const, override, final));

    MOCK_METHOD((const std::vector<uint8_t>),
                transmitApdu,
                (const std::vector<uint8_t>& apduIn),
                (override, final));

    MOCK_METHOD(bool,
                isContactless,
                (),
                (override, final));

    virtual void onUnregister() override final {}
};

class LRAT_CardSelectorMock final : public CardSelectorSpi {
public:
    MOCK_METHOD((const std::string&),
                getCardProtocol,
                (),
                (const, override, final));

    MOCK_METHOD((const std::string&),
                getPowerOnDataRegex,
                (),
                (const, override, final));

    MOCK_METHOD((const std::vector<uint8_t>),
                getAid,
                (),
                (const, override, final));

    MOCK_METHOD(FileOccurrence,
                getFileOccurrence,
                (),
                (const, override, final));

    MOCK_METHOD(FileControlInformation,
                getFileControlInformation,
                (),
                (const, override, final));

    MOCK_METHOD((const std::vector<int>&),
                getSuccessfulSelectionStatusWords,
                (),
                (const, override, final));
};

class LRAT_CardSelectionRequestSpiMock final : public CardSelectionRequestSpi {
public:
    MOCK_METHOD((const std::shared_ptr<CardSelectorSpi>),
                getCardSelector,
                (),
                (const, override, final));

    virtual const std::shared_ptr<CardRequestSpi> getCardRequest() const override final
    {
        return nullptr;
    }
};

static std::shared_ptr<LRAT_ReaderSpiMock> readerSpi;
static std::shared_ptr<LRAT_CardSelectorMock> cardSelector;
static std::shared_ptr<CardSelectionRequestSpi> cardSelectionRequestSpi;

static const std::vector<int> successfulStatusWords({0x9000});
static const std::string powerOnData = "";
static const std::string protocol = "";
static const std::vector<uint8_t> selectResponseApdu1 = ByteArrayUtil::fromHex("123456789000");
static const std::vector<uint8_t> selectResponseApdu2 = ByteArrayUtil::fromHex("123456786283");
static const std::vector<int> statusWords({0x9000, 0x6283});

static void setUp()
{
    readerSpi = std::make_shared<LRAT_ReaderSpiMock>();
    EXPECT_CALL(*readerSpi.get(), getName())
        .WillRepeatedly(ReturnRef(READER_NAME));
    EXPECT_CALL(*readerSpi.get(), checkCardPresence())
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*readerSpi.get(), getPowerOnData())
        .WillRepeatedly(Return(POWER_ON_DATA));
    EXPECT_CALL(*readerSpi.get(), transmitApdu(_))
        .WillRepeatedly(Return(ByteArrayUtil::fromHex("6D00")));
    EXPECT_CALL(*readerSpi.get(), isProtocolSupported(CARD_PROTOCOL))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*readerSpi.get(), isCurrentProtocol(CARD_PROTOCOL))
        .WillRepeatedly(Return(true));
    EXPECT_CALL(*readerSpi.get(), openPhysicalChannel())
        .WillRepeatedly(Return());
    EXPECT_CALL(*readerSpi.get(), isContactless())
        .WillRepeatedly(Return(true));

    cardSelector = std::make_shared<LRAT_CardSelectorMock>();
    EXPECT_CALL(*cardSelector.get(), getFileOccurrence())
        .WillRepeatedly(Return(CardSelectorSpi::FileOccurrence::FIRST));
    EXPECT_CALL(*cardSelector.get(), getFileControlInformation())
        .WillRepeatedly(Return(CardSelectorSpi::FileControlInformation::FCI));
    EXPECT_CALL(*cardSelector.get(), getSuccessfulSelectionStatusWords())
        .WillRepeatedly(ReturnRef(successfulStatusWords));
    EXPECT_CALL(*cardSelector.get(), getPowerOnDataRegex())
        .WillRepeatedly(ReturnRef(powerOnData));
    EXPECT_CALL(*cardSelector.get(), getAid())
        .WillRepeatedly(Return(std::vector<uint8_t>()));
    EXPECT_CALL(*cardSelector.get(), getCardProtocol())
        .WillRepeatedly(ReturnRef(protocol));

    cardSelectionRequestSpi = std::make_shared<LRAT_CardSelectionRequestSpiMock>();
}

static void tearDown()
{
    /* Force count down to force pointer deletion and expectations check */
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

    const auto request =
        std::dynamic_pointer_cast<LRAT_CardSelectionRequestSpiMock>(cardSelectionRequestSpi);
    EXPECT_CALL(*request.get(), getCardSelector())
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

    const auto request =
        std::dynamic_pointer_cast<LRAT_CardSelectionRequestSpiMock>(cardSelectionRequestSpi);
    EXPECT_CALL(*request.get(), getCardSelector())
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
    EXPECT_CALL(*cardSelector.get(), getPowerOnDataRegex())
        .WillRepeatedly(ReturnRef(powerOnData));
    const auto request =
        std::dynamic_pointer_cast<LRAT_CardSelectionRequestSpiMock>(cardSelectionRequestSpi);
    EXPECT_CALL(*request.get(), getCardSelector())
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
    const auto request =
        std::dynamic_pointer_cast<LRAT_CardSelectionRequestSpiMock>(cardSelectionRequestSpi);
    EXPECT_CALL(*request.get(), getCardSelector())
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

    EXPECT_CALL(*readerSpi.get(), transmitApdu(_))
        .WillRepeatedly(Return(selectResponseApdu1));
    EXPECT_CALL(*cardSelector.get(), getAid())
        .WillRepeatedly(Return(ByteArrayUtil::fromHex("1122334455")));
    const auto request =
        std::dynamic_pointer_cast<LRAT_CardSelectionRequestSpiMock>(cardSelectionRequestSpi);
    EXPECT_CALL(*request.get(), getCardSelector())
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

    EXPECT_CALL(*readerSpi.get(), transmitApdu(_))
        .WillRepeatedly(Return(selectResponseApdu2));
    EXPECT_CALL(*cardSelector.get(), getAid())
        .WillRepeatedly(Return(ByteArrayUtil::fromHex("1122334455")));
    const auto request =
        std::dynamic_pointer_cast<LRAT_CardSelectionRequestSpiMock>(cardSelectionRequestSpi);
    EXPECT_CALL(*request.get(), getCardSelector())
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

    EXPECT_CALL(*readerSpi.get(), transmitApdu(_))
        .WillRepeatedly(Return(selectResponseApdu2));
    EXPECT_CALL(*cardSelector.get(), getAid())
        .WillRepeatedly(Return(ByteArrayUtil::fromHex("1122334455")));
    EXPECT_CALL(*cardSelector.get(), getSuccessfulSelectionStatusWords())
        .WillRepeatedly(ReturnRef(statusWords));
    const auto request =
        std::dynamic_pointer_cast<LRAT_CardSelectionRequestSpiMock>(cardSelectionRequestSpi);
    EXPECT_CALL(*request.get(), getCardSelector())
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
    const auto request =
        std::dynamic_pointer_cast<LRAT_CardSelectionRequestSpiMock>(cardSelectionRequestSpi);
    EXPECT_CALL(*request.get(), getCardSelector())
        .WillRepeatedly(Return(cardSelector));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
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

    const auto request =
        std::dynamic_pointer_cast<LRAT_CardSelectionRequestSpiMock>(cardSelectionRequestSpi);
    EXPECT_CALL(*request.get(), getCardSelector())
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

    const auto request =
        std::dynamic_pointer_cast<LRAT_CardSelectionRequestSpiMock>(cardSelectionRequestSpi);
    EXPECT_CALL(*request.get(), getCardSelector())
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

    EXPECT_CALL(*readerSpi.get(), isContactless())
        .Times(1)
        .WillOnce(Return(true));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_TRUE(localReaderAdapter.isContactless());

    tearDown();
}

TEST(LocalReaderAdapterTest, isContactless_whenSpiIsNotContactless_shouldReturnFalse)
{
    setUp();

    EXPECT_CALL(*readerSpi.get(), isContactless())
        .Times(1)
        .WillOnce(Return(false));

    LocalReaderAdapter localReaderAdapter(readerSpi, PLUGIN_NAME);
    localReaderAdapter.doRegister();

    ASSERT_FALSE(localReaderAdapter.isContactless());

    tearDown();
}
