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

/* Keyple Core Util */
#include "LoggerFactory.h"

/* Keyple Core Plugin */
#include "WaitForCardInsertionBlockingSpi.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::plugin::spi::reader::observable::state::insertion;
using namespace keyple::core::util::cpp;

/**
 * (package-private)<br>
 * Detect the card insertion thanks to the method
 * WaitForCardInsertionBlockingSpi::waitForCardPresent().
 *
 * <p>This method is invoked in another thread.
 *
 * <p>The job waits indefinitely for the waitForCardPresent method to return unless the stop()
 * method is invoked. In this case, the job is aborted.
 *
 * <p>When a card is present, an internal CARD_INSERTED event is fired.
 *
 * <p>All runtime exceptions that may occur during the monitoring process are caught and notified at
 * the application level through the appropriate exception handler.
 *
 * @since 2.0.0
 */
class CardInsertionPassiveMonitoringJobAdapter final : public AbstractMonitoringJobAdapter {
public:
    /**
     * (package-private)<br>
     * Constructor.
     *
     * /!\ C++: cannot use a shared_ptr for reader as this is called from constructors
     *
     * @param reader The reader.
     * @since 2.0.0
     */
    CardInsertionPassiveMonitoringJobAdapter(ObservableLocalReaderAdapter* reader);

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
        LoggerFactory::getLogger(typeid(CardInsertionPassiveMonitoringJobAdapter));

    /**
     *
     */
    std::shared_ptr<WaitForCardInsertionBlockingSpi> mReaderSpi;

    /**
     *
     */
    class CardInsertionPassiveMonitoringJob final : public Job {
    public:
        /**
         *
         */
        CardInsertionPassiveMonitoringJob(
            const std::shared_ptr<AbstractObservableStateAdapter> monitoringState,
            CardInsertionPassiveMonitoringJobAdapter *parent);

        /**
         * Monitoring loop
         *
         * C++: this replaces run() override
         *
         * <p>Waits for the presence of a card until no card is present. <br>
         * Triggers a CARD_INSERTED event and exits as soon as a communication with a card is
         * established.
         *
         * <p>Any exceptions are notified to the application using the exception handler.
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
        CardInsertionPassiveMonitoringJobAdapter* mParent;
    };
};

}
}
}
