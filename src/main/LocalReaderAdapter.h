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
#include <map>
#include <memory>
#include <string>
#include <typeinfo>
#include <vector>

/* Keyple Core Plugin */
#include "ReaderSpi.h"

/* Keyple Core Service */
#include "AbstractReaderAdapter.h"
#include "ApduResponseAdapter.h"
#include "CardResponseAdapter.h"

/* Keyple Core Util */
#include "LoggerFactory.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::plugin::spi::reader;
using namespace keyple::core::util::cpp;

using FileControlInformation = CardSelectorSpi::FileControlInformation;
using FileOccurrence = CardSelectorSpi::FileOccurrence;

/**
 * (package-private)<br>
 * Local reader adapter.
 *
 * <ul>
 *   <li>RL-CMD-USED.1
 *   <li>RL-CLA-ACCEPTED.1
 *   <li>RL-PERF-TIME.1
 * </ul>
 *
 * @since 2.0.0
 */
class LocalReaderAdapter : public AbstractReaderAdapter {
public:
    /**
     * (package-private)<br>
     * Constructor.
     *
     * @param readerSpi The reader SPI.
     * @param pluginName The name of the plugin.
     * @since 2.0.0
     */
    LocalReaderAdapter(std::shared_ptr<ReaderSpi> readerSpi, const std::string& pluginName);

    /**
     *
     */
    virtual ~LocalReaderAdapter() = default;

    /**
     * (package-private)<br>
     * Gets ReaderSpi associated to this reader.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    std::shared_ptr<ReaderSpi> getReaderSpi() const;

    /**
     * (package-private)<br>
     * Gets the logical channel's opening state.
     *
     * @return True if the channel is open, false if not.
     * @since 2.0.0
     */
    virtual bool isLogicalChannelOpen() const final;

    /**
     * (package-private)<br>
     * Close both logical and physical channels
     *
     * <p>This method doesn't raise any exception.
     *
     * @since 2.0.0
     */
    virtual void closeLogicalAndPhysicalChannelsSilently() final;

    /**
     * {@inheritDoc}
     *
     * <p>Invoke  ReaderSpi#onUnregister() on the associated SPI.
     *
     * @since 2.0.0
     */
    virtual void doUnregister() override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual std::vector<std::shared_ptr<CardSelectionResponseApi>> processCardSelectionRequests(
        const std::vector<std::shared_ptr<CardSelectionRequestSpi>>& cardSelectionRequests,
        const MultiSelectionProcessing multiSelectionProcessing,
        const ChannelControl channelControl) override final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual std::shared_ptr<CardResponseApi> processCardRequest(
        const std::shared_ptr<CardRequestSpi> cardRequest,
        const ChannelControl channelControl) override final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual bool isContactless() override final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual bool isCardPresent() override;

    /**
     * (package-private)<br>
     * Activates a protocol (for configurable reader only).
     *
     * @param readerProtocol The reader protocol.
     * @param applicationProtocol The corresponding application protocol to associate.
     *
     * @since 2.0.0
     */
    void activateReaderProtocol(const std::string& readerProtocol,
                                const std::string& applicationProtocol);

    /**
     * (package-private)<br>
     * Deactivates a protocol (for configurable reader only).
     *
     * @param readerProtocol The reader protocol.
     *
     * @since 2.0.0
     */
    void deactivateReaderProtocol(const std::string& readerProtocol);

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual void releaseChannel() override final;

private:
    /**
     *
     */
    const std::unique_ptr<Logger> mLogger = LoggerFactory::getLogger(typeid(LocalReaderAdapter));

    /**
     * Predefined ISO values
     */
    static const std::vector<uint8_t> APDU_GET_RESPONSE;
    static const int DEFAULT_SUCCESSFUL_CODE;

    /**
     *
     */
    std::shared_ptr<ReaderSpi> mReaderSpi;

    /**
     *
     */
    uint64_t mBefore;

    /**
     *
     */
    bool mIsLogicalChannelOpen;

    /**
     *
     */
    bool mUseDefaultProtocol;

    /**
     *
     */
    std::string mCurrentProtocol;

    /**
     *
     */
    std::map<const std::string, const std::string> mProtocolAssociations;

    /**
     * (private)<br>
     * This POJO contains the card selection status.
     */
    class SelectionStatus {
    public:
        friend LocalReaderAdapter;

        /**
         * Constructor.
         *
         * @param powerOnData A String containing the power-on data (optional).
         * @param selectApplicationResponse The response to the select application command
         *                                  (optional).
         * @param hasMatched A boolean.
         */
        SelectionStatus(const std::string& powerOnData,
                        const std::shared_ptr<ApduResponseAdapter> selectApplicationResponse,
                        const bool hasMatched);

    private:
        /**
         *
         */
        const std::string mPowerOnData;

        /**
         *
         */
        const std::shared_ptr<ApduResponseAdapter> mSelectApplicationResponse;

        /**
         *
         */
        const bool mHasMatched;
    };

    /**
     * (private)<br>
     * Determines the current protocol used by the card.
     *
     * <p>The Map mProtocolAssociations containing the protocol names (reader and application)
     * is iterated and the reader protocol (key of the Map) is checked with the reader.<br>
     *
     * <p>If the Map is not empty:
     * <li>The boolean mUseDefaultProtocol is set to false.
     * <li>If the test provided by the reader SPI is positive (the protocol presented is the one
     *     used by the current card) then the field mCurrentProtocol is set with the name of the
     *     protocol known to the application.
     * <li>If none of the protocols present in the Map matches then the mCurrentProtocol is
     *     set to null.
     * </ul>
     *
     * <p>If the Map is empty, no other check is done, the String field mCurrentProtocol is
     * set to null and the boolean field mUseDefaultProtocol is set to true.
     */
    void computeCurrentProtocol();

