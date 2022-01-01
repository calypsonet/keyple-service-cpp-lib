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

/* Keyple Core Service */
#include "ObservableLocalReaderAdapter.h"

/* Keyple Core Util */
#include "LoggerFactory.h"

/* Mock */
#include "CardReaderObservationExceptionHandlerSpiMock.h"
#include "ObservableReaderNonBlockingSpiMock.h"
#include "ReaderObserverSpiMock.h"

/* Util */
#include "ObservableLocalReaderSuite.h"

using namespace testing;

using namespace keyple::core::service;
using namespace keyple::core::util::cpp;

static const std::string PLUGIN_NAME = "plugin";

class ObservableLocalReaderNonBlockingAdapterTest {};
static const std::shared_ptr<Logger> logger =
    LoggerFactory::getLogger(typeid(ObservableLocalReaderNonBlockingAdapterTest));

static std::shared_ptr<ObservableLocalReaderAdapter> _reader;
static std::shared_ptr<ObservableReaderNonBlockingSpiMock> readerSpi;
static std::shared_ptr<ReaderObserverSpiMock> observer;
static std::shared_ptr<CardReaderObservationExceptionHandlerSpiMock> handler;
static std::shared_ptr<ObservableLocalReaderSuite> testSuite;

/*
 * With
 * WaitForCardInsertionNonBlockingSpi,
 * WaitForCardRemovalNonBlockingSpi,
 * DontWaitForCardRemovalDuringProcessingSpi,
 */
static void setUp()
{
    readerSpi = std::make_shared<ObservableReaderNonBlockingSpiMock>(READER_NAME);
    handler = std::make_shared<CardReaderObservationExceptionHandlerSpiMock>();
    EXPECT_CALL(*handler.get(), onReaderObservationError(_, _, _)).WillRepeatedly(Return());
    _reader = std::make_shared<ObservableLocalReaderAdapter>(readerSpi, PLUGIN_NAME);
    observer = std::make_shared<ReaderObserverSpiMock>(nullptr);
    testSuite =  std::make_shared<ObservableLocalReaderSuite>(_reader, readerSpi, observer, handler, logger);
    
    _reader->doRegister();
}

static void tearDown()
{
    _reader->doUnregister();

    testSuite.reset();
    observer.reset();
    _reader.reset();
    handler.reset();
    readerSpi.reset();
}

TEST(ObservableLocalReaderNonBlockingAdapterTest, initReader_addObserver_startDetection)
{
    setUp();

    testSuite->addFirstObserver_should_startDetection();

    tearDown();
}

TEST(ObservableLocalReaderNonBlockingAdapterTest, removeObserver)
{
    setUp();

    testSuite->removeLastObserver_should_StopDetection();

    tearDown();
}

TEST(ObservableLocalReaderNonBlockingAdapterTest, clearObservers)
{
    setUp();

    testSuite->clearObservers_shouldRemove_allObservers();

    tearDown();
}

static void __insertCard_shouldNotify_CardInsertedEvent()
{
    testSuite->insertCard_onWaitForCard_shouldNotify_CardInsertedEvent();
}

TEST(ObservableLocalReaderNonBlockingAdapterTest, insertCard_shouldNotify_CardInsertedEvent)
{
    setUp();

    __insertCard_shouldNotify_CardInsertedEvent();

    tearDown();
}

TEST(ObservableLocalReaderNonBlockingAdapterTest, finalizeCardProcessing_afterInsert_switchState)
{
    setUp();

    testSuite->finalizeCardProcessing_afterInsert_switchState();

    tearDown();
}

TEST(ObservableLocalReaderNonBlockingAdapterTest, removeCard_afterFinalize_shouldNotify_CardRemoved)
{
    setUp();

    testSuite->removeCard_afterFinalize_shouldNotify_CardRemoved();

    tearDown();
}

TEST(ObservableLocalReaderNonBlockingAdapterTest, removeCard_beforeFinalize_shouldNotify_CardRemoved)
{
    setUp();

    __insertCard_shouldNotify_CardInsertedEvent();

    logger->debug("Remove card...\n");
    readerSpi->setCardPresent(false);

    std::this_thread::sleep_for(std::chrono::seconds(1));

    /* Card removal is not monitored, no event is thrown */
    ASSERT_EQ(_reader->getCurrentMonitoringState(), MonitoringState::WAIT_FOR_CARD_PROCESSING);

    tearDown();
}
