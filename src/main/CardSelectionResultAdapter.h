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

#include <map>
#include <memory>

/* Calypsonet Terminal Reader */
#include "CardSelectionResult.h"
#include "SmartCard.h"

namespace keyple {
namespace core {
namespace service {

using namespace calypsonet::terminal::reader::selection;
using namespace calypsonet::terminal::reader::selection::spi;

/**
 * (package-private)<br>
 * Implementation of {@link CardSelectionResult}.
 *
 * @since 2.0.0
 */
class CardSelectionResultAdapter final : public CardSelectionResult {
public:
    /**
     * (package-private)<br>
     * Constructor
     *
     * @since 2.0.0
     */
    CardSelectionResultAdapter();

    /**
     * (package-private)<br>
     * Append a {@link SmartCard} to the internal list
     *
     * @param selectionIndex The index of the selection that resulted in the smart card.
     * @param smartCard The smart card.
     * @since 2.0.0
     */
    void addSmartCard(const int selectionIndex, std::shared_ptr<SmartCard> smartCard);

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual const std::map<int, std::shared_ptr<SmartCard>>& getSmartCards() const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual const std::shared_ptr<SmartCard> getActiveSmartCard() const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual int getActiveSelectionIndex() const override;

private:
    /**
     *
     */
    int mActiveSelectionIndex;

    /**
     *
     */
    std::map<int, std::shared_ptr<SmartCard>> mSmartCardMap;
};

}
}
}
