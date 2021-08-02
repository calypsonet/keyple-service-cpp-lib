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

#include "CardRemovalPassiveMonitoringJobAdapter.h"

/* Keyple Core Util */
#include "RuntimeException.h"

/* Keyple Core Plugin */
#include "ReaderIOException.h"
#include "TaskCanceledException.h"

/* Keyple Core Service */
#include "ObservableLocalReaderAdapter.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::plugin;
using namespace keyple::core::util::cpp::exception;

using InternalEvent = ObservableLocalReaderAdapter::InternalEvent;

/* CARD REMOVAL PASSIVE MONITORING JOB ---------------------------------------------------------- */

CardRemovalPassiveMonitoringJobAdapter::CardRemovalPassiveMonitoringJob
  ::CardRemovalPassiveMonitoringJob(std::shared_ptr<AbstractObservableStateAdapter> monitoringState,
                                    CardRemovalPassiveMonitoringJobAdapter* parent)
: mMonitoringState(monitoringState), mParent(parent) {}

void CardRemovalPassiveMonitoringJobAdapter::CardRemovalPassiveMonitoringJob::run()
{
    try {
        // FIXME
        while (true) { //(!Thread.currentThread().isInterrupted()) {
            try {
                mParent->mReaderSpi->waitForCardRemoval();
                mMonitoringState->onEvent(InternalEvent::CARD_REMOVED);
                break;
            } catch (const ReaderIOException& e) {
                (void)e;

                /* Just warn as it can be a disconnection of the reader */
                mParent->mLogger->warn("[%] waitForCardAbsentNative => Error while processing " \
                                       "card removal event\n",
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
                                                       e);
    }
}

/* CARD REMOVAL PASSIVE MONITORING JOB ADAPTER -------------------------------------------------- */

CardRemovalPassiveMonitoringJobAdapter::CardRemovalPassiveMonitoringJobAdapter(
  ObservableLocalReaderAdapter* reader)
: AbstractMonitoringJobAdapter(reader),
  mReaderSpi(
      std::dynamic_pointer_cast<WaitForCardRemovalBlockingSpi>(reader->getObservableReaderSpi())) {}

std::shared_ptr<Job> CardRemovalPassiveMonitoringJobAdapter::getMonitoringJob(
    std::shared_ptr<AbstractObservableStateAdapter> monitoringState)
{
    return std::make_shared<CardRemovalPassiveMonitoringJob>(monitoringState, this);
}

void CardRemovalPassiveMonitoringJobAdapter::stop()
{
    mReaderSpi->stopWaitForCardRemoval();
}


}
}
}
