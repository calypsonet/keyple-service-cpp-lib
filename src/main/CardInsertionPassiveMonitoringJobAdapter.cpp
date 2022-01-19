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

#include "CardInsertionPassiveMonitoringJobAdapter.h"

/* Keyple Core Plugin */
#include "ReaderIOException.h"
#include "TaskCanceledException.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::plugin;
using namespace keyple::core::util::cpp::exception;

using InternalEvent = ObservableLocalReaderAdapter::InternalEvent;

/* CARD INSERTION PASSIVE MONITORING JOB -------------------------------------------------------- */

CardInsertionPassiveMonitoringJobAdapter::CardInsertionPassiveMonitoringJob
  ::CardInsertionPassiveMonitoringJob(
      std::shared_ptr<AbstractObservableStateAdapter> monitoringState,
      CardInsertionPassiveMonitoringJobAdapter* parent)
: Job("CardInsertionPassiveMonitoringJob"), mMonitoringState(monitoringState), mParent(parent) {}

void CardInsertionPassiveMonitoringJobAdapter::CardInsertionPassiveMonitoringJob::execute()
{
    try {
        while (!isInterrupted()) {
            try {
                mParent->mReaderSpi->waitForCardInsertion();
                mMonitoringState->onEvent(InternalEvent::CARD_INSERTED);
                break;
            } catch (const ReaderIOException& e) {
                (void)e;
                /* Just warn as it can be a disconnection of the reader */
                mParent->mLogger->warn("[%] waitForCardPresent => Error while processing card " \
                                       "insertion event\n",
                                       mParent->getReader()->getName());
                break;
            } catch (const TaskCanceledException& e) {
                (void)e;
                break;
            }
        }
    } catch (const RuntimeException& e) {
        mParent->getReader()->getObservationExceptionHandler()
                            ->onReaderObservationError(mParent->getReader()->getPluginName(),
                                                       mParent->getReader()->getName(),
                                                       std::make_shared<RuntimeException>(e));
    }
}

/* CARD INSERTION PASSIVE MONITORING JOB ADAPTER ------------------------------------------------ */

CardInsertionPassiveMonitoringJobAdapter::CardInsertionPassiveMonitoringJobAdapter(
  ObservableLocalReaderAdapter* reader)
: AbstractMonitoringJobAdapter(reader),
  mReaderSpi(
      std::dynamic_pointer_cast<WaitForCardInsertionBlockingSpi>(reader->getObservableReaderSpi()))
{}

std::shared_ptr<Job> CardInsertionPassiveMonitoringJobAdapter::getMonitoringJob(
    std::shared_ptr<AbstractObservableStateAdapter> monitoringState)
{
    return std::make_shared<CardInsertionPassiveMonitoringJob>(monitoringState, this);
}

void CardInsertionPassiveMonitoringJobAdapter::stop()
{
    mLogger->trace("[%] stopWaitForCard on reader\n", getReader()->getName());

    mReaderSpi->stopWaitForCardInsertion();
}

}
}
}
