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
#include "AbstractObservableStateAdapter.h"
#include "ObservableLocalReaderAdapter.h"

/* Keyple Core Util */
#include "LoggerFactory.h"

namespace keyple {
namespace core {
namespace service {

/**
 * (package-private)<br>
 * Wait for card removal state implementation.
 *
 * <p>The state in which the card is still present and awaiting removal.
 *
 * <ul>
 *   <li>Upon CARD_REMOVED event, the machine changes state for WAIT_FOR_CARD_INSERTION or
 *       WAIT_FOR_CARD_DETECTION according to the {@link ObservableCardReader.DetectionMode}
 *       setting.
 *   <li>Upon STOP_DETECT event, the machine changes state for WAIT_FOR_CARD_DETECTION.
 * </ul>
 *
 * @since 2.0.0
 */
class WaitForCardRemovalStateAdapter final : public AbstractObservableStateAdapter {
public:
    /**
     * (package-private)<br>
     * Creates an instance.
     *
     * @param reader The observable local reader adapter.
     * @param monitoringJob The monitoring job.
     * @param executorService The executor service to use.
     * @since 2.0.0
     */
    WaitForCardRemovalStateAdapter(
        ObservableLocalReaderAdapter* reader,
        std::shared_ptr<AbstractMonitoringJobAdapter> monitoringJob,
        std::shared_ptr<ExecutorService> executorService);

    /**
     * (package-private)<br>
     * Creates an instance.
     *
     * @param reader The observable local reader adapter.
     * @since 2.0.0
     */
    WaitForCardRemovalStateAdapter(ObservableLocalReaderAdapter* reader);

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
        LoggerFactory::getLogger(typeid(WaitForCardRemovalStateAdapter));
};

}
}
}
