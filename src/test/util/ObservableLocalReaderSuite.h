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

#pragma once

#include "gmock/gmock.h"
#include "gtest/gtest.h"

/* Calypsonet Terminal Reader */
#include "CardReaderObservationExceptionHandlerSpi.h"

/* Keyple Core Service */
#include "MonitoringState.h"
#include "ObservableLocalReaderAdapter.h"

/* Keyple Core Util */
#include "LoggerFactory.h"

/* Mock */
#include "ControllableReaderSpiMock.h"
#include "ReaderObserverSpiMock.h"

using namespace testing;

using namespace calypsonet::terminal::reader::spi;

using namespace keyple::core::util::cpp;
using namespace keyple::core::service;

static const std::string READER_NAME = "reader";

class ObservableLocalReaderSuite {
public:
    ObservableLocalReaderSuite(std::shared_ptr<ObservableLocalReaderAdapter> reader,
                               std::shared_ptr<ControllableReaderSpiMock> readerSpi,
                               std::shared_ptr<ReaderObserverSpiMock> observer,
                               std::shared_ptr<CardReaderObservationExceptionHandlerSpi> handler,
                               std::shared_ptr<Logger> logger)
    : mReader(reader), 
      mReaderSpi(readerSpi), 
      mObserver(observer), 
      mHandler(handler), 
      mLogger(logger) {}

    void addFirstObserver_should_startDetection()
    {
        ASSERT_EQ(mReader->getCurrentMonitoringState(), MonitoringState::WAIT_FOR_START_DETECTION);

        mReader->setReaderObservationExceptionHandler(mHandler);
        mReader->addObserver(mObserver);
        mReader->startCardDetection(ObservableCardReader::DetectionMode::REPEATING);
        
        ASSERT_EQ(mReader->countObservers(), 1);
        ASSERT_EQ(mReader->getCurrentMonitoringState(), MonitoringState::WAIT_FOR_CARD_INSERTION);
    }

    void removeLastObserver_should_StopDetection()
    {
        addFirstObserver_should_startDetection();
        mReader->removeObserver(mObserver);
        
        ASSERT_EQ(mReader->countObservers(), 0);

        /* State is not changed */
        ASSERT_EQ(mReader->getCurrentMonitoringState(), MonitoringState::WAIT_FOR_CARD_INSERTION);
    }

    void clearObservers_shouldRemove_allObservers() 
    {
        addFirstObserver_should_startDetection();
        mReader->clearObservers();
        
        ASSERT_EQ(mReader->countObservers(), 0);
    }

    void insertCard_onWaitForCard_shouldNotify_CardInsertedEvent()
    {
        addFirstObserver_should_startDetection();
        mLogger->debug("Insert card...\n");
        mReaderSpi->setCardPresent(true);

        /* Wait until state is WAIT_FOR_CARD_PROCESSING, should not take longer than 2 sec */
        std::this_thread::sleep_for(std::chrono::seconds(2));

        /* Check event is well formed */
        auto event = mObserver->getLastEventOfType(CardReaderEvent::Type::CARD_INSERTED);
        
        ASSERT_NE(event, nullptr);
        ASSERT_EQ(event->getReaderName(), READER_NAME);
    }

    void finalizeCardProcessing_afterInsert_switchState()
    {
        insertCard_onWaitForCard_shouldNotify_CardInsertedEvent();

        mLogger->debug("Finalize processing...\n");
        mReader->finalizeCardProcessing();

        /* Wait until state is WAIT_FOR_CARD_REMOVAL, should not take longer than 3 sec */
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    // @Test
    void removeCard_afterFinalize_shouldNotify_CardRemoved()
    {
        finalizeCardProcessing_afterInsert_switchState();

        mLogger->debug("Remove card...\n");
        mReaderSpi->setCardPresent(false);

        /* Wait until state is WAIT_FOR_CARD_INSERTION, should not take longer than 1 sec */
        std::this_thread::sleep_for(std::chrono::seconds(1));

        /* Check event is well formed */
        auto event = mObserver->getLastEventOfType(CardReaderEvent::Type::CARD_REMOVED);
        
        ASSERT_EQ(event->getReaderName(), READER_NAME);
    }

    // @Test
    void removeCard_beforeFinalize_shouldNotify_CardRemoved()
    {
        insertCard_onWaitForCard_shouldNotify_CardInsertedEvent();

        mLogger->debug("Remove card...\n");
        mReaderSpi->setCardPresent(false);

        /* Wait until state is WAIT_FOR_CARD_INSERTION, should not take longer than 2 sec */
        std::this_thread::sleep_for(std::chrono::seconds(2));

        /* Check event is well formed */
        auto event = mObserver->getLastEventOfType(CardReaderEvent::Type::CARD_REMOVED);
        
        ASSERT_EQ(event->getReaderName(), READER_NAME);
    }

    // /*
    // * Callables
    // */

    // private Callable<Boolean> eventOfTypeIsReceived(final CardReaderEvent.Type eventType) {
    //     return new Callable<Boolean>() {
    //     @Override
    //     public Boolean call() throws Exception {
    //         return observer.hasReceived(eventType);
    //     }
    //     };
    // }

    // private Callable<Boolean> stateIs(
    //     final AbstractObservableStateAdapter.MonitoringState monitoringState) {
    //     return new Callable<Boolean>() {
    //     @Override
    //     public Boolean call() throws Exception {
    //         logger.trace(
    //             "TEST ... wait for {} is {}", reader.getCurrentMonitoringState(), monitoringState);
    //         return reader.getCurrentMonitoringState().equals(monitoringState);
    //     }
    //     };
    // }

    void setObserver(std::shared_ptr<ReaderObserverSpiMock> observer) 
    {
        mObserver = observer;
    }

private:
    std::shared_ptr<ObservableLocalReaderAdapter> mReader;
    std::shared_ptr<ControllableReaderSpiMock> mReaderSpi;
    std::shared_ptr<ReaderObserverSpiMock> mObserver;
    std::shared_ptr<CardReaderObservationExceptionHandlerSpi> mHandler;
    const std::shared_ptr<Logger> mLogger;
};