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

#include "CardInsertionActiveMonitoringJobAdapter.h"

/* Keyple Core Util */
#include "InterruptedException.h"

/* Keyple Core Service */
#include "Thread.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::util::cpp::exception;

using InternalEvent = ObservableLocalReaderAdapter::InternalEvent;

/* CARD INSERTION ACTIVE MONITORING JOB --------------------------------------------------------- */

CardInsertionActiveMonitoringJobAdapter::CardInsertionActiveMonitoringJob
  ::CardInsertionActiveMonitoringJob(
      std::shared_ptr<AbstractObservableStateAdapter> monitoringState,
      CardInsertionActiveMonitoringJobAdapter* parent)
: Job("CardInsertionActiveMonitoringJobAdapter - " + parent->mReader->getName()),
  mMonitoringState(monitoringState),
  mParent(parent) {}

void CardInsertionActiveMonitoringJobAdapter::CardInsertionActiveMonitoringJob::execute()
{
    try {
        mParent->mLogger->debug("[%] Polling from isCardPresent\n", mParent->mReader->getName());

        /* Re-init loop value to true */
        mParent->mLoop = true;

        while (mParent->mLoop) {
            /* Polls for CARD_INSERTED */
            if (mParent->mMonitorInsertion && mParent->mReader->isCardPresent()) {
                mParent->mLogger->debug("[%] The card is present\n", mParent->mReader->getName());
                mMonitoringState->onEvent(InternalEvent::CARD_INSERTED);
                return;
            }

            /* Polls for CARD_REMOVED */
            if (!mParent->mMonitorInsertion && !mParent->mReader->isCardPresent()) {
                mParent->mLogger->debug("[%] The card is not present\n",
                                        mParent->mReader->getName());
                mParent->mLoop = false;
                mMonitoringState->onEvent(InternalEvent::CARD_REMOVED);
                return;
            }

            mRetries++;

            mParent->mLogger->trace("[%] isCardPresent polling retries : %\n",
                                    mParent->mReader->getName(),
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

        mParent->mLogger->trace("[%] Looping has been stopped\n", mParent->mReader->getName());

    } catch (const RuntimeException& e) {
        dynamic_cast<ObservableLocalReaderAdapter*>(mParent->mReader)
            ->getObservationExceptionHandler()
            ->onReaderObservationError(
                dynamic_cast<ObservableLocalReaderAdapter*>(mParent->mReader)->getPluginName(),
                mParent->mReader->getName(),
                std::make_shared<RuntimeException>(e));
    }
}

/* CARD INSERTION ACTIVE MONITORING JOB ADAPTER ------------------------------------------------- */

CardInsertionActiveMonitoringJobAdapter::CardInsertionActiveMonitoringJobAdapter(
  ObservableLocalReaderAdapter* reader,
  const long cycleDurationMillis,
  const bool monitorInsertion)
: AbstractMonitoringJobAdapter(reader),
  mCycleDurationMillis(cycleDurationMillis),
  mMonitorInsertion(monitorInsertion),
  mReader(reader) {}

std::shared_ptr<Job> CardInsertionActiveMonitoringJobAdapter::getMonitoringJob(
    const std::shared_ptr<AbstractObservableStateAdapter> monitoringState)
{
    return std::make_shared<CardInsertionActiveMonitoringJob>(monitoringState, this);
}

void CardInsertionActiveMonitoringJobAdapter::stop()
{
    mLogger->debug("[%] Stop polling\n", mReader->getName());

    mLoop = false;
}

}
}
}
