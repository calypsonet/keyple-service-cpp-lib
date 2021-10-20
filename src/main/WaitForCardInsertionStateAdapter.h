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
#include <typeinfo>

/* Keyple Core Service */
#include "AbstractObservableStateAdapter.h"
#include "ExecutorService.h"
#include "ObservableLocalReaderAdapter.h"

/* Keyple Core Util */
#include "LoggerFactory.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::util::cpp;

using InternalEvent = ObservableLocalReaderAdapter::InternalEvent;

/**
 * (package-private)<br>
 * Wait for card insertion state implementation.
 *
 * <p>The state during which the insertion of a card is expected.
 *
 * <ul>
 *   <li>Upon CARD_INSERTED event, the default selection is processed if required and if the
 *       conditions are met (ALWAYS or CARD_MATCHED) the machine changes state for
 *       WAIT_FOR_CARD_PROCESSING.
 *   <li>Upon STOP_DETECT event, the machine changes state for WAIT_FOR_CARD_DETECTION.
 *   <li>Upon CARD_REMOVED event, the machine changes state for WAIT_FOR_CARD_DETECTION.
 * </ul>
 *
 * @since 2.0.0
 */
class WaitForCardInsertionStateAdapter final : public AbstractObservableStateAdapter {
public:
    /**
     * (package-private)<br>
     * Creates an instance.
     *
     * /!\ C++: cannot use a shared_ptr for reader as this is called from constructors
     *
     * @param reader The observable local reader adapter.
     * @param monitoringJob The monitoring job.
     * @param executorService The executor service to use.
     * @since 2.0.0
     */
    WaitForCardInsertionStateAdapter(
        ObservableLocalReaderAdapter* reader,
        std::shared_ptr<AbstractMonitoringJobAdapter> monitoringJob,
        std::shared_ptr<ExecutorService> executorService);

    /**
     * (package-private)<br>
     * Creates an instance.
     *
     * /!\ C++: cannot use a shared_ptr for reader as this is called from constructors
     *
     * @param reader The observable local reader adapter.
     * @since 2.0.0
     */
    WaitForCardInsertionStateAdapter(ObservableLocalReaderAdapter* reader);

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual void onEvent(const InternalEvent event) override;

private:
    /**
     *
     */
    const std::unique_ptr<Logger> mLogger =
        LoggerFactory::getLogger(typeid(WaitForCardInsertionStateAdapter));
};

}
}
}
