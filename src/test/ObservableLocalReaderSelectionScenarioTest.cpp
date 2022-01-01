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
#include "LoggerFactory.h"

/* Keyple Core Service */
#include "ObservableLocalReaderAdapter.h"
#include "ScheduledCardSelectionsResponseAdapter.h"

/* Mock */
#include "CardReaderObservationExceptionHandlerSpiMock.h"
#include "CardResponseApiMock.h"
#include "CardSelectionRequestSpiMock.h"
#include "CardSelectionResponseApiMock.h"
#include "ObservableLocalReaderAdapterMock.h"
#include "ObservableReaderAutonomousSpiMock.h"
#include "ReaderObserverSpiMock.h"

/* Util */
#include "ObservableLocalReaderSuite.h"
#include "ReaderAdapterTestUtils.h"

/* Calypsonet Terminal Card */
#include "CardBrokenCommunicationException.h"
#include "ReaderBrokenCommunicationException.h"


using namespace testing;

using namespace calypsonet::terminal::card;
using namespace keyple::core::service;
using namespace keyple::core::util::cpp;

static const std::string PLUGIN_NAME = "plugin";

class ObservableLocalReaderSelectionScenarioTest {};
static const std::shared_ptr<Logger> logger =
    LoggerFactory::getLogger(typeid(ObservableLocalReaderSelectionScenarioTest));

static std::shared_ptr<ObservableLocalReaderAdapterMock> readerSpy;
static std::shared_ptr<ObservableReaderAutonomousSpiMock> readerSpi;
static std::shared_ptr<ReaderObserverSpiMock> observer;
static std::shared_ptr<CardReaderObservationExceptionHandlerSpiMock> handler;
static std::shared_ptr<ObservableLocalReaderSuite> testSuite;

static std::shared_ptr<CardSelectionRequestSpi> cardSelectionRequestSpi;
static std::shared_ptr<CardSelectionResponseApi> cardSelectionResponseApi;
static std::shared_ptr<CardResponseApi> cardResponseApi;
static std::shared_ptr<ReaderEvent> event;

/*
 *  With ObservableReaderAutonomousSpi
 */
static void setUp()
{
    readerSpi = std::make_shared<ObservableReaderAutonomousSpiMock>(READER_NAME);
    handler = std::make_shared<CardReaderObservationExceptionHandlerSpiMock>();
    readerSpy = std::make_shared<ObservableLocalReaderAdapterMock>(readerSpi, PLUGIN_NAME);
    observer = std::make_shared<ReaderObserverSpiMock>(nullptr);
    testSuite = std::make_shared<ObservableLocalReaderSuite>(readerSpy, readerSpi, observer, handler, logger);
    cardSelectionRequestSpi = std::make_shared<CardSelectionRequestSpiMock>();
    cardSelectionResponseApi = std::make_shared<CardSelectionResponseApiMock>();
    cardResponseApi = std::make_shared<CardResponseApiMock>();

    /* Test with event notification executor service */
    readerSpy->doRegister();
    readerSpy->setReaderObservationExceptionHandler(handler);
}

static void tearDown()
{
    cardResponseApi.reset();
    cardSelectionResponseApi.reset();
    cardSelectionRequestSpi.reset();
    testSuite.reset();
    observer.reset();
    readerSpy.reset();
    handler.reset();
    readerSpi.reset();
    event.reset();
}

/**
 * Configure card selection scenario and mock selection responses
 *
 * @param cardSelectionResponse
 * @param notificationMode
 * @throws Exception
 */
static void mockReaderWithSelectionResponses(
    const std::vector<std::shared_ptr<CardSelectionResponseApi>>& cardSelectionResponse,
    const ObservableCardReader::NotificationMode notificationMode)
{
    EXPECT_CALL(*readerSpy.get(), transmitCardSelectionRequests(_,_,_))
        .WillRepeatedly(Return(cardSelectionResponse));

    const std::vector<std::shared_ptr<CardSelectionRequestSpi>> requests = {
        cardSelectionRequestSpi};
    
    readerSpy->scheduleCardSelectionScenario(
        std::make_shared<CardSelectionScenarioAdapter>(requests,
                                                       MultiSelectionProcessing::PROCESS_ALL,
                                                       ChannelControl::KEEP_OPEN),
        notificationMode,
        ObservableCardReader::DetectionMode::SINGLESHOT);
}

/**
 * assert event is of type
 *
 * @param type
 */
static void assertEventIs(const CardReaderEvent::Type type)
{
    ASSERT_NE(event, nullptr);
    ASSERT_EQ(event->getType(), type);
    ASSERT_EQ(event->getReaderName(), READER_NAME);
    ASSERT_EQ(event->getPluginName(), PLUGIN_NAME);
}

TEST(ObservableLocalReaderSelectionScenarioTest, process_card_with_no_scenario_return_cardInserted)
{
    setUp();

    /* Scenario : no scenario scheduled */

    event = readerSpy->processCardInserted();

    /* Verify ScheduledCardSelectionsResponseAdapter in event */
    assertEventIs(CardReaderEvent::Type::CARD_INSERTED);

    tearDown();
}

