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
#include "ObservableReaderBlockingSpiMock.h"
#include "ReaderObserverSpiMock.h"

/* Util */
#include "ObservableLocalReaderSuite.h"

using namespace testing;

using namespace keyple::core::service;
using namespace keyple::core::util::cpp;

static const std::string PLUGIN_NAME = "plugin";

static std::shared_ptr<ObservableLocalReaderAdapter> _reader;
static std::shared_ptr<ObservableReaderBlockingSpiMock> readerSpi;
static std::shared_ptr<ReaderObserverSpiMock> observer;
static std::shared_ptr<CardReaderObservationExceptionHandlerSpiMock> handler;
static std::shared_ptr<ObservableLocalReaderSuite> testSuite;

static const long waitInsertion = 1000;
static const long waitRemoval = 1000;

class ObservableLocalReaderBlockingAdapterTest {};
static const std::shared_ptr<Logger> logger =
    LoggerFactory::getLogger(typeid(ObservableLocalReaderBlockingAdapterTest));

/*
* With
* WaitForCardInsertionBlockingSpi,
* WaitForCardRemovalBlockingSpi,
* WaitForCardRemovalDuringProcessingBlockingSpi
*/
static void setUp()
{
    readerSpi = std::make_shared<ObservableReaderBlockingSpiMock>(READER_NAME, waitInsertion, waitRemoval);
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

TEST(ObservableLocalReaderBlockingAdapterTest, initReader_addObserver_startDetection)
{
    setUp();

    testSuite->addFirstObserver_should_startDetection();

    tearDown();
}

TEST(ObservableLocalReaderBlockingAdapterTest, removeObserver)
{
    setUp();

    testSuite->removeLastObserver_should_StopDetection();

    tearDown();
}

TEST(ObservableLocalReaderBlockingAdapterTest, clearObservers)
{
    setUp();

    testSuite->clearObservers_shouldRemove_allObservers();

    tearDown();
}

TEST(ObservableLocalReaderBlockingAdapterTest, insertCard_shouldNotify_CardInsertedEvent)
{
    setUp();

    testSuite->insertCard_onWaitForCard_shouldNotify_CardInsertedEvent();

    tearDown();
}

TEST(ObservableLocalReaderBlockingAdapterTest, finalizeCardProcessing_afterInsert_switchState)
{
    setUp();

    testSuite->finalizeCardProcessing_afterInsert_switchState();

    tearDown();
}

TEST(ObservableLocalReaderBlockingAdapterTest, removeCard_afterFinalize_shouldNotify_CardRemoved)
{
    setUp();

    testSuite->removeCard_afterFinalize_shouldNotify_CardRemoved();

    tearDown();
}

TEST(ObservableLocalReaderBlockingAdapterTest, removeCard_beforeFinalize_shouldNotify_CardRemoved)
{
    setUp();

    /* TODO flaky?, depends on what values are given for waitInsertion and waitRemoval */
    testSuite->removeCard_beforeFinalize_shouldNotify_CardRemoved();

    tearDown();
}
