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

/* Calypsonet Terminal Reader */
#include "CardReaderEvent.h"
#include "ScheduledCardSelectionsResponse.h"

namespace keyple {
namespace core {
namespace service {

using namespace calypsonet::terminal::reader;
using namespace calypsonet::terminal::reader::selection;

using Type = CardReaderEvent::Type;

/**
 * All information about a change of card state within an ObservableReader.
 *
 * <p>In the case of a card insertion, the responses received by the reader are included in the
 * event.
 *
 * @since 2.0.0
 */
class ReaderEvent : public CardReaderEvent {
public:
    /**
     * Gets the name of the plugin from which the reader that generated the event comes from.
     *
     * @return A not empty string.
     * @since 2.0.0
     */
    virtual const std::string& getPluginName() const = 0;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual const std::string& getReaderName() const override = 0;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual Type getType() const override = 0;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual const std::shared_ptr<ScheduledCardSelectionsResponse>
        getScheduledCardSelectionsResponse() const override = 0;
};

}
}
}
