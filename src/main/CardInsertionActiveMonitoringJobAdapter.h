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
#include "Reader.h"

/* Keyple Core Util */
#include "LoggerFactory.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::util::cpp;

/**
 * (package-private)<br>
 * This monitoring job polls the Reader#isCardPresent() method to detect a card insertion or
 * a card removal.
 *
 * <p>All runtime exceptions that may occur during the monitoring process are caught and notified at
 * the application level through the
 * keyple::core::service::spi::CardReaderObservationExceptionHandlerSpi mechanism.
 *
 * @since 2.0
 */
class CardInsertionActiveMonitoringJobAdapter final : public AbstractMonitoringJobAdapter {
public:
    /**
     * (package-private)<br>
     * Build a monitoring job to detect the card insertion
     *
     * @param reader reader that will be polled with the method isCardPresent()
     * @param cycleDurationMillis time interval between two presence polls.
     * @param monitorInsertion if true, polls for CARD_INSERTED, else CARD_REMOVED
     * @since 2.0
     */
    CardInsertionActiveMonitoringJobAdapter(std::shared_ptr<ObservableLocalReaderAdapter> reader,
                                            const long cycleDurationMillis,
                                            const bool monitorInsertion);

    /**
     * (package-private)<br>
     * Gets the monitoring process.
     *
     * @return A not null reference.
     * @since 2.0
     */
    virtual std::shared_ptr<Runnable> getMonitoringJob(
        const std::shared_ptr<AbstractObservableStateAdapter> monitoringState) override;

    /**
     * (package-private)<br>
     * Terminates the monitoring process.
     *
     * @since 2.0
     */
    virtual void stop() override;

private:
    /**
     *
     */
    const std::unique_ptr<Logger> mLogger =
        LoggerFactory::getLogger(typeid(CardInsertionActiveMonitoringJobAdapter));

    /**
     *
     */
    const long mCycleDurationMillis;

    /**
     *
     */
    const bool mMonitorInsertion;

    /**
     *
     */
    std::shared_ptr<Reader> mReader;

    /**
     *
     */
    std::atomic<bool> mLoop;

    /**
     *
     */
    class CardInsertionActiveMonitoringJob final : public Runnable {
    public:
        /**
         *
         */
        CardInsertionActiveMonitoringJob(
            std::shared_ptr<AbstractObservableStateAdapter> monitoringState,
            CardInsertionActiveMonitoringJobAdapter* parent);

        /**
         * Monitoring loop
         *
         * <p>Polls for the presence of a card and loops until no card responds. <br>
         * Triggers a CARD_INSERTED event and exits as soon as a communication with a card is
         * established.
         *
         * <p>Any exceptions are notified to the application using the exception handler.
         */
        virtual void run() override final;

    private:
        /**
         *
         */
        long mRetries = 0;

        /**
         *
         */
        std::shared_ptr<AbstractObservableStateAdapter> mMonitoringState;

        /**
         *
         */
        CardInsertionActiveMonitoringJobAdapter* mParent;
    };
};

}
}
}
