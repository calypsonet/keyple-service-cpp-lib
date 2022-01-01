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

#include "ReaderAdapterTestUtils.h"

/* Keyple Core Util */
#include "ByteArrayUtil.h"

/* Mock */
#include "ApduResponseApiMock.h"
#include "CardResponseApiMock.h"

using namespace keyple::core::util;

using FileOccurrence = CardSelectorSpi::FileOccurrence;
using FileControlInformation = CardSelectorSpi::FileControlInformation;

const std::string ReaderAdapterTestUtils::READER_NAME = "reader";
const std::string ReaderAdapterTestUtils::CARD_PROTOCOL = "cardProtocol";
const std::string ReaderAdapterTestUtils::OTHER_CARD_PROTOCOL = "otherCardProtocol";
const std::string ReaderAdapterTestUtils::POWER_ON_DATA = "12345678";

const std::vector<int> success = {0x9000};

std::shared_ptr<ConfigurableReaderSpiMock> readerSpi;
std::shared_ptr<ConfigurableReaderSpiMock> readerSpiSpy;
std::shared_ptr<ObservableControllableReaderSpiMock> observableReaderSpi;
std::shared_ptr<CardSelectorSpiMock> cardSelector;
std::shared_ptr<CardSelectionResponseApiMock> matching;
std::shared_ptr<CardSelectionResponseApiMock> notMatching;

ReaderAdapterTestUtils::ReaderAdapterTestUtils()
: NOT_MATCHING_RESPONSES({getNotMatchingResponse()}),
  MULTI_NOT_MATCHING_RESPONSES({getNotMatchingResponse(), getNotMatchingResponse()}),
  MATCHING_RESPONSES({getMatchingResponse()}),
  MULTI_MATCHING_RESPONSES({getNotMatchingResponse(), getMatchingResponse(), getMatchingResponse()}) {}

void ReaderAdapterTestUtils::reset()
{
    readerSpi.reset();
    readerSpiSpy.reset();
    observableReaderSpi.reset();
    cardSelector.reset();
    matching.reset();
    notMatching.reset();
}

std::shared_ptr<ConfigurableReaderSpiMock> ReaderAdapterTestUtils::getReaderSpi() 
{
    readerSpi = std::make_shared<ConfigurableReaderSpiMock>();
    EXPECT_CALL(*readerSpi.get(), getName()).WillRepeatedly(ReturnRef(READER_NAME));
    EXPECT_CALL(*readerSpi.get(), checkCardPresence()).WillRepeatedly(Return(true));
    EXPECT_CALL(*readerSpi.get(), getPowerOnData()).WillRepeatedly(Return(POWER_ON_DATA));
    EXPECT_CALL(*readerSpi.get(), transmitApdu(_)).WillRepeatedly(Return(ByteArrayUtil::fromHex("6D00")));
    EXPECT_CALL(*readerSpi.get(), isProtocolSupported(CARD_PROTOCOL)).WillRepeatedly(Return(true));
    EXPECT_CALL(*readerSpi.get(), isCurrentProtocol(CARD_PROTOCOL)).WillRepeatedly(Return(true));

    return readerSpi;
}

std::shared_ptr<ConfigurableReaderSpiMock> ReaderAdapterTestUtils::getReaderSpiSpy()
{
    readerSpiSpy = std::make_shared<ConfigurableReaderSpiMock>();
    EXPECT_CALL(*readerSpiSpy.get(), getName()).WillRepeatedly(ReturnRef(READER_NAME));
    EXPECT_CALL(*readerSpiSpy.get(), checkCardPresence()).WillRepeatedly(Return(true));
    EXPECT_CALL(*readerSpiSpy.get(), getPowerOnData()).WillRepeatedly(Return(POWER_ON_DATA));
    EXPECT_CALL(*readerSpiSpy.get(), transmitApdu(_)).WillRepeatedly(Return(ByteArrayUtil::fromHex("6D00")));
    EXPECT_CALL(*readerSpiSpy.get(), isProtocolSupported(CARD_PROTOCOL)).WillRepeatedly(Return(true));
    EXPECT_CALL(*readerSpiSpy.get(), isCurrentProtocol(CARD_PROTOCOL)).WillRepeatedly(Return(true));

    return readerSpiSpy;
}

std::shared_ptr<ObservableControllableReaderSpiMock> ReaderAdapterTestUtils::getObservableReaderSpi()
{
    observableReaderSpi = std::make_shared<ObservableControllableReaderSpiMock>();
    EXPECT_CALL(*observableReaderSpi.get(), getName()).WillRepeatedly(ReturnRef(READER_NAME));
    EXPECT_CALL(*observableReaderSpi.get(), checkCardPresence()).WillRepeatedly(Return(true));
    EXPECT_CALL(*observableReaderSpi.get(), getPowerOnData()).WillRepeatedly(Return(POWER_ON_DATA));
    EXPECT_CALL(*observableReaderSpi.get(), transmitApdu(_)).WillRepeatedly(Return(ByteArrayUtil::fromHex("6D00")));
    EXPECT_CALL(*observableReaderSpi.get(), isProtocolSupported(CARD_PROTOCOL)).WillRepeatedly(Return(true));
    EXPECT_CALL(*observableReaderSpi.get(), isCurrentProtocol(CARD_PROTOCOL)).WillRepeatedly(Return(true));

    return observableReaderSpi;
}

std::shared_ptr<CardSelectorSpiMock> ReaderAdapterTestUtils::getCardSelectorSpi() 
{
    cardSelector = std::make_shared<CardSelectorSpiMock>();
    EXPECT_CALL(*cardSelector.get(), getFileOccurrence()).WillRepeatedly(Return(FileOccurrence::FIRST));
    EXPECT_CALL(*cardSelector.get(), getFileControlInformation()).WillRepeatedly(Return(FileControlInformation::FCI));
    EXPECT_CALL(*cardSelector.get(), getSuccessfulSelectionStatusWords()).WillRepeatedly(ReturnRef(success));

    return cardSelector;
}

std::shared_ptr<CardSelectionResponseApiMock> ReaderAdapterTestUtils::getMatchingResponse()
{
    matching = std::make_shared<CardSelectionResponseApiMock>();
    EXPECT_CALL(*matching.get(), hasMatched()).WillRepeatedly(Return(true));
    EXPECT_CALL(*matching.get(), getCardResponse()).WillRepeatedly(Return(std::make_shared<CardResponseApiMock>()));
    EXPECT_CALL(*matching.get(), getSelectApplicationResponse()).WillRepeatedly(Return(std::make_shared<ApduResponseApiMock>()));

    return matching;
}

std::shared_ptr<CardSelectionResponseApiMock> ReaderAdapterTestUtils::getNotMatchingResponse()
{
    notMatching = std::make_shared<CardSelectionResponseApiMock>();
    EXPECT_CALL(*notMatching.get(), hasMatched()).WillRepeatedly(Return(false));
    EXPECT_CALL(*notMatching.get(), getCardResponse()).WillRepeatedly(Return(std::make_shared<CardResponseApiMock>()));
    EXPECT_CALL(*notMatching.get(), getSelectApplicationResponse()).WillRepeatedly(Return(std::make_shared<ApduResponseApiMock>()));

    return notMatching;
}
