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

#include <memory>

/* Keyple Core Service */
#include "AbstractObservableStateAdapter.h"
#include "Job.h"
#include "ObservableLocalReaderAdapter.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::service::cpp;

/**
 * (package-private)<br>
 * Abstract class for all monitoring jobs.
 *
 * @since 2.0.0
 */
class AbstractMonitoringJobAdapter {
public:
    /**
     * (package-private)<br>
     * Creates an instance.
     *
     * /!\ C++: cannot use a shared_ptr for reader as this is called from constructors
     *
     * @param reader The reader.
     * @since 2.0.0
     */
    AbstractMonitoringJobAdapter(ObservableLocalReaderAdapter* reader);

    /**
     * 
     */
    virtual ~AbstractMonitoringJobAdapter() = default;

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
     * Gets the task of the monitoring job.
     *
     * @param monitoringState reference to the state the monitoring job in running against.
     * @return A not null reference.
     * @since 2.0.0
     */
    virtual std::shared_ptr<Job> getMonitoringJob(
        const std::shared_ptr<AbstractObservableStateAdapter> monitoringState) = 0;

    /**
     * (package-private)<br>
     * Stops/interrupts the monitoring job
     *
     * @since 2.0.0
     */
    virtual void stop() = 0;

private:
    /**
     *
     */
    ObservableLocalReaderAdapter* mReader;
};

}
}
}
