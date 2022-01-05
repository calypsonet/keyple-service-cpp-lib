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

#include "CardRemovalActiveMonitoringJobAdapter.h"

/* Keyple Core Util */
#include "InterruptedException.h"
#include "RuntimeException.h"

/* Keyple Core Plugin */
#include "ReaderIOException.h"
#include "TaskCanceledException.h"

/* Keyple Core Service */
#include "ObservableLocalReaderAdapter.h"
#include "Thread.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::plugin;
using namespace keyple::core::service::cpp;
using namespace keyple::core::util::cpp::exception;

using InternalEvent = ObservableLocalReaderAdapter::InternalEvent;

/* CARD REMOVAL ACTIVE MONITORING JOB ----------------------------------------------------------- */

CardRemovalActiveMonitoringJobAdapter::CardRemovalActiveMonitoringJob
  ::CardRemovalActiveMonitoringJob(std::shared_ptr<AbstractObservableStateAdapter> monitoringState,
                                   CardRemovalActiveMonitoringJobAdapter* parent)
: Job("CardRemovalActiveMonitoringJobAdapter"),
  mMonitoringState(monitoringState),
  mParent(parent) {}

void CardRemovalActiveMonitoringJobAdapter::CardRemovalActiveMonitoringJob::execute()
{
    try {
        mParent->mLogger->debug("[%] Polling from isCardPresentPing\n",
                                mParent->getReader()->getName());

        /* Re-init loop value to true */
        mParent->mLoop = true;

        while (mParent->mLoop) {
            if (!mParent->getReader()->isCardPresentPing()) {
                mParent->mLogger->debug("[%] the card stopped responding\n",
                                        mParent->getReader()->getName());
                mMonitoringState->onEvent(InternalEvent::CARD_REMOVED);
                return;
            }

            mRetries++;

            mParent->mLogger->trace("[%] Polling retries : %\n",
                                    mParent->getReader()->getName(),
                                    mRetries);

            try {
                /* Wait a bit */
                Thread::sleep(mParent->mCycleDurationMillis);
            } catch (const InterruptedException& ignored) {
                (void)ignored;
                /* Restore interrupted state... */
                interrupt();
                mParent->mLoop = false;
            }
        }

        mParent->mLogger->debug("[%] Polling loop has been stopped\n",
                                mParent->getReader()->getName());
    } catch (const RuntimeException& e) {
        mParent->getReader()->getObservationExceptionHandler()
                            ->onReaderObservationError(mParent->getReader()->getPluginName(),
                                                       mParent->getReader()->getName(),
                                                       std::make_shared<RuntimeException>(e));
    }
}

/* CARD REMOVAL ACTIVE MONITORING JOB ADAPTER --------------------------------------------------- */

CardRemovalActiveMonitoringJobAdapter::CardRemovalActiveMonitoringJobAdapter(
  ObservableLocalReaderAdapter* reader, const long cycleDurationMillis)
: AbstractMonitoringJobAdapter(reader),
  mReaderSpi(
      std::dynamic_pointer_cast<WaitForCardRemovalBlockingSpi>(reader->getObservableReaderSpi())),
  mCycleDurationMillis(cycleDurationMillis) {}

std::shared_ptr<Job> CardRemovalActiveMonitoringJobAdapter::getMonitoringJob(
    std::shared_ptr<AbstractObservableStateAdapter> monitoringState)
{
    return std::make_shared<CardRemovalActiveMonitoringJob>(monitoringState, this);
}

void CardRemovalActiveMonitoringJobAdapter::stop()
{
    mLogger->debug("[%] Stop Polling\n", getReader()->getName());

    mLoop = false;
}


}
}
}
