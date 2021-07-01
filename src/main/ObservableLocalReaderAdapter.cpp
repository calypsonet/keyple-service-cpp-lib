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

#include "ObservableLocalReaderAdapter.h"

/* Calypsonet Terminal Card */
#include "CardBrokenCommunicationException.h"
#include "ReaderBrokenCommunicationException.h"

/* Keyple Core Plugin */
#include "CardIOException.h"
#include "ReaderIOException.h"

namespace keyple {
namespace core {
namespace service {

using namespace calypsonet::terminal::card;
using namespace keyple::core::plugin;

const std::vector<uint8_t> ObservableLocalReaderAdapter::APDU_PING_CARD_PRESENCE = {
    0x00, 0xC0, 0x00, 0x00, 0x00};
const std::string ObservableLocalReaderAdapter::READER_MONITORING_ERROR =
    "An error occurred while monitoring the reader.";

ObservableLocalReaderAdapter::ObservableLocalReaderAdapter(
  std::shared_ptr<ObservableReaderSpi> observableReaderSpi, const std::string& pluginName)
: LocalReaderAdapter(observableReaderSpi),
  mObservableReaderSpi(observableReaderSpi),
  mStateService(std::make_shared<ObservableReaderStateServiceAdapter>(this)),
  mObservationManager(
      std::make_shared<ObservationManagerAdapter<CardReaderObserverSpi,
                                                 CardReaderObservationExceptionHandlerSpi>>(
        pluginName, getName()))
{
    if (std::dynamic_pointer_cast<WaitForCardInsertionAutonomousSpi>observableReaderSpi) {
        observableReaderSpi->connect(this);
    }

    if (std::dynamic_pointer_cast<WaitForCardRemovalAutonomousSpi>(observableReaderSpi)) {
        observableReaderSpi->connect(this);
    }
}

std::shared_ptr<ObservableReaderSpi> ObservableLocalReaderAdapter::getObservableReaderSpi() const
{
    return mObservableReaderSpi;
}

std::shared_ptr<CardReaderObservationExceptionHandlerSpi>
    ObservableLocalReaderAdapter::getObservationExceptionHandler() const
{
    return mObservationManager->getObservationExceptionHandler();
}

DetectionMode ObservableLocalReaderAdapter::getdetectionMode()  const
{
    return mDetectionMode;
}

MonitoringState ObservableLocalReaderAdapter::getCurrentMonitoringState() const
{
    return mStateService->getCurrentMonitoringState();
}

bool ObservableLocalReaderAdapter::isCardPresentPing()
{
    /* Transmits the APDU and checks for the IO exception */
    try {
        mLogger->trace("[%] Ping card\n", getName());

        mObservableReaderSpi->transmitApdu(APDU_PING_CARD_PRESENCE);
    } catch (const ReaderIOException& e) {
        /* Notify the reader communication failure with the exception handler */
        getObservationExceptionHandler()
            ->onReaderObservationError(
                getPluginName(),
                getName(),
                std::make_shared<ReaderCommunicationException>(READER_MONITORING_ERROR, e));
    } catch (const CardIOException& e) {
        mLogger->trace("[%] Exception occurred in isCardPresentPing. Message: %\n",
                       getName(),
                       e.getMessage());

        return false;
    }

    return true;
}

std::shared_ptr<ReaderEvent> ObservableLocalReaderAdapter::processCardInserted()
{
    mLogger->trace("[%] process the inserted card\n", getName());

    if (mCardSelectionScenario == nullptr) {
        mLogger->trace("[%] no card selection scenario defined, notify CARD_INSERTED\n", getName());

        /* No default request is defined, just notify the card insertion */
        return std::make_shared<ReaderEventAdapter>(getPluginName(),
                                                    getName(),
                                                    CardReaderEvent::Type::CARD_INSERTED,
                                                    nullptr);
    }

    /*
     * A card selection scenario is defined, send it and notify according to the notification mode
     * and the selection status
     */
    try {
        const std::vector<std::shared_ptr<CardSelectionResponseApi>> cardSelectionResponses =
            transmitCardSelectionRequests(
                mCardSelectionScenario.getCardSelectionRequests(),
                mCardSelectionScenario.getMultiSelectionProcessing(),
                mCardSelectionScenario.getChannelControl());

        if (hasACardMatched(cardSelectionResponses)) {
            return std::make_shared<ReaderEventAdapter>(
                       getPluginName(),
                       getName(),
                       CardReaderEvent::Type::CARD_MATCHED,
                       std::make_shared<ScheduledCardSelectionsResponseAdapter>(
                           cardSelectionResponses));
        }

        if (mNotificationMode == NotificationMode::MATCHED_ONLY) {
            /* Notify only if a card matched the selection, just ignore if not */
            mLogger->trace("[%] selection hasn't matched, does not throw any event because of " \
                           "MATCHED_ONLY flag\n",
                           getName());

            return nullptr;
        }

        /* The card didn't match, notify an CARD_INSERTED event with the received response */
        mLogger->trace("[%] none of % default selection matched\n",
                       getName(),
                       cardSelectionResponses.size());

        return std::make_shared<ReaderEventAdapter>(
                   getPluginName(),
                   getName(),
                   CardReaderEvent::Type::CARD_INSERTED,
                   std::make_shared<ScheduledCardSelectionsResponseAdapter>(
                       cardSelectionResponses));

    } catch (const ReaderBrokenCommunicationException& e) {
        /* Notify the reader communication failure with the exception handler */
        getObservationExceptionHandler()
            ->onReaderObservationError(
                getPluginName(),
                getName(),
                std::make_shared<ReaderCommunicationException>(READER_MONITORING_ERROR, e));

    } catch (const CardBrokenCommunicationException& e) {
        /* The last transmission failed, close the logical and physical channels */
        closeLogicalAndPhysicalChannelsSilently();

        /*
         * The card was removed or not read correctly, no exception raising or event notification,
         * just log.
         */
        mLogger->debug("A card error or communication exception occurred while processing the " \
                       "card selection scenario. %\n",
                       e.getMessage());
    }

    /*
     * Here we close the physical channel in case it was opened for a card excluded by the selection
     * scenario.
     */
    try {
        mObservableReaderSpi->closePhysicalChannel();
    } catch (const ReaderIOException& e) {
        /* Notify the reader communication failure with the exception handler */
        getObservationExceptionHandler()
            ->onReaderObservationError(
                getPluginName(),
                getName(),
                std::make_shared<ReaderCommunicationException>(READER_MONITORING_ERROR, e));
    }

    /* No event returned */
    return nullptr;
}

}
}
}
