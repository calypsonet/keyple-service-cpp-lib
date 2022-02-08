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
#include "AbstractMonitoringJobAdapter.h"
#include "Job.h"
#include "MonitoringState.h"

/* Keyple Core Plugin */
#include "WaitForCardRemovalBlockingSpi.h"
#include "WaitForCardRemovalDuringProcessingBlockingSpi.h"

/* Keyple Core Util */
#include "LoggerFactory.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::plugin::spi::reader::observable::state::processing;
using namespace keyple::core::plugin::spi::reader::observable::state::removal;
using namespace keyple::core::util::cpp;

/**
 * (package-private)<br>
 * Detect the card removal thanks to the method WaitForCardRemovalBlockingSpi:waitForCardRemoval()
 * or WaitForCardRemovalDuringProcessingBlockingSpi::waitForCardRemovalDuringProcessing() depending
 * of the provided SPI.
 *
 * <p>This method is invoked in another thread
 *
 * <p>This job should be used by readers who have the ability to natively detect the disappearance
 * of the card during a communication session with an ES (between two APDU exchanges).
 *
 * <p>PC/SC readers have this capability.
 *
 * <p>If the card is removed during processing, then an internal CARD_REMOVED event is triggered.
 *
 * <p>If a communication problem with the reader occurs (KeypleReaderIOException) an internal
 * STOP_DETECT event is fired.
 *
 * <p>All runtime exceptions that may occur during the monitoring process are caught and notified at
 * the application level through the appropriate exception handler.
 *
 * @since 2.0.0
 */
class CardRemovalPassiveMonitoringJobAdapter final : public AbstractMonitoringJobAdapter {
public:
    /**
     * (package-private)<br>
     * Constructor.
     *
     * /!\ C++: cannot use a shared_ptr for reader as this is called from constructors
     *
     * @param reader reference to the reader
     * @since 2.0.0
     */
    CardRemovalPassiveMonitoringJobAdapter(ObservableLocalReaderAdapter* reader);

    /**
     * (package-private)<br>
     * Gets the monitoring process.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    virtual std::shared_ptr<Job> getMonitoringJob(
        std::shared_ptr<AbstractObservableStateAdapter> monitoringState) override;

    /**
     * (package-private)<br>
     * Terminates the monitoring process.
     *
     * @since 2.0.0
     */
    virtual void stop() override;

private:
    /**
     *
     */
    const std::unique_ptr<Logger> mLogger =
        LoggerFactory::getLogger(typeid(CardRemovalPassiveMonitoringJobAdapter));

    /**
     *
     */
    std::shared_ptr<WaitForCardRemovalBlockingSpi> mReaderSpi;

    /**
     *
     */
    std::shared_ptr<WaitForCardRemovalDuringProcessingBlockingSpi> mReaderProcessingSpi;

    /**
     *
     */
    class CardRemovalPassiveMonitoringJob final : public Job {
    public:
        /**
         *
         */
        CardRemovalPassiveMonitoringJob(
            const std::shared_ptr<AbstractObservableStateAdapter> monitoringState,
            CardRemovalPassiveMonitoringJobAdapter *parent);

        /**
         *
         */
        void execute() final;

    private:
        /**
         *
         */
        const std::shared_ptr<AbstractObservableStateAdapter> mMonitoringState;

        /**
         *
         */
        CardRemovalPassiveMonitoringJobAdapter* mParent;
    };
};

}
}
}
