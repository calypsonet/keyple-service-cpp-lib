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

#include "CardSelectionScenarioAdapter.h"

/* Calypsonet Terminal Card */
#include "CardSelectionRequestSpi.h"

/* Keyple Core Util */
#include "KeypleAssert.h"

namespace keyple {
namespace core {
namespace service {

using namespace calypsonet::terminal::card::spi;
using namespace keyple::core::util;
using namespace keyple::core::util::cpp;

CardSelectionScenarioAdapter::CardSelectionScenarioAdapter(
  const std::vector<std::shared_ptr<CardSelectionRequestSpi>>& cardSelectionRequests,
  const MultiSelectionProcessing multiSelectionProcessing,
  const ChannelControl channelControl)
: mCardSelectionRequests(cardSelectionRequests),
  mMultiSelectionProcessing(multiSelectionProcessing),
  mChannelControl(channelControl)
{
    Assert::getInstance().notEmpty(cardSelectionRequests, "cardSelectionRequests");
}

const std::vector<std::shared_ptr<CardSelectionRequestSpi>>&
    CardSelectionScenarioAdapter::getCardSelectionRequests() const
{
    return mCardSelectionRequests;
}

MultiSelectionProcessing CardSelectionScenarioAdapter::getMultiSelectionProcessing() const
{
    return mMultiSelectionProcessing;
}

ChannelControl CardSelectionScenarioAdapter::getChannelControl() const
{
    return mChannelControl;
}

std::ostream& operator<<(std::ostream& os,
                         const std::shared_ptr<CardSelectionScenarioAdapter> sa)
{
    (void)sa;

    os << "CARD_SELECTION_SCENARIO : {"
       << "<TODO> CardSelectionRequest, "
       << "<TODO> MultiSelectionProcessing, "
       << "<TODO> ChannelControl"
       << "}";

    return os;
}

}
}
}
