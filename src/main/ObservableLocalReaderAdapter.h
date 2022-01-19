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

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

/* Calypsonet Terminal Reader */
#include "CardReaderObservationExceptionHandlerSpi.h"
#include "CardReaderObserverSpi.h"
#include "ObservableCardReader.h"

/* Keyple Core Plugin */
#include "ObservableReaderSpi.h"
#include "WaitForCardInsertionAutonomousReaderApi.h"
#include "WaitForCardRemovalAutonomousReaderApi.h"

/* Keyple Core Service */
#include "CardSelectionScenarioAdapter.h"
#include "Job.h"
#include "LocalReaderAdapter.h"
#include "MonitoringState.h"
#include "ObservationManagerAdapter.h"
#include "ObservableReader.h"
#include "ReaderEvent.h"

/* Keyple Core Util */
#include "LoggerFactory.h"

namespace keyple {
namespace core {
namespace service {

using namespace calypsonet::terminal::reader;
using namespace calypsonet::terminal::reader::spi;
using namespace keyple::core::plugin;
using namespace keyple::core::plugin::spi::reader::observable;
using namespace keyple::core::util::cpp;

class AbstractObservableStateAdapter;
class ObservableReaderStateServiceAdapter;

using DetectionMode = ObservableCardReader::DetectionMode;
using NotificationMode = ObservableCardReader::NotificationMode;

/**
 * (package-private)<br>
 * Implementation for ObservableReader, WaitForCardInsertionAutonomousReaderApi and
 * WaitForCardRemovalAutonomousReaderApi.
 *
 * @since 2.0.0
 */
class ObservableLocalReaderAdapter
: public LocalReaderAdapter,
  public ObservableReader,
  public WaitForCardInsertionAutonomousReaderApi,
  public WaitForCardRemovalAutonomousReaderApi,
  public std::enable_shared_from_this<ObservableLocalReaderAdapter> {
public:
    /**
     *
     */
    static const std::string READER_MONITORING_ERROR;

    /**
     * (package-private)<br>
     * The events that drive the card's observation state machine.
     *
     * @since 2.0.0
     */
    enum InternalEvent {
        /**
         * A card has been inserted
         *
         * @since 2.0.0
         */
        CARD_INSERTED,

        /**
         * The card has been removed
         *
         * @since 2.0.0
         */
        CARD_REMOVED,

        /**
         * The application has completed the processing of the card
         *
         * @since 2.0.0
         */
        CARD_PROCESSED,

        /**
         * The application has requested the start of card detection
         *
         * @since 2.0.0
         */
        START_DETECT,

        /**
         * The application has requested that card detection is to be stopped.
         *
         * @since 2.0.0
         */
        STOP_DETECT,

        /**
         * A timeout has occurred (not yet implemented)
         *
         * @since 2.0.0
         */
        TIME_OUT
    };

    /**
     * (package-private)<br>
     * Creates an instance of ObservableLocalReaderAdapter.
     *
     * <p>Creates the ObservableReaderStateServiceAdapter with the possible states and their
     * implementation.
     *
     * @param observableReaderSpi The reader SPI.
     * @param pluginName The plugin name.
     * @since 2.0.0
     */
    ObservableLocalReaderAdapter(std::shared_ptr<ObservableReaderSpi> observableReaderSpi,
                                 const std::string& pluginName);

    /**
     * (package-private)<br>
     * Gets the SPI of the reader.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    std::shared_ptr<ObservableReaderSpi> getObservableReaderSpi() const;

    /**
     * (package-private)<br>
     * Gets the exception handler used to notify the application of exceptions raised during the
     * observation process.
     *
     * @return Null if no exception has been set.
     * @since 2.0.0
     */
    std::shared_ptr<CardReaderObservationExceptionHandlerSpi> getObservationExceptionHandler()
        const;

    /**
     * (package-private)<br>
     * Gets the current calypsonet::terminal::reader::ObservableCardReader::DetectionMode.
     *
     * @return Null if the polling mode has not been defined.
     * @since 2.0.0
     */
    DetectionMode getDetectionMode() const;

    /**
     * (package-private)<br>
     * Get the current monitoring state
     *
     * @return current getMonitoringState
     * @since 2.0.0
     */
    MonitoringState getCurrentMonitoringState() const;

    /**
     * (package-private)<br>
     * Sends a neutral APDU to the card to check its presence. The status of the response is not
     * verified as long as the mere fact that the card responds is sufficient to indicate whether or
     * not it is present.
     *
     * <p>This method has to be called regularly until the card no longer respond.
     *
     * @return True if the card still responds, false if not
     * @since 2.0.0
     */
    bool isCardPresentPing();

    /**
     * (package-private)<br>
     * This method is invoked by the card insertion monitoring process when a card is inserted.
     *
     * <p>It will return a ReaderEvent or null:
     *
     * <ul>
     *   <li>CARD_INSERTED: if no card selection scenario was defined.
     *   <li>CARD_MATCHED: if a card selection scenario was defined in any mode and a card matched the
     *       selection.
     *   <li>CARD_INSERTED: if a card selection scenario was defined in ALWAYS mode but no card
     *       matched the selection (the DefaultSelectionsResponse is however transmitted).
     * </ul>
     *
     * <p>It returns null if a card selection scenario is defined in MATCHED_ONLY mode but no card
     * matched the selection.
     *
     * <p>The selection data and the responses to the optional requests that may be present in the
     * card selection scenario are embedded into the ReaderEvent as a list of
     * CardSelectionResponseApi.
     *
     * @return Null if the card has been rejected by the card selection scenario.
     * @since 2.0.0
     */
    std::shared_ptr<ReaderEvent> processCardInserted();

    /**
     * (package-private)<br>
     * This method is invoked when a card is removed to notify the application of the
     * CardReaderEvent::Type::CARD_REMOVED event.
     *
     * <p>It will also be invoked if isCardPresent() is called and at least one of the
     * physical or logical channels is still open.
     *
     * @since 2.0.0
     */
    void processCardRemoved();

    /**
     * (package-private)<br>
     * Changes the state of the state machine
     *
     * @param stateId new stateId
     * @since 2.0.0
     */
    void switchState(MonitoringState stateId);

    /**
     * (package-private)<br>
     * Notifies all registered observers with the provided ReaderEvent.
     *
     * <p>This method never throws an exception. Any errors at runtime are notified to the
     * application using the exception handler.
     *
     * @param event The reader event.
     * @since 2.0.0
     */
    void notifyObservers(const std::shared_ptr<ReaderEvent> event);

    /**
     * (package-private)<br>
     * If defined, the prepared {@link CardSelectionScenarioAdapter} will be processed as soon as a
     * card is inserted. The result of this request set will be added to the reader event notified to
     * the application.
     *
     * <p>If it is not defined (set to null), a simple card detection will be notified in the end.
     *
     * <p>Depending on the notification policy, the observer will be notified whenever a card is
     * inserted, regardless of the selection status, or only if the current card matches the selection
     * criteria.
     *
     * @param cardSelectionScenario The card selection scenario.
     * @param notificationMode The notification policy.
     * @param detectionMode The polling policy (optional).
     * @since 2.0.0
     */
    void scheduleCardSelectionScenario(
        std::shared_ptr<CardSelectionScenarioAdapter> cardSelectionScenario,
        const NotificationMode notificationMode,
        const DetectionMode detectionMode);

    /**
     * {@inheritDoc}
     *
     * <p>Notifies all observers of the UNAVAILABLE event.<br>
     * Stops the card detection unconditionally.<br>
     * Shuts down the reader's executor service.
     *
     * @since 2.0.0
     */
    void doUnregister() override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    bool isCardPresent() override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void addObserver(std::shared_ptr<CardReaderObserverSpi> observer) override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void removeObserver(std::shared_ptr<CardReaderObserverSpi> observer) override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    int countObservers() const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void clearObservers() override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void startCardDetection(const DetectionMode detectionMode) override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void stopCardDetection() override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void finalizeCardProcessing() override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void setReaderObservationExceptionHandler(
        std::shared_ptr<CardReaderObservationExceptionHandlerSpi> exceptionHandler) override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void onCardInserted() override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void onCardRemoved() override;

private:
    /**
     *
     */
    class ObservableLocalReaderAdapterJob final : public Job {
    public:
        /**
         *
         */
        ObservableLocalReaderAdapterJob(std::shared_ptr<CardReaderObserverSpi> observer,
                                        const std::shared_ptr<ReaderEvent> event,
                                        ObservableLocalReaderAdapter* parent);

        /**
         * C++: this replaces run() override
         */
        void execute() final;

    private:
        /**
         *
         */
        std::shared_ptr<CardReaderObserverSpi> mObserver;

        /**
         *
         */
        const std::shared_ptr<ReaderEvent> mEvent;

        /**
         *
         */
        ObservableLocalReaderAdapter* mParent;
    };

    /**
     *
     */
    const std::shared_ptr<Logger> mLogger =
        LoggerFactory::getLogger(typeid(ObservableLocalReaderAdapter));

    /**
     *
     */
    static const std::vector<uint8_t> APDU_PING_CARD_PRESENCE;

    /**
     *
     */
    std::shared_ptr<ObservableReaderSpi> mObservableReaderSpi;

    /**
     *
     */
    std::shared_ptr<ObservableReaderStateServiceAdapter> mStateService;

    /**
     *
     */
    std::shared_ptr<ObservationManagerAdapter<CardReaderObserverSpi,
                                              CardReaderObservationExceptionHandlerSpi>>
        mObservationManager;

    /**
     *
     */
    std::shared_ptr<CardSelectionScenarioAdapter> mCardSelectionScenario;

    /**
     *
     */
    NotificationMode mNotificationMode;

    /**
     *
     */
    DetectionMode mDetectionMode;

    /**
     * Notifies a single observer of an event.
     *
     * @param observer The observer to notify.
     * @param event The event.
     */
    void notifyObserver(std::shared_ptr<CardReaderObserverSpi> observer,
                        const std::shared_ptr<ReaderEvent> event);

    /**
     * Check if a card has matched.
     *
     * @param cardSelectionResponses The responses received.
     * @return True if a card has matched, false if not.
     */
    bool hasACardMatched(
        const std::vector<std::shared_ptr<CardSelectionResponseApi>>& cardSelectionResponses);
};

}
}
}
