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

#include "AbstractObservableStateAdapter.h"

#include <typeinfo>

namespace keyple {
namespace core {
namespace service {

AbstractObservableStateAdapter::AbstractObservableStateAdapter(
  const MonitoringState monitoringState,
  ObservableLocalReaderAdapter* reader,
  std::shared_ptr<AbstractMonitoringJobAdapter> monitoringJob,
  std::shared_ptr<ExecutorService> executorService)
: mMonitoringState(monitoringState),
  mReader(reader),
  mMonitoringJob(monitoringJob),
  mExecutorService(executorService) {}

AbstractObservableStateAdapter::AbstractObservableStateAdapter(
  const MonitoringState monitoringState, ObservableLocalReaderAdapter* reader)
: AbstractObservableStateAdapter(monitoringState, reader, nullptr, nullptr) {}

MonitoringState AbstractObservableStateAdapter::getMonitoringState() const
{
    return mMonitoringState;
}

ObservableLocalReaderAdapter* AbstractObservableStateAdapter::getReader() const
{
    return mReader;
}

void AbstractObservableStateAdapter::switchState(const MonitoringState stateId)
{
    mReader->switchState(stateId);
}

void AbstractObservableStateAdapter::onActivate()
{
    mLogger->trace("[%] onActivate => %\n", mReader->getName(), getMonitoringState());

    /* Launch the monitoringJob if necessary */
    if (mMonitoringJob != nullptr) {
        if (mExecutorService == nullptr) {
            throw IllegalStateException("ExecutorService must be set");
        }

        mMonitoringEvent = mExecutorService->submit(mMonitoringJob->getMonitoringJob(shared_from_this()));
    }
}

void AbstractObservableStateAdapter::onDeactivate()
{
    mLogger->trace("[%] onDeactivate => %\n", mReader->getName(), getMonitoringState());

    /* Cancel the monitoringJob if necessary */
    if (mMonitoringEvent != nullptr && !mMonitoringEvent->isDone()) {
        mMonitoringJob->stop();

        const bool canceled = mMonitoringEvent->cancel(false);
        const auto& id = *mMonitoringJob.get();
        mLogger->trace("[%] onDeactivate => cancel monitoring job % by thread interruption %\n",
                       mReader->getName(),
                       typeid(id).name(),
                       canceled);
    }

    /* Make sure pointer is set to nullptr to force Job/Thread destruction */
    mMonitoringEvent = nullptr;
}

}
}
}