TEST(ObservableLocalReaderSelectionScenarioTest, 
     process_card_with_matching_selections_return_cardMatched)
{
    /* Scenario : selection match */

    setUp();

    ReaderAdapterTestUtils utils;
    
    mockReaderWithSelectionResponses(utils.MATCHING_RESPONSES, 
                                     ObservableCardReader::MATCHED_ONLY);

    event = readerSpy->processCardInserted();

    /* Verify ScheduledCardSelectionsResponseAdapter in event */
    assertEventIs(CardReaderEvent::Type::CARD_MATCHED);
    ASSERT_NE(event->getScheduledCardSelectionsResponse(), nullptr);

    utils.reset();
    
    tearDown();
}

TEST(ObservableLocalReaderSelectionScenarioTest,
     process_card_with_no_matching_selections_with_Always_return_CardInserted)
{
    /* Scenario : selection no match */

    setUp();

    ReaderAdapterTestUtils utils;

    mockReaderWithSelectionResponses(utils.NOT_MATCHING_RESPONSES, ObservableCardReader::ALWAYS);

    event = readerSpy->processCardInserted();

    assertEventIs(CardReaderEvent::Type::CARD_INSERTED);

    utils.reset();
    
    tearDown();
}

TEST(ObservableLocalReaderSelectionScenarioTest,
     process_card_with_no_matching_selections_with_MatchedOnly_return_null)
{
    /* Scenario : selection no match */

    setUp();

    ReaderAdapterTestUtils utils;

    mockReaderWithSelectionResponses(utils.NOT_MATCHING_RESPONSES, 
                                     ObservableCardReader::MATCHED_ONLY);

    event = readerSpy->processCardInserted();

    ASSERT_EQ(event, nullptr);

    utils.reset();
    
    tearDown();
}

TEST(ObservableLocalReaderSelectionScenarioTest,
     process_card_with_multi_matching_selections_return_CardMatched)
{
    /* Scenario : first no match, second match, third match */

    setUp();

    ReaderAdapterTestUtils utils;

    mockReaderWithSelectionResponses(utils.MULTI_MATCHING_RESPONSES, 
                                     ObservableCardReader::MATCHED_ONLY);

    event = readerSpy->processCardInserted();

    assertEventIs(CardReaderEvent::Type::CARD_MATCHED);
    ASSERT_NE(event->getScheduledCardSelectionsResponse(), nullptr);

    auto response = event->getScheduledCardSelectionsResponse();
    ASSERT_NE(std::dynamic_pointer_cast<ScheduledCardSelectionsResponseAdapter>(response), nullptr);
    
    auto adapter = std::dynamic_pointer_cast<ScheduledCardSelectionsResponseAdapter>(response);
    auto responses = adapter->getCardSelectionResponses();
    ASSERT_EQ(static_cast<int>(responses.size()), 3);

    utils.reset();
    
    tearDown();
}

TEST(ObservableLocalReaderSelectionScenarioTest,
     process_card_with_multi_not_matching_selections_with_matchedOnly_return_null)
{
    /* Scenario : first no match, second no match */

    setUp();

    ReaderAdapterTestUtils utils;

    mockReaderWithSelectionResponses(utils.MULTI_NOT_MATCHING_RESPONSES, 
                                     ObservableCardReader::MATCHED_ONLY);

    event = readerSpy->processCardInserted();

    ASSERT_EQ(event, nullptr);

    utils.reset();
    
    tearDown();
}

TEST(ObservableLocalReaderSelectionScenarioTest,
     process_card_with_throw_reader_exception_scenario_return_null) 
{
    setUp();

    EXPECT_CALL(*handler.get(), onReaderObservationError(_,_,_)).WillRepeatedly(Return());
    EXPECT_CALL(*readerSpy.get(), transmitCardSelectionRequests(_,_,_))
        .WillRepeatedly(Throw(ReaderBrokenCommunicationException(
                                  nullptr, 
                                  true, 
                                  "",
                                  std::make_shared<RuntimeException>())));

    const std::vector<std::shared_ptr<CardSelectionRequestSpi>> req = {cardSelectionRequestSpi};
    auto adapter = std::make_shared<CardSelectionScenarioAdapter>(
                       req,
                       MultiSelectionProcessing::PROCESS_ALL,
                       ChannelControl::KEEP_OPEN);
    
    readerSpy->scheduleCardSelectionScenario(adapter,
                                             ObservableCardReader::MATCHED_ONLY,
                                             ObservableCardReader::DetectionMode::SINGLESHOT);

    event = readerSpy->processCardInserted();

    ASSERT_EQ(event, nullptr);

    tearDown();
}

TEST(ObservableLocalReaderSelectionScenarioTest,
     process_card_with_throw_card_exception_scenario_return_null)
{
    setUp();

    EXPECT_CALL(*handler.get(), onReaderObservationError(_,_,_)).WillRepeatedly(Return());
    EXPECT_CALL(*readerSpy.get(), transmitCardSelectionRequests(_,_,_))
        .WillRepeatedly(Throw(CardBrokenCommunicationException(
                                  nullptr, 
                                  true, 
                                  "",
                                  std::make_shared<RuntimeException>())));

    const std::vector<std::shared_ptr<CardSelectionRequestSpi>> req = {cardSelectionRequestSpi};
    auto adapter = std::make_shared<CardSelectionScenarioAdapter>(
                       req,
                       MultiSelectionProcessing::PROCESS_ALL,
                       ChannelControl::KEEP_OPEN);
    
    readerSpy->scheduleCardSelectionScenario(adapter,
                                             ObservableCardReader::MATCHED_ONLY,
                                             ObservableCardReader::DetectionMode::SINGLESHOT);

    event = readerSpy->processCardInserted();

    ASSERT_EQ(event, nullptr);

    tearDown();
}
