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

#include "AbstractReaderAdapter.h"

/* Calypsonet Terminal Card */
#include "UnexpectedStatusWordException.h"

namespace keyple {
namespace core {
namespace service {

using namespace calypsonet::terminal::card;

AbstractReaderAdapter::AbstractReaderAdapter(const std::string& readerName,
                                             std::shared_ptr<KeypleReaderExtention> readerExtension,
                                             const std::string& pluginName)
: mReaderName(readerName), mReaderExtension(readerExtension), mPluginName(pluginName) {}

const std::string& AbstractReaderAdapter::getPluginName() const
{
    return mPluginName;
}

const std::vector<std::shared_ptr<CardSelectionResponseApi>>
    AbstractReaderAdapter::transmitCardSelectionRequests(
        const std::vector<CardSelectionRequestSpi>& cardSelectionRequests,
        const MultiSelectionProcessing multiSelectionProcessing,
        const ChannelControl channelControl)
{
    checkStatus();

    std::vector<std::shared_ptr<CardSelectionResponseApi>> cardSelectionResponses = nullptr;

    long timeStamp = System::nanoTime();
    long elapsed10ms = (timeStamp - mBefore) / 100000;
    mBefore = timeStamp;

    mLogger->debug("[%] transmit => %, elapsed % ms\n",
                   getName(),
                   cardSelectionRequests,
                   elapsed10ms / 10.0);

    try {
        cardSelectionResponses = processCardSelectionRequests(
                                     cardSelectionRequests,
                                     multiSelectionProcessing,
                                     channelControl);
    } catch (const UnexpectedStatusWordException& e) {
        throw CardBrokenCommunicationException(e.getCardResponse(),
                                               false,
                                               "An unexpected status word was received.",
                                               e);
    }

    timeStamp = System::nanoTime();
    elapsed10ms = (timeStamp - mBefore) / 100000;
    mBefore = timeStamp;

    mLogger->debug("[%] received => %, elapsed % ms\n",
                   getName(),
                   cardSelectionResponses,
                   elapsed10ms / 10.0);


    return cardSelectionResponses;
}

void AbstractReaderAdapter::checkStatus() const
{
    if (!mIsRegistered) {
        throw IllegalStateException("This reader, " + getName() + " is not registered");
    }
}

void AbstractReaderAdapter::register()
{
    mIsRegistered = true;
}

void AbstractReaderAdapter::unregister()
{
    mIsRegistered = false;
}

const std::string& AbstractReaderAdapter::getName() const
{
    return mReaderName;
}

std::shared_ptr<KeypleReaderExtension> AbstractReaderAdapter::getExtension(
    const std::type_info& readerExtensionClass) const
{
    checkStatus();

    return mReaderExtension;
}

std::shared_ptr<CardResponseApi> AbstractReaderAdapter::transmitCardRequest(
    const std::shared_ptr<CardRequestSpi> cardRequest,
    const ChannelControl channelControl)
{
    checkStatus();

    Assert::getInstance().notNull(cardRequest, "cardRequest")
                         .notNull(channelControl, "channelControl");

    std::shared_ptr<CardResponseApi> cardResponse = nullptr;

    long timeStamp = System::nanoTime();
    long elapsed10ms = (timeStamp - mBefore) / 100000;
    mBefore = timeStamp;

    mLogger->debug("[%] transmit => %, elapsed % ms\n", getName(), cardRequest, elapsed10ms / 10.0);

    try {
        cardResponse = processCardRequest(cardRequest, channelControl);
    } catch (const Exception& e) {
    }

    timeStamp = System::nanoTime();
    elapsed10ms = (timeStamp - mBefore) / 100000;
    mBefore = timeStamp;

    mLogger->debug("[%] receive => %, elapsed % ms\n", getName(), cardResponse, elapsed10ms / 10.0);

    return cardResponse;
}

}
}
}