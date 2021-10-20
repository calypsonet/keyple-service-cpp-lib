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

#include <memory>
#include <vector>

/* Calypsonet Terminal Card */
#include "CardSelectionResponseApi.h"

/* Calypsonet Terminal Reader */
#include "ScheduledCardSelectionsResponse.h"

namespace keyple {
namespace core {
namespace service {

using namespace calypsonet::terminal::card;
using namespace calypsonet::terminal::reader::selection;

/**
 * (package-private)<br>
 * POJO containing the card selection responses received during the card selection process.
 *
 * @since 2.0.0
 */
class ScheduledCardSelectionsResponseAdapter : public ScheduledCardSelectionsResponse {
public:
    /**
     * (package-private)<br>
     * Constructor
     *
     * @param cardSelectionResponses The card selection responses.
     * @since 2.0.0
     */
    ScheduledCardSelectionsResponseAdapter(
        const std::vector<std::shared_ptr<CardSelectionResponseApi>>& cardSelectionResponses);

    /**
     * (package-private)<br>
     * Gets the card responses.
     *
     * @return A list of {@link CardSelectionResponseApi}.
     * @since 2.0.0
     */
    const std::vector<std::shared_ptr<CardSelectionResponseApi>>& getCardSelectionResponses() const;

private:
    /**
     *
     */
    const std::vector<std::shared_ptr<CardSelectionResponseApi>> mCardSelectionResponses;
};

}
}
}