    /**
     * (private)<br>
     * Close the logical channel.
     */
    void closeLogicalChannel();

    /**
     * (private)<br>
     * Computes the P2 parameter of the ISO7816-4 Select Application APDU command from the provided
     * FileOccurrence and FileControlInformation.
     *
     * @param fileOccurrence The file's position relative to the current file.
     * @param fileControlInformation The file control information output.
     * @throw IllegalStateException If one of the provided argument is unexpected.
     */
    uint8_t computeSelectApplicationP2(const FileOccurrence fileOccurrence,
                                       const FileControlInformation fileControlInformation);

    /**
     * (private)<br>
     * Sends the select application command to the card and returns the requested data according to
     * AidSelector attributes (ISO7816-4 selection data) into an {@link ApduResponseAdapter}.
     *
     * @param cardSelector The card selector.
     * @return A not null {@link ApduResponseAdapter}.
     * @throw ReaderIOException if the communication with the reader has failed.
     * @throw CardIOException if the communication with the card has failed.
     */
    std::shared_ptr<ApduResponseAdapter> processExplicitAidSelection(
        std::shared_ptr<CardSelectorSpi> cardSelector);

    /**
     * (private)<br>
     * Selects the card with the provided AID and gets the FCI response in return.
     *
     * @param cardSelector The card selector.
     * @return A not null ApduResponseAdapter containing the FCI.
     * @see processSelection(CardSelectorSpi)
     */
    std::shared_ptr<ApduResponseAdapter> selectByAid(std::shared_ptr<CardSelectorSpi> cardSelector);

    /**
     * (private)<br>
     * Checks the provided power-on data with the PowerOnDataFilter.
     *
     * <p>Returns true if the power-on data is accepted by the filter.
     *
     * @param powerOnData A String containing the power-on data.
     * @param cardSelector The card selector.
     * @return True or false.
     * @throw IllegalStateException if no power-on data is available and the PowerOnDataFilter is
     *        set.
     * @see processSelection(CardSelectorSpi)
     */
    bool checkPowerOnData(const std::string& powerOnData,
                          std::shared_ptr<CardSelectorSpi> cardSelector);

    /**
     * (private)<br>
     * Select the card according to the {@link CardSelectorSpi}.
     *
     * <p>The selection status is returned.<br>
     * 3 levels of filtering/selection are applied successively if they are enabled: protocol, power
     * on data and AID.<br>
     * As soon as one of these operations fails, the method returns with a failed selection status.
     *
     * <p>Conversely, the selection is considered successful if none of the filters have rejected the
     * card, even if none of the filters are active.
     *
     * @param cardSelector A not null {@link CardSelectorSpi}.
     * @return A not null {@link SelectionStatus}.
     * @throw ReaderIOException if the communication with the reader has failed.
     * @throw CardIOException if the communication with the card has failed.
     */
    std::shared_ptr<SelectionStatus> processSelection(
        std::shared_ptr<CardSelectorSpi> cardSelector);

    /**
     * (private)<br>
     * Attempts to select the card and executes the optional requests if any.
     *
     * @param cardSelectionRequest The CardSelectionRequestSpi to be processed.
     * @return A not null reference.
     * @throw ReaderBrokenCommunicationException If the communication with the reader has failed.
     * @throw CardBrokenCommunicationException If the communication with the card has failed.
     * @throw UnexpectedStatusWordException If status word verification is enabled in the card
     *        request and the card returned an unexpected code.
     */
    std::shared_ptr<CardSelectionResponseApi> processCardSelectionRequest(
        std::shared_ptr<CardSelectionRequestSpi> cardSelectionRequest);

    /**
     * (private)<br>
     * Process dedicated to some cards not following the ISO standard for case 4 management.
     *
     * <p>Execute an explicit get response command in order to get the outgoing data from specific
     * cards answering 9000 with no data although the command has outgoing data.
     *
     * @return ApduResponse the response to the get response command
     * @throw ReaderIOException if the communication with the reader has failed.
     * @throw CardIOException if the communication with the card has failed.
     */
    std::shared_ptr<ApduResponseAdapter> case4HackGetResponse();

    /**
     * (private)<br>
     * Transmits an ApduRequestSpi and receives the ApduResponseAdapter.
     *
     * <p>The time measurement is carried out and logged with the detailed information of the
     * exchanges (TRACE level).
     *
     * @param apduRequest The APDU request to transmit.
     * @return A not null reference.
     * @throw ReaderIOException if the communication with the reader has failed.
     * @throw CardIOException if the communication with the card has failed.
     */
    std::shared_ptr<ApduResponseAdapter> processApduRequest(
        const std::shared_ptr<ApduRequestSpi> apduRequest);

    /**
     * (private)<br>
     * Transmits a CardRequestSpi and returns a CardResponseAdapter.
     *
     * @param cardRequest The card request to transmit.
     * @return A not null reference.
     * @throw ReaderBrokenCommunicationException If the communication with the reader has failed.
     * @throw CardBrokenCommunicationException If the communication with the card has failed.
     * @throw UnexpectedStatusWordException If status word verification is enabled in the card
     *        request and the card returned an unexpected code.
     */
    std::shared_ptr<CardResponseAdapter> processCardRequest(
        const std::shared_ptr<CardRequestSpi> cardRequest);
};

}
}
}
