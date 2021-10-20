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

/* Calypsonet Terminal Reader */
#include "CardSelectionManager.h"
#include "ObservableCardReader.h"

/* Calypsonet Terminal Card */
#include "CardSelectionSpi.h"
#include "ChannelControl.h"

/* Keple Core Service */
#include "MultiSelectionProcessing.h"

namespace keyple {
namespace core {
namespace service {

using namespace calypsonet::terminal::card;
using namespace calypsonet::terminal::card::spi;
using namespace calypsonet::terminal::reader::selection;

using DetectionMode = ObservableCardReader::DetectionMode;
using NotificationMode = ObservableCardReader::NotificationMode;

/**
 * (package-private) <br>
 * Implementation of the {@link CardSelectionManager}.
 *
 * @since 2.0.0
 */
class CardSelectionManagerAdapter final : public CardSelectionManager {
public:
    /**
     * (package-private) <br>
     * Creates an instance of the service with which the selection stops as soon as a card matches a
     * selection case.
     *
     * @since 2.0.0
     */
    CardSelectionManagerAdapter();

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual void setMultipleSelectionMode() override final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual int prepareSelection(const std::shared_ptr<CardSelection> cardSelection) override final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual void prepareReleaseChannel() override final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual const std::shared_ptr<CardSelectionResult> processCardSelectionScenario(
        std::shared_ptr<CardReader> reader) override final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual void scheduleCardSelectionScenario(
        std::shared_ptr<ObservableCardReader> observableCardReader,
        const DetectionMode detectionMode,
        const NotificationMode notificationMode) override final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual const std::shared_ptr<CardSelectionResult> parseScheduledCardSelectionsResponse(
        const std::shared_ptr<ScheduledCardSelectionsResponse> scheduledCardSelectionsResponse) 
            const override final;

private:
    /**
     *
     */
    std::vector<std::shared_ptr<CardSelectionSpi>> mCardSelections;

    /**
     *
     */
    std::vector<std::shared_ptr<CardSelectionRequestSpi>> mCardSelectionRequests;
    
    /**
     * 
     */
    MultiSelectionProcessing mMultiSelectionProcessing;
    
    /**
     * 
     */
    ChannelControl mChannelControl;

    /**
     * (private)<br>
     * Analyzes the responses received in return of the execution of a card selection scenario and
     * returns the CardSelectionResult.
     *
     * @param cardSelectionResponses The card selection responses.
     * @return A not null reference.
     * @throw IllegalArgumentException If the list is null or empty.
     */
    const std::shared_ptr<CardSelectionResult> processCardSelectionResponses(
        const std::vector<std::shared_ptr<CardSelectionResponseApi>>& cardSelectionResponses) const;
};

}
}
}