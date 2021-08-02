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

#include "CardSelectionResultAdapter.h"

/* Keyple Core Util */
#include "IllegalStateException.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::util::cpp::exception;

CardSelectionResultAdapter::CardSelectionResultAdapter() : mActiveSelectionIndex(-1) {}

void CardSelectionResultAdapter::addSmartCard(const int selectionIndex,
                                              std::shared_ptr<SmartCard> smartCard)
{
    mSmartCardMap.insert({selectionIndex, smartCard});

    /* Keep the current selection index */
    mActiveSelectionIndex = selectionIndex;
}

const std::map<int, std::shared_ptr<SmartCard>>& CardSelectionResultAdapter::getSmartCards() const
{
    return mSmartCardMap;
}

const std::shared_ptr<SmartCard> CardSelectionResultAdapter::getActiveSmartCard() const
{
    const auto& it = mSmartCardMap.find(mActiveSelectionIndex);
    if (it == mSmartCardMap.end()) {
        throw IllegalStateException("No active matching card is available");
    }

    return it->second;
}

int CardSelectionResultAdapter::getActiveSelectionIndex() const
{
    return mActiveSelectionIndex;
}

}
}
}
