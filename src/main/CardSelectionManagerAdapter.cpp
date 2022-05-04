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

#include "CardSelectionManagerAdapter.h"

/* Keyple Core Util */
#include "KeypleAssert.h"

/* Calypsonet Terminal Card */
#include "CardBrokenCommunicationException.h"
#include "CardSelectionResponseApi.h"
#include "ParseException.h"
#include "ReaderBrokenCommunicationException.h"

/* Calypsonet Terminal Reader */
#include "InvalidCardResponseException.h"
#include "ReaderCommunicationException.h"

/* Keyple Core Service */
#include "AbstractReaderAdapter.h"
#include "CardCommunicationException.h"
#include "CardSelectionResultAdapter.h"
#include "CardSelectionScenarioAdapter.h"
#include "ObservableLocalReaderAdapter.h"
#include "ScheduledCardSelectionsResponseAdapter.h"

namespace keyple {
namespace core {
namespace service {

using namespace calypsonet::terminal::card;
using namespace calypsonet::terminal::card::spi;
using namespace calypsonet::terminal::reader;
using namespace calypsonet::terminal::reader::selection;
using namespace keyple::core::util;

CardSelectionManagerAdapter::CardSelectionManagerAdapter()
: mMultiSelectionProcessing(MultiSelectionProcessing::FIRST_MATCH),
  mChannelControl(ChannelControl::KEEP_OPEN) {}

void CardSelectionManagerAdapter::setMultipleSelectionMode()
{
    mMultiSelectionProcessing = MultiSelectionProcessing::PROCESS_ALL;
}

int CardSelectionManagerAdapter::prepareSelection(
    const std::shared_ptr<CardSelection> cardSelection)
{
    Assert::getInstance().notNull(cardSelection, "cardSelection");

    /* Keep the selection request */
    auto selection = std::dynamic_pointer_cast<CardSelectionSpi>(cardSelection);
    mCardSelections.push_back(selection);
    std::shared_ptr<CardSelectionRequestSpi> selectionRequest = selection->getCardSelectionRequest();
    mCardSelectionRequests.push_back(selectionRequest);

    /* Return the selection index (starting at 0) */
    return static_cast<int>(mCardSelections.size()) - 1;
}

void CardSelectionManagerAdapter::prepareReleaseChannel()
{
    mChannelControl = ChannelControl::CLOSE_AFTER;
}

const std::shared_ptr<CardSelectionResult>
    CardSelectionManagerAdapter::processCardSelectionScenario(std::shared_ptr<CardReader> reader)
{
        Assert::getInstance().notNull(reader, "reader");

        /* Communicate with the card to make the actual selection */
        std::vector<std::shared_ptr<CardSelectionResponseApi>> cardSelectionResponses;

        try {
            cardSelectionResponses =
                std::dynamic_pointer_cast<AbstractReaderAdapter>(reader)
                    ->transmitCardSelectionRequests(mCardSelectionRequests,
                                                    mMultiSelectionProcessing,
                                                    mChannelControl);
        } catch (const ReaderBrokenCommunicationException& e) {
            throw ReaderCommunicationException(
                e.getMessage(), std::make_shared<ReaderBrokenCommunicationException>(e));
        } catch (const CardBrokenCommunicationException& e) {
            throw CardCommunicationException(e.getMessage(),
                                             std::make_shared<CardBrokenCommunicationException>(e));
        }

        /* Clear the selection requests list */
        mCardSelectionRequests.clear();

        /* Analyze the received responses */
        return processCardSelectionResponses(cardSelectionResponses);
    }

    void CardSelectionManagerAdapter::scheduleCardSelectionScenario(
        std::shared_ptr<ObservableCardReader> observableCardReader,
        const DetectionMode detectionMode,
        const NotificationMode notificationMode)
{

    Assert::getInstance().notNull(observableCardReader, "observableCardReader");

    auto cardSelectionScenario =
        std::make_shared<CardSelectionScenarioAdapter>(mCardSelectionRequests,
                                                        mMultiSelectionProcessing,
                                                        mChannelControl);

    auto local = std::dynamic_pointer_cast<ObservableLocalReaderAdapter>(observableCardReader);
//        auto remote =std::dynamic_pointer_cast<ObservableRemoteReaderAdapter>(observableCardReader);
    if (local) {
        local->scheduleCardSelectionScenario(cardSelectionScenario,
                                                notificationMode,
                                                detectionMode);
//        } else if (remote) {
//            remote->scheduleCardSelectionScenario(cardSelectionScenario,
//                                                  notificationMode,
//                                                  detectionMode);
    } else {
        throw IllegalArgumentException("Not a Keyple reader implementation.");
    }
}

const std::shared_ptr<CardSelectionResult>
    CardSelectionManagerAdapter::parseScheduledCardSelectionsResponse(
        const std::shared_ptr<ScheduledCardSelectionsResponse> scheduledCardSelectionsResponse)
            const
{

    Assert::getInstance().notNull(scheduledCardSelectionsResponse,
                                  "scheduledCardSelectionsResponse");

    return processCardSelectionResponses(
        std::static_pointer_cast<ScheduledCardSelectionsResponseAdapter>(scheduledCardSelectionsResponse)
            ->getCardSelectionResponses());
}

const std::shared_ptr<CardSelectionResult>
    CardSelectionManagerAdapter::processCardSelectionResponses(
        const std::vector<std::shared_ptr<CardSelectionResponseApi>>& cardSelectionResponses) const
{
    Assert::getInstance().notEmpty(cardSelectionResponses, "cardSelectionResponses");

    auto cardSelectionsResult = std::make_shared<CardSelectionResultAdapter>();

    int index = 0;

    /* Check card responses */
    for (const auto& cardSelectionResponse : cardSelectionResponses) {
        if (cardSelectionResponse->hasMatched()) {
            /* Invoke the parse method defined by the card extension to retrieve the smart card */
            std::shared_ptr<SmartCard> smartCard = nullptr;
            try {
                smartCard = std::dynamic_pointer_cast<SmartCard>(
                                mCardSelections[index]->parse(cardSelectionResponse));
            } catch (const ParseException& e) {
                throw InvalidCardResponseException(
                          "Error occurred while parsing the card response: " + e.getMessage(),
                          std::make_shared<ParseException>(e));
            }

            cardSelectionsResult->addSmartCard(index, smartCard);
        }

        index++;
    }

    return cardSelectionsResult;
}

}
}
}