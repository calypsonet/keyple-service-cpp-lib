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

/* Calypsonet Terminal Reader */
#include "ReaderCommunicationException.h"

/* Keyple Core Plugin */
#include "CardIOException.h"
#include "ObservableReaderStateServiceAdapter.h"
#include "ReaderIOException.h"
#include "WaitForCardInsertionAutonomousSpi.h"
#include "WaitForCardRemovalAutonomousSpi.h"

/* Keyple Core Service */
#include "ReaderEventAdapter.h"
#include "ScheduledCardSelectionsResponseAdapter.h"

/* Keyple Core Util */
#include "Exception.h"
#include "KeypleAssert.h"

namespace keyple {
namespace core {
namespace service {

using namespace calypsonet::terminal::card;
using namespace calypsonet::terminal::reader;
using namespace keyple::core::plugin;
using namespace keyple::core::plugin::spi::reader::observable::state::insertion;
using namespace keyple::core::plugin::spi::reader::observable::state::removal;
using namespace keyple::core::util::cpp;
using namespace keyple::core::util::cpp::exception;

/* OBSERVABLE LOCAL READER ADAPTER JOB ---------------------------------------------------------- */

ObservableLocalReaderAdapter::ObservableLocalReaderAdapterJob::ObservableLocalReaderAdapterJob(
  std::shared_ptr<CardReaderObserverSpi> observer,
  const std::shared_ptr<ReaderEvent> event,
  ObservableLocalReaderAdapter* parent)
: mObserver(observer), mEvent(event), mParent(parent) {}

void ObservableLocalReaderAdapter::ObservableLocalReaderAdapterJob::run()
{
    mParent->notifyObserver(mObserver, mEvent);
}

/* OBSERVABLE LOCAL READER ADAPTER -------------------------------------------------------------- */

const std::vector<uint8_t> ObservableLocalReaderAdapter::APDU_PING_CARD_PRESENCE = {
    0x00, 0xC0, 0x00, 0x00, 0x00};
const std::string ObservableLocalReaderAdapter::READER_MONITORING_ERROR =
    "An error occurred while monitoring the reader.";

ObservableLocalReaderAdapter::ObservableLocalReaderAdapter(
  std::shared_ptr<ObservableReaderSpi> observableReaderSpi, const std::string& pluginName)
: LocalReaderAdapter(observableReaderSpi, pluginName),
  mObservableReaderSpi(observableReaderSpi),
  mStateService(std::make_shared<ObservableReaderStateServiceAdapter>(shared_from_this())),
  mObservationManager(
      std::make_shared<ObservationManagerAdapter<CardReaderObserverSpi,
                                                 CardReaderObservationExceptionHandlerSpi>>(
          pluginName, getName()))
{
    auto insert = std::dynamic_pointer_cast<WaitForCardInsertionAutonomousSpi>(observableReaderSpi);
    if (insert) {
        insert->connect(
            std::dynamic_pointer_cast<WaitForCardInsertionAutonomousReaderApi>(shared_from_this()));
    }

    auto remove = std::dynamic_pointer_cast<WaitForCardRemovalAutonomousSpi>(observableReaderSpi);
    if (remove) {
        remove->connect(
            std::dynamic_pointer_cast<WaitForCardRemovalAutonomousReaderApi>(shared_from_this()));
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
        const ReaderCommunicationException ex(READER_MONITORING_ERROR, e);
        getObservationExceptionHandler()->onReaderObservationError(getPluginName(),
                                                                   getName(),
                                                                   ex);
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
                mCardSelectionScenario->getCardSelectionRequests(),
                mCardSelectionScenario->getMultiSelectionProcessing(),
                mCardSelectionScenario->getChannelControl());

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
        const ReaderCommunicationException ex(READER_MONITORING_ERROR, e);
        getObservationExceptionHandler()->onReaderObservationError(getPluginName(),
                                                                   getName(),
                                                                   ex);

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
        const ReaderCommunicationException ex(READER_MONITORING_ERROR, e);
        getObservationExceptionHandler()->onReaderObservationError(getPluginName(),
                                                                   getName(),
                                                                   ex);
    }

