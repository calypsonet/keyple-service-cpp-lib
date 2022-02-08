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

#include "CardSelectionResponseAdapter.h"

namespace keyple {
namespace core {
namespace service {

CardSelectionResponseAdapter::CardSelectionResponseAdapter(
  const std::string& powerOnData,
  const std::shared_ptr<ApduResponseAdapter> selectApplicationResponse,
  const bool hasMatched,
  const std::shared_ptr<CardResponseAdapter> cardResponse)
: mPowerOnData(powerOnData),
  mSelectApplicationResponse(selectApplicationResponse),
  mHasMatched(hasMatched),
  mCardResponse(cardResponse) {}

const std::string& CardSelectionResponseAdapter::getPowerOnData() const
{
    /* RL-ATR-IDENTIFY.1 */
    return mPowerOnData;
}

const std::shared_ptr<ApduResponseApi>
    CardSelectionResponseAdapter::getSelectApplicationResponse() const
{
    return mSelectApplicationResponse;
}

bool CardSelectionResponseAdapter::hasMatched() const
{
    return mHasMatched;
}

const std::shared_ptr<CardResponseApi> CardSelectionResponseAdapter::getCardResponse() const
{
    return mCardResponse;
}

std::ostream& operator<<(std::ostream& os, const CardSelectionResponseAdapter& csra)
{
    os << "CARD_SELECTION_RESPONSE_ADAPTER: {"
       << "POWER_ON_DATA = " << csra.mPowerOnData << ", "
       << "HAS_MATCHED = " << csra.mHasMatched << ", "
       << "SELECT_APPLICATION_RESPONSE = <TODO>, "
       << "CARD_RESPONSE = <TODO>"
       << "}";

    return os;
}

std::ostream& operator<<(std::ostream& os, const std::shared_ptr<CardSelectionResponseAdapter> csra)
{
    os << *csra.get();

    return os;
}

}
}
}
