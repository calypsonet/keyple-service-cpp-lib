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
#include "ReaderSpi.h"

/* Keyple Core Service */
#include "ExecutorService.h"
#include "ObservableLocalReaderAdapter.h"

/* Mock */
#include "CardReaderObservationExceptionHandlerSpiMock.h"
#include "CardResponseApiMock.h"
#include "CardSelectionRequestSpiMock.h"
#include "CardSelectionResponseApiMock.h"
#include "ObservableLocalPluginSpiMock.h"
#include "ObservableReaderAutonomousSpiMock.h"
#include "PluginObservationExceptionHandlerSpiMock.h"
#include "PluginObserverSpiMock.h"
#include "ReaderObserverSpiMock.h"
#include "ReaderSpiMock.h"

/* Util */
#include "ObservableLocalReaderSuite.h"

using namespace testing;

using namespace keyple::core::plugin;
using namespace keyple::core::plugin::spi;
using namespace keyple::core::plugin::spi::reader;
using namespace keyple::core::service;
using namespace keyple::core::util::cpp::exception;

static const std::string PLUGIN_NAME = "plugin";

static std::shared_ptr<ObservableLocalReaderAdapter> _reader;
static std::shared_ptr<ObservableReaderAutonomousSpiMock> readerSpi;
static std::shared_ptr<ReaderObserverSpiMock> observer;
static std::shared_ptr<CardReaderObservationExceptionHandlerSpiMock> handler;
static std::shared_ptr<ObservableLocalReaderSuite> testSuite;
static std::shared_ptr<ExecutorService> notificationExecutorService;

static std::shared_ptr<CardSelectionRequestSpi> cardSelectionRequestSpi;
static std::shared_ptr<CardSelectionResponseApi> cardSelectionResponseApi;
static std::shared_ptr<CardResponseApi> cardResponseApi;
static std::shared_ptr<ReaderEvent> event;

class ObservableLocalReaderAutonomousAdapterTest {};
static const std::shared_ptr<Logger> logger =
    LoggerFactory::getLogger(typeid(ObservableLocalReaderAutonomousAdapterTest));

static void setUp()
{
    notificationExecutorService = std::make_shared<ExecutorService>();
    readerSpi = std::make_shared<ObservableReaderAutonomousSpiMock>(READER_NAME);
    handler = std::make_shared<CardReaderObservationExceptionHandlerSpiMock>();
    EXPECT_CALL(*handler.get(), onReaderObservationError(_, _, _)).WillRepeatedly(Return());
    _reader = std::make_shared<ObservableLocalReaderAdapter>(readerSpi, PLUGIN_NAME);
    observer = std::make_shared<ReaderObserverSpiMock>(nullptr);
    testSuite = std::make_shared<ObservableLocalReaderSuite>(_reader, readerSpi, observer, handler, logger);
    cardSelectionRequestSpi = std::make_shared<CardSelectionRequestSpiMock>();
    cardSelectionResponseApi = std::make_shared<CardSelectionResponseApiMock>();
    cardResponseApi = std::make_shared<CardResponseApiMock>();

    /* Test with event notification executor service */
    _reader->doRegister();
}

static void tearDown()
{
    _reader->doUnregister();

    cardResponseApi.reset();
    cardSelectionResponseApi.reset();
    cardSelectionRequestSpi.reset();
    testSuite.reset();
    observer.reset();
    _reader.reset();
    handler.reset();
    readerSpi.reset();
    notificationExecutorService.reset();
}

/*
 *  With ObservableReaderAutonomousSpi
 */

TEST(ObservableLocalReaderAutonomousAdapterTest, initReader_addObserver_startDetection)
{
    setUp();

    testSuite->addFirstObserver_should_startDetection();

    tearDown();
}

TEST(ObservableLocalReaderAutonomousAdapterTest, removeObserver)
{
    setUp();

    testSuite->removeLastObserver_should_StopDetection();

    tearDown();
}

TEST(ObservableLocalReaderAutonomousAdapterTest, clearObservers)
{
    setUp();

    testSuite->clearObservers_shouldRemove_allObservers();

    tearDown();
}

TEST(ObservableLocalReaderAutonomousAdapterTest, insertCard_shouldNotify_CardInsertedEvent)
{
    setUp();

    testSuite->insertCard_onWaitForCard_shouldNotify_CardInsertedEvent();

    tearDown();
}

TEST(ObservableLocalReaderAutonomousAdapterTest, finalizeCardProcessing_afterInsert_switchState)
{
    setUp();

    testSuite->finalizeCardProcessing_afterInsert_switchState();

    tearDown();
}

TEST(ObservableLocalReaderAutonomousAdapterTest, removeCard_afterFinalize_shouldNotify_CardRemoved)
{
    setUp();

    testSuite->removeCard_afterFinalize_shouldNotify_CardRemoved();

    tearDown();
}

TEST(ObservableLocalReaderAutonomousAdapterTest, removeCard_beforeFinalize_shouldNotify_CardRemoved)
{
    setUp();

    testSuite->removeCard_beforeFinalize_shouldNotify_CardRemoved();

    tearDown();
}

/*
 * Method of ObservableLocalReaderAdapter
 */

TEST(ObservableLocalReaderAutonomousAdapterTest, observerThrowsError_shouldBe_transferTo_handler)
{
    setUp();

    auto e = std::make_shared<RuntimeException>();
    auto readerObserver = std::make_shared<ReaderObserverSpiMock>(e);
    testSuite->setObserver(readerObserver);
    testSuite->insertCard_onWaitForCard_shouldNotify_CardInsertedEvent();

    EXPECT_CALL(*handler.get(), onReaderObservationError(_, READER_NAME, _)).WillRepeatedly(Return());

    tearDown();
}