    /* No event returned */
    return nullptr;
}

bool ObservableLocalReaderAdapter::hasACardMatched(
        const std::vector<std::shared_ptr<CardSelectionResponseApi>>& cardSelectionResponses)
{
    for (const auto& cardSelectionResponse : cardSelectionResponses) {
        if (cardSelectionResponse != nullptr && cardSelectionResponse->hasMatched()) {
            mLogger->trace("[%] a default selection has matched\n", getName());
            return true;
        }
    }

    return false;
}

void ObservableLocalReaderAdapter::processCardRemoved()
{
    closeLogicalAndPhysicalChannelsSilently();
    notifyObservers(
        std::make_shared<ReaderEventAdapter>(getPluginName(),
                                             getName(),
                                             CardReaderEvent::Type::CARD_REMOVED,
                                             nullptr));
}

void ObservableLocalReaderAdapter::switchState(const MonitoringState stateId)
{
    mStateService->switchState(stateId);
}

void ObservableLocalReaderAdapter::notifyObservers(const std::shared_ptr<ReaderEvent> event)
{
    mLogger->debug("The reader '%' is notifying the reader event '%' to % observers\n",
                   getName(),
                   event->getType(),
                   countObservers());

    std::vector<std::shared_ptr<CardReaderObserverSpi>> observers =
        mObservationManager->getObservers();

    if (mObservationManager->getEventNotificationExecutorService() == nullptr) {
        /* Synchronous notification */
        for (const auto& observer : observers) {
            notifyObserver(observer, event);
        }
    } else {
        /* Asynchronous notification */
        for (const auto& observer : observers) {
            auto job = std::make_shared<ObservableLocalReaderAdapterJob>(observer, event, this);
            mObservationManager->getEventNotificationExecutorService()->execute(job);
        }
    }
}

void ObservableLocalReaderAdapter::notifyObserver(std::shared_ptr<CardReaderObserverSpi> observer,
                                                  const std::shared_ptr<ReaderEvent> event)
{
    try {
        observer->onReaderEvent(event);
    } catch (const Exception& e) {
        try {
            mObservationManager->getObservationExceptionHandler()
                               ->onReaderObservationError(getPluginName(), getName(), e);
        } catch (const Exception& e2) {
            mLogger->error("Exception during notification", e2);
            mLogger->error("Original cause", e);
        }
    }
}

void ObservableLocalReaderAdapter::scheduleCardSelectionScenario(
    std::shared_ptr<CardSelectionScenarioAdapter> cardSelectionScenario,
    const NotificationMode notificationMode,
    const DetectionMode detectionMode)
{
    mCardSelectionScenario = cardSelectionScenario;
    mNotificationMode = notificationMode;
    mDetectionMode = detectionMode;
}

void ObservableLocalReaderAdapter::doUnregister()
{
    LocalReaderAdapter::doUnregister();

    try {
        notifyObservers(
            std::make_shared<ReaderEventAdapter>(getPluginName(),
                                                 getName(),
                                                 CardReaderEvent::Type::UNAVAILABLE,
                                                 nullptr));
        stopCardDetection();
    } catch (const Exception& e) {

    }

    /* Finally */
    clearObservers();
    mStateService->shutdown();
}

bool ObservableLocalReaderAdapter::isCardPresent()
{
    checkStatus();

    if (LocalReaderAdapter::isCardPresent()) {
        return true;
    } else {
        /*
         * If the card is no longer present but one of the channels is still open, then the
         * card removal sequence is initiated.
         */
        if (isLogicalChannelOpen() || mObservableReaderSpi->isPhysicalChannelOpen()) {
            processCardRemoved();
        }

        return false;
    }
}

void ObservableLocalReaderAdapter::addObserver(std::shared_ptr<CardReaderObserverSpi> observer)
{
    checkStatus();
    mObservationManager->addObserver(observer);
}

void ObservableLocalReaderAdapter::removeObserver(std::shared_ptr<CardReaderObserverSpi> observer)
{
    mObservationManager->removeObserver(observer);
}

int ObservableLocalReaderAdapter::countObservers() const
{
    return mObservationManager->countObservers();
}

void ObservableLocalReaderAdapter::clearObservers()
{
    mObservationManager->clearObservers();
}

void ObservableLocalReaderAdapter::startCardDetection(const DetectionMode detectionMode)
{
    checkStatus();

    mLogger->debug("The reader '%' of plugin '%' is starting the card detection with polling mode" \
                   " '%'\n",
                   getName(),
                   getPluginName(),
                   detectionMode);

    mDetectionMode = detectionMode;
    mStateService->onEvent(InternalEvent::START_DETECT);
}

void ObservableLocalReaderAdapter::stopCardDetection()
{
    mLogger->debug("The reader '%' of plugin '%' is stopping the card detection\n",
                   getName(),
                   getPluginName());

    mStateService->onEvent(InternalEvent::STOP_DETECT);
}

void ObservableLocalReaderAdapter::finalizeCardProcessing()
{
    mLogger->debug("The reader '%' of plugin '%' is starting the removal sequence of the card\n",
                   getName(),
                   getPluginName());

    mStateService->onEvent(InternalEvent::CARD_PROCESSED);
}

// void ObservableLocalReaderAdapter::setEventNotificationExecutorService(
//     std::shared_ptr<ExecutorService> eventNotificationExecutorService)
// {
//     checkStatus();

//     mObservationManager->setEventNotificationExecutorService(eventNotificationExecutorService);
// }

void ObservableLocalReaderAdapter::setReaderObservationExceptionHandler(
    std::shared_ptr<CardReaderObservationExceptionHandlerSpi> exceptionHandler)
{
    checkStatus();
    mObservationManager->setObservationExceptionHandler(exceptionHandler);
}

void ObservableLocalReaderAdapter::onCardInserted()
{
    mStateService->onEvent(InternalEvent::CARD_INSERTED);
}

void ObservableLocalReaderAdapter::onCardRemoved()
{
    mStateService->onEvent(InternalEvent::CARD_REMOVED);
}

}
}
}
