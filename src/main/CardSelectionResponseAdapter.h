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
#include <ostream>
#include <string>

/* Calypsonet Terminal Card */
#include "ApduResponseApi.h"
#include "CardResponseApi.h"
#include "CardSelectionResponseApi.h"

namespace keyple {
namespace core {
namespace service {

using namespace calypsonet::terminal::card;

/**
 * (package-private)<br>
 * This POJO contains the data from a card obtained in response to a card selection request.
 *
 * <p>These data are the selection status and the responses, if any, to the additional APDUs sent to
 * the card CardResponseApi).
 *
 * @see calypsonet::terminal::card::spi::CardSelectionRequestSpi
 * @since 2.0
 */
class CardSelectionResponseAdapter final : public CardSelectionResponseApi {
public:
    /**
     * (package-private)<br>
     * Builds a card selection response including the selection status and a CardResponseApi
     * (list of calypsonet::terminal::card::ApduResponseApi).
     *
     * @param powerOnData The card power-on data, null if the power-on data is not available.
     * @param selectApplicationResponse The response to the Select Application command, null if no
     *     Select Application command was performed...
     * @param hasMatched True if the card inserted matches the selection filters.
     * @param cardResponse null if no card response is available.
     * @since 2.0
     */
    CardSelectionResponseAdapter(const std::string& powerOnData,
                                 const std::shared_ptr<ApduResponseApi> selectApplicationResponse,
                                 const bool hasMatched,
                                 const std::shared_ptr<CardResponseApi> cardResponse);

    /**
     * {@inheritDoc}
     *
     * @since 2.0
     */
    virtual const std::string& getPowerOnData() const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0
     */
    virtual const std::shared_ptr<ApduResponseApi> getSelectApplicationResponse() const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0
     */
    virtual bool hasMatched() const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0
     */
    virtual const std::shared_ptr<CardResponseApi> getCardResponse() const override;

    /**
     *
     */
    friend std::ostream& operator<<(std::ostream& os, const CardSelectionResponseAdapter& csra);

    /**
     *
     */
    friend std::ostream& operator<<(std::ostream& os,
                                    const std::shared_ptr<CardSelectionResponseAdapter> csra);

private:
    /**
     *
     */
    const std::string mPowerOnData;

    /**
     *
     */
    const std::shared_ptr<ApduResponseApi> mSelectApplicationResponse;

    /**
     *
     */
    const bool mHasMatched;

    /**
     *
     */
    const std::shared_ptr<CardResponseApi> mCardResponse;
};

}
}
}
