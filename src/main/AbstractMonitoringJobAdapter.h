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

/* Keyple Service */
#include "AbstractObservableStateAdapter.h"
#include "ObservableLocalReaderAdapter.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::service;

/**
 * (package-private)<br>
 * Abstract class for all monitoring jobs.
 *
 * @since 2.0
 */
class AbstractMonitoringJobAdapter {
public:
    /**
     * (package-private)<br>
     * Creates an instance.
     *
     * @param reader The reader.
     * @since 2.0
     */
    AbstractMonitoringJobAdapter(const std::shared_ptr<ObservableLocalReaderAdapter> reader)
    : mReader(reader) {}

    /**
     * (package-private)<br>
     * Gets the reader.
     *
     * @return A not null reference.
     * @since 2.0
     */
    virtual std::shared_ptr<ObservableLocalReaderAdapter> getReader() const final
    {
        return mReader;
    }

    /**
     * (package-private)<br>
     * Gets the task of the monitoring job.
     *
     * @param monitoringState reference to the state the monitoring job in running against.
     * @return A not null reference.
     * @since 2.0
     */
    virtual Runnable getMonitoringJob(
        const std::shared_ptr<AbstractObservableStateAdapter> monitoringState) = 0;

    /**
     * (package-private)<br>
     * Stops/interrupts the monitoring job
     *
     * @since 2.0
     */
    virtual void stop() = 0;

private:
    /**
     *
     */
    const std::shared_ptr<ObservableLocalReaderAdapter> mReader;
};

}
}
}
