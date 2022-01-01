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

#include <atomic>
#include <memory>
#include <typeinfo>

/* Keyple Core Service */
#include "AbstractMonitoringJobAdapter.h"
#include "MonitoringState.h"
#include "Job.h"

/* Keyple Core Plugin */
#include "WaitForCardRemovalBlockingSpi.h"

/* Keyple Core Util */
#include "LoggerFactory.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::plugin::spi::reader::observable::state::removal;
using namespace keyple::core::util::cpp;

/**
 * (package-private)<br>
 * Ping the card to detect removal thanks to the method {@link
 * ObservableLocalReaderAdapter#isCardPresentPing()}.
 *
 * <p>This method is invoked in another thread.
 *
 * <p>This job should be used by readers who do not have the ability to natively detect the
 * disappearance of the card at the end of the transaction.
 *
 * <p>It is based on sending a neutral APDU command as long as the card is responding, an internal
 * CARD_REMOVED event is fired when the card is no longer responding.
 *
 * <p>By default a delay of 200 ms is inserted between each APDU sending .
 *
 * <p>All runtime exceptions that may occur during the monitoring process are caught and notified at
 * the application level through the appropriate exception handler.
 *
 * @since 2.0.0
 */
class CardRemovalActiveMonitoringJobAdapter final : public AbstractMonitoringJobAdapter {
public:
    /**
     * (package-private)<br>
     * Create a job monitor job that ping the card with the method isCardPresentPing()
     *
     * /!\ C++: cannot use a shared_ptr for reader as this is called from constructors
     *
     * @param reader reference to the reader
     * @param cycleDurationMillis delay between between each APDU sending
     * @since 2.0.0
     */
    CardRemovalActiveMonitoringJobAdapter(ObservableLocalReaderAdapter* reader,
                                          const long cycleDurationMillis);

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
        LoggerFactory::getLogger(typeid(CardRemovalActiveMonitoringJobAdapter));

    /**
     *
     */
    std::shared_ptr<WaitForCardRemovalBlockingSpi> mReaderSpi;

    /**
     *
     */
    const long mCycleDurationMillis;

    /**
     *
     */
    std::atomic<bool> mLoop;

    /**
     *
     */
    class CardRemovalActiveMonitoringJob final : public Job {
    public:
        /**
         *
         */
        CardRemovalActiveMonitoringJob(
            const std::shared_ptr<AbstractObservableStateAdapter> monitoringState,
            CardRemovalActiveMonitoringJobAdapter *parent);

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
        CardRemovalActiveMonitoringJobAdapter* mParent;

        /**
         *
         */
        long mRetries;
    };
};

}
}
}
