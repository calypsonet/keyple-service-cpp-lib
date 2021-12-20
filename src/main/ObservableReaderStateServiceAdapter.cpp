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

#include "ObservableReaderStateServiceAdapter.h"

/* Keyple Core Util */
#include "IllegalStateException.h"

/* Keyple Core Plugin */
#include "DontWaitForCardRemovalDuringProcessingSpi.h"
#include "WaitForCardInsertionAutonomousSpi.h"
#include "WaitForCardInsertionBlockingSpi.h"
#include "WaitForCardInsertionNonBlockingSpi.h"
#include "WaitForCardRemovalAutonomousSpi.h"
#include "WaitForCardRemovalDuringProcessingBlockingSpi.h"
#include "WaitForCardRemovalNonBlockingSpi.h"

/* Keyple Core Service */
#include "CardInsertionActiveMonitoringJobAdapter.h"
#include "CardInsertionPassiveMonitoringJobAdapter.h"
#include "CardRemovalActiveMonitoringJobAdapter.h"
#include "CardRemovalPassiveMonitoringJobAdapter.h"
#include "WaitForCardInsertionStateAdapter.h"
#include "WaitForCardProcessingStateAdapter.h"
#include "WaitForCardRemovalStateAdapter.h"
#include "WaitForStartDetectStateAdapter.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::plugin::spi::reader::observable::state::insertion;
using namespace keyple::core::plugin::spi::reader::observable::state::processing;
using namespace keyple::core::plugin::spi::reader::observable::state::removal;
using namespace keyple::core::util::cpp::exception;

ObservableReaderStateServiceAdapter::ObservableReaderStateServiceAdapter(
  ObservableLocalReaderAdapter* reader)
: mReader(reader),
  mReaderSpi(reader->getObservableReaderSpi()),
  mExecutorService(std::make_shared<ExecutorService>())
{
    /* Wait for start */
    mStates.insert({MonitoringState::WAIT_FOR_START_DETECTION,
                    std::make_shared<WaitForStartDetectStateAdapter>(mReader)});

    /* Insertion */
    if (std::dynamic_pointer_cast<WaitForCardInsertionAutonomousSpi>(mReaderSpi)) {
        mStates.insert({MonitoringState::WAIT_FOR_CARD_INSERTION,
                        std::make_shared<WaitForCardInsertionStateAdapter>(mReader)});
    } else if (std::dynamic_pointer_cast<WaitForCardInsertionNonBlockingSpi>(mReaderSpi)) {
        auto cardInsertionActiveMonitoringJobAdapter =
            std::make_shared<CardInsertionActiveMonitoringJobAdapter>(mReader, 200, true);
        mStates.insert({MonitoringState::WAIT_FOR_CARD_INSERTION,
                        std::make_shared<WaitForCardInsertionStateAdapter>(
                            mReader,
                            cardInsertionActiveMonitoringJobAdapter,
                            mExecutorService)});
    } else if (std::dynamic_pointer_cast<WaitForCardInsertionBlockingSpi>(mReaderSpi)) {
        auto cardInsertionPassiveMonitoringJobAdapter =
            std::make_shared<CardInsertionPassiveMonitoringJobAdapter>(mReader);
        mStates.insert({MonitoringState::WAIT_FOR_CARD_INSERTION,
                        std::make_shared<WaitForCardInsertionStateAdapter>(
                            mReader,
                            cardInsertionPassiveMonitoringJobAdapter,
                            mExecutorService)});
    } else {
        throw IllegalStateException("Reader should implement implement a WaitForCardInsertion " \
                                    "interface.");
    }

    /* Processing */
    if (std::dynamic_pointer_cast<WaitForCardRemovalDuringProcessingBlockingSpi>(mReaderSpi)) {
        auto cardRemovalPassiveMonitoringJobAdapter =
            std::make_shared<CardRemovalPassiveMonitoringJobAdapter>(mReader);
        mStates.insert({MonitoringState::WAIT_FOR_CARD_PROCESSING,
                        std::make_shared<WaitForCardProcessingStateAdapter>(
                            mReader,
                            cardRemovalPassiveMonitoringJobAdapter,
                            mExecutorService)});
    } else if (std::dynamic_pointer_cast<DontWaitForCardRemovalDuringProcessingSpi>(mReaderSpi)) {
        mStates.insert({MonitoringState::WAIT_FOR_CARD_PROCESSING,
                        std::make_shared<WaitForCardProcessingStateAdapter>(mReader)});
    } else {
        throw IllegalStateException("Reader should implement implement a Wait/DontWait " \
                                    "ForCardRemovalDuringProcessing interface.");
    }

    /* Removal */
    if (std::dynamic_pointer_cast<WaitForCardRemovalAutonomousSpi>(mReaderSpi)) {
        mStates.insert({MonitoringState::WAIT_FOR_CARD_REMOVAL,
                        std::make_shared<WaitForCardRemovalStateAdapter>(mReader)});
    } else if (std::dynamic_pointer_cast<WaitForCardRemovalNonBlockingSpi>(mReaderSpi)) {
        auto cardRemovalActiveMonitoringJobAdapter =
            std::make_shared<CardRemovalActiveMonitoringJobAdapter>(mReader, 200);
        mStates.insert({MonitoringState::WAIT_FOR_CARD_REMOVAL,
            std::make_shared<WaitForCardRemovalStateAdapter>(mReader,
                                                             cardRemovalActiveMonitoringJobAdapter,
                                                             mExecutorService)});
    } else if (std::dynamic_pointer_cast<WaitForCardRemovalBlockingSpi>(mReaderSpi)) {
        auto cardRemovalPassiveMonitoringJobAdapter =
            std::make_shared<CardRemovalPassiveMonitoringJobAdapter>(mReader);
        mStates.insert({MonitoringState::WAIT_FOR_CARD_REMOVAL,
                        std::make_shared<WaitForCardRemovalStateAdapter>(
                            mReader,
                            cardRemovalPassiveMonitoringJobAdapter,
                            mExecutorService)});
    } else {
        throw IllegalStateException("Reader should implement implement a WaitForCardRemoval " \
                                    "interface.");
    }

    switchState(MonitoringState::WAIT_FOR_START_DETECTION);
}

