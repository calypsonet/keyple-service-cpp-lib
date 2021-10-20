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

#include "WaitForCardInsertionStateAdapter.h"

namespace keyple {
namespace core {
namespace service {

WaitForCardInsertionStateAdapter::WaitForCardInsertionStateAdapter(
  ObservableLocalReaderAdapter* reader,
  std::shared_ptr<AbstractMonitoringJobAdapter> monitoringJob,
  std::shared_ptr<ExecutorService> executorService)
: AbstractObservableStateAdapter(MonitoringState::WAIT_FOR_CARD_INSERTION,
                                 reader,
                                 monitoringJob,
                                 executorService) {}

WaitForCardInsertionStateAdapter::WaitForCardInsertionStateAdapter(
  ObservableLocalReaderAdapter* reader)
: WaitForCardInsertionStateAdapter(reader, nullptr, nullptr) {}

void WaitForCardInsertionStateAdapter::onEvent(const InternalEvent event)
{
    mLogger->trace("[%] onInternalEvent => Event % received in currentState %\n",
                   getReader()->getName(),
                   event,
                   getMonitoringState());

    /* Process InternalEvent */
    switch (event) {
    case InternalEvent::CARD_INSERTED:
    {
        /* Process default selection if any, return an event, can be null */
        const std::shared_ptr<ReaderEvent> cardEvent = getReader()->processCardInserted();
        if (cardEvent != nullptr) {
            /* Switch internal state */
            switchState(MonitoringState::WAIT_FOR_CARD_PROCESSING);
            /* Notify the external observer of the event */
            getReader()->notifyObservers(cardEvent);
        } else {
            /*
             * If none event was sent to the application, back to card detection
             * stay in the same state, however switch to WAIT_FOR_CARD_INSERTION to relaunch
             * the monitoring job
             */
            mLogger->trace("[%] onInternalEvent => Inserted card hasn't matched\n",
                           getReader()->getName());

            switchState(MonitoringState::WAIT_FOR_CARD_REMOVAL);
        }
        break;
    }
    case InternalEvent::STOP_DETECT:
        switchState(MonitoringState::WAIT_FOR_START_DETECTION);
        break;

    case InternalEvent::CARD_REMOVED:
        /* The card has been removed during default selection */
        if (getReader()->getDetectionMode() == DetectionMode::REPEATING) {
            switchState(MonitoringState::WAIT_FOR_CARD_INSERTION);
        } else {
            switchState(MonitoringState::WAIT_FOR_START_DETECTION);
        }
        break;

    default:
        mLogger->warn("[%] Ignore =>  Event % received in currentState %\n",
                      getReader()->getName(),
                      event,
                      getMonitoringState());
        break;
    }
}

}
}
}
