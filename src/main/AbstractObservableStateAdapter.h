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

#include <future>
#include <memory>
#include <typeinfo>

/* Keyple Core Util */
#include "ExecutorService.h"
#include "LoggerFactory.h"

/* Keyple Core Service */
#include "AbstractMonitoringJobAdapter.h"
#include "Job.h"
#include "MonitoringState.h"
#include "ObservableLocalReaderAdapter.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::service::cpp;
using namespace keyple::core::util::cpp;

using InternalEvent = ObservableLocalReaderAdapter::InternalEvent;

class AbstractMonitoringJobAdapter;

/**
 * (package-private)<br>
 * Abstract class for all states of a {@link ObservableLocalReaderAdapter}.
 *
 * @since 2.0.0
 */
class AbstractObservableStateAdapter
: public std::enable_shared_from_this<AbstractObservableStateAdapter> {
public:
    /**
     * (package-private)<br>
     * Create a new state with a state identifier and a monitor job
     *
     * /!\ C++: cannot use a shared_ptr for reader as this is called from constructors
     *
     * @param monitoringState the state identifier
     * @param reader the current reader
     * @param monitoringJob the job to be executed in background (may be null if no background job is
     *        required)
     * @param executorService the executor service
     * @since 2.0.0
     */
    AbstractObservableStateAdapter(
        const MonitoringState monitoringState,
        ObservableLocalReaderAdapter* reader,
        std::shared_ptr<AbstractMonitoringJobAdapter> monitoringJob,
        std::shared_ptr<ExecutorService> executorService);

    /**
     * (package-private)<br>
     * Create a new state with a state identifier without monitoring job.
     *
     * /!\ C++: cannot use a shared_ptr for reader as this is called from constructors
     *
     * @param reader observable reader this currentState is attached to
     * @param monitoringState name of the currentState
     * @since 2.0.0
     */
    AbstractObservableStateAdapter(const MonitoringState monitoringState,
                                   ObservableLocalReaderAdapter* reader);

    /**
     * 
     */
    virtual ~AbstractObservableStateAdapter() = default;

    /**
     * (package-private)<br>
     * Get the current state identifier of the state machine
     *
     * @return the current state identifier
     * @since 2.0.0
     */
    virtual MonitoringState getMonitoringState() const final;

    /**
     * (package-private)<br>
     * Gets the reader.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    virtual ObservableLocalReaderAdapter* getReader() const final;

    /**
     * (package-private)<br>
     * Switch state in the parent reader
     *
     * @param stateId the new state
     * @since 2.0.0
     */
    virtual void switchState(const MonitoringState stateId) final;

    /**
     * (package-private)<br>
     * Invoked when activated, a custom behaviour can be added here.
     *
     * @since 2.0.0
     * @throw IllegalStateException if a job is defined with a null executor service.
     */
    virtual void onActivate() final;

    /**
     * (package-private)<br>
     * Invoked when deactivated.
     *
     * @since 2.0.0
     */
    virtual void onDeactivate() final;

    /**
     * (package-private)<br>
     * Handle Internal Event.
     *
     * @param event internal event received by reader
     * @since 2.0.0
     */
    virtual void onEvent(InternalEvent event) = 0;

private:
    /**
     *
     */
    const std::shared_ptr<Logger> mLogger =
        LoggerFactory::getLogger(typeid(AbstractObservableStateAdapter));

    /**
     * Identifier of the currentState
     */
    MonitoringState mMonitoringState;

    /**
     * Reference to Reader
     */
    ObservableLocalReaderAdapter* mReader;

    /**
     * Background job definition if any
     */
    std::shared_ptr<AbstractMonitoringJobAdapter> mMonitoringJob;

    /**
     * Result of the background job if any
     */
    std::shared_ptr<Job> mMonitoringEvent;

    /**
     * Executor service used to execute AbstractMonitoringJobAdapter
     */
    std::shared_ptr<ExecutorService> mExecutorService;
};

}
}
}