void ObservableReaderStateServiceAdapter::onEvent(const InternalEvent event)
{
    /* C++: cannot use std::lock_guard as mutex needs to be unlocked before calling onEvent */
    mMutex.lock();

    switch (event) {
    case InternalEvent::CARD_INSERTED:
    case InternalEvent::CARD_REMOVED:
    case InternalEvent::CARD_PROCESSED:
    case InternalEvent::TIME_OUT:
        break;
    case InternalEvent::START_DETECT:
        mReaderSpi->onStartDetection();
        break;
    case InternalEvent::STOP_DETECT:
        mReaderSpi->onStopDetection();
        break;
    }

    mMutex.unlock();
    mCurrentState->onEvent(event);
}

void ObservableReaderStateServiceAdapter::switchState(const MonitoringState stateId)
{
    std::lock_guard<std::mutex> lock(mMutex);

    if (mCurrentState != nullptr) {
        mLogger->trace("[%] Switch currentState from % to %\n",
                       mReader->getName(),
                       mCurrentState->getMonitoringState(),
                       stateId);
        mCurrentState->onDeactivate();
    } else {
        mLogger->trace("[%] Switch to a new currentState %\n", mReader->getName(), stateId);
    }

    /* Switch currentState */
    mCurrentState = mStates.at(stateId);

    /* onActivate the new current state */
    mCurrentState->onActivate();
}

std::shared_ptr<AbstractObservableStateAdapter> ObservableReaderStateServiceAdapter::getCurrentState()
{
    std::lock_guard<std::mutex> lock(mMutex);

    return mCurrentState;
}

MonitoringState ObservableReaderStateServiceAdapter::getCurrentMonitoringState()
{
    std::lock_guard<std::mutex> lock(mMutex);

    return mCurrentState->getMonitoringState();
}

void ObservableReaderStateServiceAdapter::shutdown()
{
    mExecutorService->shutdown();
}

}
}
}
