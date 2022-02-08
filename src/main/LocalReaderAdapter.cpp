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

#include "LocalReaderAdapter.h"

#include <sstream>

/* Calypsonet Terminal Card */
#include "CardBrokenCommunicationException.h"
#include "ReaderBrokenCommunicationException.h"
#include "UnexpectedStatusWordException.h"

/* Calypsonet Terminal Reader */
#include "ReaderCommunicationException.h"
#include "ReaderProtocolNotSupportedException.h"

/* Keyple Core Util */
#include "ApduUtil.h"
#include "ByteArrayUtil.h"
#include "IllegalStateException.h"
#include "KeypleAssert.h"
#include "KeypleStd.h"
#include "System.h"

/* Keyple Core Plugin */
#include "AutonomousSelectionReaderSpi.h"
#include "CardIOException.h"
#include "ConfigurableReaderSpi.h"
#include "ReaderIOException.h"

/* Keyple Core Service */
#include "ApduRequestAdapter.h"
#include "ApduResponseAdapter.h"
#include "CardResponseAdapter.h"
#include "CardSelectionResponseAdapter.h"

namespace keyple {
namespace core {
namespace service {

using namespace calypsonet::terminal::card;
using namespace calypsonet::terminal::reader;
using namespace keyple::core::plugin;
using namespace keyple::core::plugin::spi::reader;
using namespace keyple::core::util;
using namespace keyple::core::util::cpp;
using namespace keyple::core::util::cpp::exception;

/* LOCAL READER ADAPTER ------------------------------------------------------------------------- */

const std::vector<uint8_t> LocalReaderAdapter::APDU_GET_RESPONSE = {0x00, 0xC0, 0x00, 0x00, 0x00};
const int LocalReaderAdapter::DEFAULT_SUCCESSFUL_CODE = 0x9000;

LocalReaderAdapter::LocalReaderAdapter(std::shared_ptr<ReaderSpi> readerSpi,
                                       const std::string& pluginName)
: AbstractReaderAdapter(readerSpi->getName(),
                        std::dynamic_pointer_cast<KeypleReaderExtension>(readerSpi),
                        pluginName),
  mReaderSpi(readerSpi),
  mBefore(0),
  mIsLogicalChannelOpen(false),
  mUseDefaultProtocol(false),
  mCurrentProtocol(""),
  mProtocolAssociations({}) {}

void LocalReaderAdapter::computeCurrentProtocol()
{
    mCurrentProtocol = "";

    if (mProtocolAssociations.size() == 0) {
        mUseDefaultProtocol = true;
    } else {
        mUseDefaultProtocol = false;

        const auto configurable = std::dynamic_pointer_cast<ConfigurableReaderSpi>(mReaderSpi);
        for (const auto& entry : mProtocolAssociations) {
            if (configurable->isCurrentProtocol(entry.first)) {
                mCurrentProtocol = entry.second;
            }
        }
    }
}

void LocalReaderAdapter::closeLogicalChannel()
{
    mLogger->trace("[%] closeLogicalChannel => Closing of the logical channel\n", getName());

    auto reader = std::dynamic_pointer_cast<AutonomousSelectionReaderSpi>(mReaderSpi);
    if (reader) {
        /* AutonomousSelectionReader have an explicit method for closing channels */
        reader->closeLogicalChannel();
    }

    mIsLogicalChannelOpen = false;
}

uint8_t LocalReaderAdapter::computeSelectApplicationP2(
    const FileOccurrence fileOccurrence, const FileControlInformation fileControlInformation)
{
    uint8_t p2;

    switch (fileOccurrence) {
    case FileOccurrence::FIRST:
        p2 = 0x00;
        break;
    case FileOccurrence::LAST:
        p2 = 0x01;
        break;
    case FileOccurrence::NEXT:
        p2 = 0x02;
        break;
    case FileOccurrence::PREVIOUS:
        p2 = 0x03;
        break;
    default:
        std::stringstream ss;
        ss << fileOccurrence;
        throw IllegalStateException("Unexpected value: " + ss.str());
    }

    switch (fileControlInformation) {
    case FileControlInformation::FCI:
        p2 |= 0x00;
        break;
    case FileControlInformation::FCP:
        p2 |= 0x04;
        break;
    case FileControlInformation::FMD:
        p2 |= 0x08;
        break;
    case FileControlInformation::NO_RESPONSE:
        p2 |= 0x0C;
        break;
    default:
        std::stringstream ss;
        ss << fileOccurrence;
        throw IllegalStateException("Unexpected value: " + ss.str());
    }

    return p2;
}

std::shared_ptr<ApduResponseAdapter> LocalReaderAdapter::processExplicitAidSelection(
    std::shared_ptr<CardSelectorSpi> cardSelector)
{
    const std::vector<uint8_t>& aid = cardSelector->getAid();

    mLogger->debug("[%] openLogicalChannel => Select Application with AID = %\n",
                   getName(),
                   ByteArrayUtil::toHex(aid));

    /*
     * Build a get response command the actual length expected by the card in the get response
     * command is handled in transmitApdu
     *
     * RL-SEL-CLA.1
     * RL-SEL-P2LC.1
     */
    std::vector<uint8_t> selectApplicationCommand(6 + aid.size());
    selectApplicationCommand[0] = 0x00; /* CLA */
    selectApplicationCommand[1] = 0xA4; /* INS */
    selectApplicationCommand[2] = 0x04; /* P1: select by name */
    /*
     * P2: b0,b1 define the File occurrence, b2,b3 define the File control information
     * we use the bitmask defined in the respective enums
     */
    selectApplicationCommand[3] = computeSelectApplicationP2(
                                      cardSelector->getFileOccurrence(),
                                      cardSelector->getFileControlInformation());
    selectApplicationCommand[4] = static_cast<uint8_t>(aid.size()); /* Lc */
    System::arraycopy(aid, 0, selectApplicationCommand, 5, static_cast<int>(aid.size()));
    selectApplicationCommand[5 + aid.size()] = 0x00; /* Le */

    auto apduRequest = std::make_shared<ApduRequestAdapter>(selectApplicationCommand);
    apduRequest->setInfo("Internal Select Application");

    return processApduRequest(apduRequest);
}

std::shared_ptr<ApduResponseAdapter> LocalReaderAdapter::selectByAid(
    std::shared_ptr<CardSelectorSpi> cardSelector)
{
    std::shared_ptr<ApduResponseAdapter> fciResponse = nullptr;

    auto reader = std::dynamic_pointer_cast<AutonomousSelectionReaderSpi>(mReaderSpi);
    if (reader) {
        const std::vector<uint8_t>& aid = cardSelector->getAid();
        const uint8_t p2 = computeSelectApplicationP2(cardSelector->getFileOccurrence(),
                                                      cardSelector->getFileControlInformation());
        const std::vector<uint8_t> selectionDataBytes = reader->openChannelForAid(aid, p2);
        fciResponse = std::make_shared<ApduResponseAdapter>(selectionDataBytes);
    } else {
        fciResponse = processExplicitAidSelection(cardSelector);
    }

    return fciResponse;
}

bool LocalReaderAdapter::checkPowerOnData(const std::string& powerOnData,
                                          std::shared_ptr<CardSelectorSpi> cardSelector)
{
    mLogger->debug("[%] openLogicalChannel => PowerOnData = %\n", getName(), powerOnData);

    const std::string& powerOnDataRegex = cardSelector->getPowerOnDataRegex();

    /* Check the power-on data */
    if (powerOnData != "" &&
        powerOnDataRegex != "" &&
        !std::regex_match(powerOnData, std::regex(powerOnDataRegex))) {
        mLogger->info("[%] openLogicalChannel => Power-on data didn't match. PowerOnData = %, " \
                      "regex filter = %\n",
                      getName(),
                      powerOnData,
                      cardSelector->getPowerOnDataRegex());

        /* The power-on data have been rejected */
        return false;
    } else {
        /* The power-on data have been accepted */
        return true;
    }
}

std::shared_ptr<LocalReaderAdapter::SelectionStatus> LocalReaderAdapter::processSelection(
    std::shared_ptr<CardSelectorSpi> cardSelector)
{
    /* RL-CLA-CHAAUTO.1 */
    std::string powerOnData = "";
    std::shared_ptr<ApduResponseAdapter> fciResponse = nullptr;
    bool hasMatched = true;

    if (cardSelector->getCardProtocol() != "" && mUseDefaultProtocol) {
        throw IllegalStateException("Protocol " +
                                    cardSelector->getCardProtocol() +
                                    " not associated to a reader protocol.");
    }

    /* Check protocol if enabled */
    if (cardSelector->getCardProtocol() == "" ||
        cardSelector->getCardProtocol() == mCurrentProtocol) {
        /*
         * Protocol check succeeded, check power-on data if enabled
         * RL-ATR-FILTER
         * RL-SEL-USAGE.1
         */
        powerOnData = mReaderSpi->getPowerOnData();
        if (checkPowerOnData(powerOnData, cardSelector)) {
            /* No power-on data filter or power-on data check succeeded, select by AID if enabled */
            if (cardSelector->getAid().size() != 0) {
                fciResponse = selectByAid(cardSelector);
                const std::vector<int>& statusWords =
                    cardSelector->getSuccessfulSelectionStatusWords();
                hasMatched = std::find(statusWords.begin(),
                                       statusWords.end(),
                                       fciResponse->getStatusWord()) != statusWords.end();
            } else {
                fciResponse = nullptr;
            }
        } else {
            /* Check failed */
            hasMatched = false;
            fciResponse = nullptr;
        }
    } else {
        /* Protocol failed */
        powerOnData = "";
        fciResponse = nullptr;
        hasMatched = false;
    }

    return std::make_shared<SelectionStatus>(powerOnData, fciResponse, hasMatched);
}

std::shared_ptr<CardSelectionResponseApi> LocalReaderAdapter::processCardSelectionRequest(
    std::shared_ptr<CardSelectionRequestSpi> cardSelectionRequest)
{
    mIsLogicalChannelOpen = false;
    std::shared_ptr<SelectionStatus> selectionStatus = nullptr;

    try {
        selectionStatus = processSelection(cardSelectionRequest->getCardSelector());
    } catch (const ReaderIOException& e) {
        throw ReaderBrokenCommunicationException(
                std::make_shared<CardResponseAdapter>(
                      std::vector<std::shared_ptr<ApduResponseApi>>({}), false),
                false,
                e.getMessage(),
                std::make_shared<ReaderIOException>(e));
    } catch (const CardIOException& e) {
        throw CardBrokenCommunicationException(
                  std::make_shared<CardResponseAdapter>(
                      std::vector<std::shared_ptr<ApduResponseApi>>({}), false),
                  false,
                  e.getMessage(),
                  std::make_shared<CardIOException>(e));
    }

    if (!selectionStatus->mHasMatched) {
        /* The selection failed, return an empty response having the selection status */
        return std::make_shared<CardSelectionResponseAdapter>(
                   selectionStatus->mPowerOnData,
                   selectionStatus->mSelectApplicationResponse,
                   false,
                   std::make_shared<CardResponseAdapter>(
                      std::vector<std::shared_ptr<ApduResponseApi>>({}), false));
    }

    mIsLogicalChannelOpen = true;

    std::shared_ptr<CardResponseAdapter> cardResponse = nullptr;

    if (cardSelectionRequest->getCardRequest() != nullptr) {
        cardResponse = processCardRequest(cardSelectionRequest->getCardRequest());
    } else {
        cardResponse = nullptr;
    }

    return std::make_shared<CardSelectionResponseAdapter>(
               selectionStatus->mPowerOnData,
               selectionStatus->mSelectApplicationResponse,
               true,
               cardResponse);
}

std::shared_ptr<ApduResponseAdapter> LocalReaderAdapter::case4HackGetResponse()
{
    uint64_t timeStamp = System::nanoTime();
    uint64_t elapsed10ms = (timeStamp - mBefore) / 100000;
    mBefore = timeStamp;

    mLogger->debug("[%] case4HackGetResponse => ApduRequest: NAME = \"Internal Get Response\", " \
                   "RAWDATA = %, elapsed = %\n",
                   getName(),
                   ByteArrayUtil::toHex(APDU_GET_RESPONSE),
                   elapsed10ms / 10.0);

    const std::vector<uint8_t> getResponseHackResponseBytes =
        mReaderSpi->transmitApdu(APDU_GET_RESPONSE);

    std::shared_ptr<ApduResponseAdapter> getResponseHackResponse =
        std::make_shared<ApduResponseAdapter>(getResponseHackResponseBytes);

    timeStamp = System::nanoTime();
    elapsed10ms = (timeStamp - mBefore) / 100000;
    mBefore = timeStamp;

    mLogger->debug("[%] case4HackGetResponse => Internal %, elapsed % ms\n",
                   getName(),
                   getResponseHackResponseBytes,
                   elapsed10ms / 10.0);

    return getResponseHackResponse;
}

std::shared_ptr<ApduResponseAdapter> LocalReaderAdapter::processApduRequest(
    const std::shared_ptr<ApduRequestSpi> apduRequest)
{
    std::shared_ptr<ApduResponseAdapter> apduResponse = nullptr;

    uint64_t timeStamp = System::nanoTime();
    uint64_t elapsed10ms = (timeStamp - mBefore) / 100000;
    mBefore = timeStamp;

    mLogger->debug("[%] processApduRequest => %, elapsed % ms\n",
                   getName(),
                   apduRequest,
                   elapsed10ms / 10.0);

    apduResponse = std::make_shared<ApduResponseAdapter>(
                       mReaderSpi->transmitApdu(apduRequest->getApdu()));

    /* RL-SW-ANALYSIS.1 */
    if (ApduUtil::isCase4(apduRequest->getApdu()) &&
        apduResponse->getDataOut().size() == 0 &&
        apduResponse->getStatusWord() == DEFAULT_SUCCESSFUL_CODE) {
        /* Do the get response command */
        apduResponse = case4HackGetResponse();
    }

    timeStamp = System::nanoTime();
    elapsed10ms = (timeStamp - mBefore) / 100000;
    mBefore = timeStamp;

    mLogger->debug("[%] processApduRequest => %, elapsed % ms\n",
                   getName(),
                   apduResponse,
                   elapsed10ms / 10.0);

    return apduResponse;
}

std::shared_ptr<CardResponseAdapter> LocalReaderAdapter::processCardRequest(
    const std::shared_ptr<CardRequestSpi> cardRequest)
{

    std::vector<std::shared_ptr<ApduResponseApi>> apduResponses;

    /* Proceeds with the APDU requests present in the CardRequest */
    for (const auto& apduRequest : cardRequest->getApduRequests()) {
        try {
            const auto apduResponse = processApduRequest(apduRequest);
            apduResponses.push_back(apduResponse);

            const std::vector<int>& successfulSW = apduRequest->getSuccessfulStatusWords();
            if (cardRequest->stopOnUnsuccessfulStatusWord() &&
                std::find(successfulSW.begin(),
                          successfulSW.end(),
                          apduResponse->getStatusWord()) ==
                    apduRequest->getSuccessfulStatusWords().end()) {
                throw UnexpectedStatusWordException(
                          std::make_shared<CardResponseAdapter>(apduResponses, false),
                          cardRequest->getApduRequests().size() == apduResponses.size(),
                          "Unexpected status word.");
            }
        } catch (const ReaderIOException& e) {
            /*
             * The process has been interrupted. We close the logical channel and launch a
             * KeypleReaderException with the Apdu responses collected so far.
             */
            closeLogicalAndPhysicalChannelsSilently();

            throw ReaderBrokenCommunicationException(
                      std::make_shared<CardResponseAdapter>(apduResponses, false),
                      false,
                      "Reader communication failure while transmitting a card request.",
                      std::make_shared<ReaderIOException>(e));
        } catch (const CardIOException& e) {
            /*
             * The process has been interrupted. We close the logical channel and launch a
             * KeypleReaderException with the Apdu responses collected so far.
             */
            closeLogicalAndPhysicalChannelsSilently();

            throw CardBrokenCommunicationException(
                      std::make_shared<CardResponseAdapter>(apduResponses, false),
                      false,
                      "Card communication failure while transmitting a card request.",
                      std::make_shared<CardIOException>(e));
        }
    }

    return std::make_shared<CardResponseAdapter>(apduResponses, mIsLogicalChannelOpen);
}

void LocalReaderAdapter::releaseChannel()
{
    checkStatus();

    try {
        mReaderSpi->closePhysicalChannel();
    } catch (const ReaderIOException& e) {
        throw ReaderBrokenCommunicationException(nullptr,
                                                 false,
                                                 "Failed to release the physical channel",
                                                 std::make_shared<ReaderIOException>(e));
    }
}

void LocalReaderAdapter::deactivateReaderProtocol(const std::string& readerProtocol)
{
    /* RL-CL-PROTOCOL.1 */
    checkStatus();
    Assert::getInstance().notEmpty(readerProtocol, "readerProtocol");

    mProtocolAssociations.erase(readerProtocol);

    const auto configurable = std::dynamic_pointer_cast<ConfigurableReaderSpi>(mReaderSpi);
    if (!configurable || !configurable->isProtocolSupported(readerProtocol)) {
        throw ReaderProtocolNotSupportedException(readerProtocol);
    }

    configurable->deactivateProtocol(readerProtocol);
}

void LocalReaderAdapter::activateReaderProtocol(const std::string& readerProtocol,
                                                const std::string& applicationProtocol)
{
    /* RL-CL-PROTOCOL.1 */
    checkStatus();
    Assert::getInstance().notEmpty(readerProtocol, "readerProtocol")
                         .notEmpty(applicationProtocol, "applicationProtocol");

    const auto configurable = std::dynamic_pointer_cast<ConfigurableReaderSpi>(mReaderSpi);
    if (!configurable || !configurable->isProtocolSupported(readerProtocol)) {
        throw ReaderProtocolNotSupportedException(readerProtocol);
    }

    configurable->activateProtocol(readerProtocol);

    mProtocolAssociations.insert({readerProtocol, applicationProtocol});
}

bool LocalReaderAdapter::isCardPresent()
{
    /*
     * RL-DET-PCRQ.1
     * RL-DET-PCAPDU.1
     */
    checkStatus();

    try {
        return mReaderSpi->checkCardPresence();
    } catch (const ReaderIOException& e) {
        throw ReaderCommunicationException(
                  "An exception occurred while checking the card presence.",
                  std::make_shared<ReaderIOException>(e));
    }
}

bool LocalReaderAdapter::isContactless()
{
    return mReaderSpi->isContactless();
}

std::shared_ptr<CardResponseApi> LocalReaderAdapter::processCardRequest(
    const std::shared_ptr<CardRequestSpi> cardRequest,
    const ChannelControl channelControl)
{
    checkStatus();

    std::shared_ptr<CardResponseAdapter> cardResponse = nullptr;

    /* Process the CardRequest and keep the CardResponse */
    cardResponse = processCardRequest(cardRequest);

    /* Close the channel if requested */
    if (channelControl == ChannelControl::CLOSE_AFTER) {
        releaseChannel();
    }

    return cardResponse;
}

std::vector<std::shared_ptr<CardSelectionResponseApi>>
    LocalReaderAdapter::processCardSelectionRequests(
        const std::vector<std::shared_ptr<CardSelectionRequestSpi>>& cardSelectionRequests,
        const MultiSelectionProcessing multiSelectionProcessing,
        const ChannelControl channelControl)
{
    checkStatus();

    std::vector<std::shared_ptr<CardSelectionResponseApi>> cardSelectionResponses;

    /* Open the physical channel if needed, determine the current protocol */
    if (!mReaderSpi->isPhysicalChannelOpen()) {
        try {
            mReaderSpi->openPhysicalChannel();
            computeCurrentProtocol();
        } catch (const ReaderIOException& e) {
            throw ReaderBrokenCommunicationException(
                      nullptr,
                      false,
                      "Reader communication failure while opening physical channel",
                      std::make_shared<ReaderIOException>(e));
        } catch (const CardIOException& e) {
            throw CardBrokenCommunicationException(
                      nullptr,
                      false,
                      "Card communication failure while opening physical channel",
                      std::make_shared<CardIOException>(e));
        }
    }

    /* Loop over all CardRequest provided in the list */
    for (const auto& cardSelectionRequest : cardSelectionRequests) {
        /* Process the CardRequest and append the CardResponse list */
        const auto cardSelectionResponse = processCardSelectionRequest(cardSelectionRequest);
        cardSelectionResponses.push_back(cardSelectionResponse);

        if (multiSelectionProcessing == MultiSelectionProcessing::PROCESS_ALL) {
            /*
             * Multi CardRequest case: just close the logical channel and go on with the next
             * selection.
             */
            closeLogicalChannel();
        } else {
            if (mIsLogicalChannelOpen) {
                /* The logical channel being open, we stop here */
                break; /* Exit for loop */
            }
        }
    }

    /* Close the channel if requested */
    if (channelControl == ChannelControl::CLOSE_AFTER) {
        releaseChannel();
    }

    return cardSelectionResponses;
}

void LocalReaderAdapter::doUnregister()
{
    try {
        mReaderSpi->onUnregister();
    } catch (const Exception& e) {
        mLogger->error("Error during the unregistration of the extension of reader '%' (%)\n",
                       getName(),
                       e);
    }

    AbstractReaderAdapter::doUnregister();
}

void LocalReaderAdapter::closeLogicalAndPhysicalChannelsSilently()
{
    closeLogicalChannel();

    /* Closes the physical channel and resets the current protocol info */
    mCurrentProtocol = "";
    mUseDefaultProtocol = false;

    try {
        mReaderSpi->closePhysicalChannel();
    } catch (const ReaderIOException& e) {
        mLogger->error("[%] Exception occurred in releaseSeChannel. Message: %\n",
                       getName(),
                       e.getMessage());
    }
}

bool LocalReaderAdapter::isLogicalChannelOpen() const
{
    return mIsLogicalChannelOpen;
}

std::shared_ptr<ReaderSpi> LocalReaderAdapter::getReaderSpi() const
{
    return mReaderSpi;
}

/* SELECTION STATUS ----------------------------------------------------------------------------- */

LocalReaderAdapter::SelectionStatus::SelectionStatus(
  const std::string& powerOnData,
  const std::shared_ptr<ApduResponseAdapter> selectApplicationResponse,
  const bool hasMatched)
: mPowerOnData(powerOnData),
  mSelectApplicationResponse(selectApplicationResponse),
  mHasMatched(hasMatched) {}

}
}
}
