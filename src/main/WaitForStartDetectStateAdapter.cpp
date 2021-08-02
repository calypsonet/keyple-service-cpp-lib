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

#include "WaitForStartDetectStateAdapter.h"

namespace keyple {
namespace core {
namespace service {

WaitForStartDetectStateAdapter::WaitForStartDetectStateAdapter(
  ObservableLocalReaderAdapter* reader,
  std::shared_ptr<AbstractMonitoringJobAdapter> monitoringJob,
  std::shared_ptr<ExecutorService> executorService)
: AbstractObservableStateAdapter(MonitoringState::WAIT_FOR_START_DETECTION,
                                 reader,
                                 monitoringJob,
                                 executorService) {}

WaitForStartDetectStateAdapter::WaitForStartDetectStateAdapter(
  ObservableLocalReaderAdapter* reader)
: WaitForStartDetectStateAdapter(reader, nullptr, nullptr) {}

void WaitForStartDetectStateAdapter::onEvent(const InternalEvent event)
{
    mLogger->trace("[%] onInternalEvent => Event % received in currentState %\n",
                   getReader()->getName(),
                   event,
                   getMonitoringState());

    /* Process InternalEvent */
    switch (event) {
    case InternalEvent::START_DETECT:
        switchState(MonitoringState::WAIT_FOR_CARD_INSERTION);
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
