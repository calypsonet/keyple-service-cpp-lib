/**************************************************************************************************
 * Copyright (c) 2020 Calypso Networks Association https://www.calypsonet-asso.org/               *
 *                                                                                                *
 * See the NOTICE file(s) distributed with this work for additional information regarding         *
 * copyright ownership.                                                                           *
 *                                                                                                *
 * This program and the accompanying materials are made available under the terms of the Eclipse  *
 * Public License 2.0 which is available at http://www.eclipse.org/legal/epl-2.0                  *
 *                                                                                                *
 * SPDX-License-Identifier: EPL-2.0                                                               *
 **************************************************************************************************/

#include "CardResponseAdapter.h"

namespace keyple {
namespace core {
namespace service {

CardResponseAdapter::CardResponseAdapter(
  const std::vector<std::shared_ptr<ApduResponseApi>> apduResponses,
  const bool isLogicalChannelOpen)
: mApduResponses(apduResponses), mIsLogicalChannelOpen(isLogicalChannelOpen) {}

const std::vector<std::shared_ptr<ApduResponseApi>>& CardResponseAdapter::getApduResponses() 
    const
{
	return mApduResponses;
}

bool CardResponseAdapter::isLogicalChannelOpen() const
{
    return mIsLogicalChannelOpen;
}

std::ostream& operator<<(std::ostream& os, const CardResponseAdapter& cra)
{
    os << "CARD_RESPONSE_ADAPTER: {"
       << "APDU_RESPONSE = <TODO>, "
       << "IS_LOGICAL_CHANNEL_OPEN = " << cra.mIsLogicalChannelOpen
       << "}";

    return os;
}

std::ostream& operator<<(std::ostream& os, const std::shared_ptr<CardResponseAdapter> cra)
{
    os << *cra.get();

    return os;
}

}
}
}
