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
#include <string>

/* Calypsonet Terminal Reader */
#include "CardReaderEvent.h"
#include "ScheduledCardSelectionsResponse.h"

/* Keyple Core Service */
#include "ReaderEvent.h"

namespace keyple {
namespace core {
namespace service {

using namespace calypsonet::terminal::reader;
using namespace calypsonet::terminal::reader::selection;

using Type = CardReaderEvent::Type;

/**
 * (package-private)<br>
 * Implementation of {@link ReaderEvent}.
 *
 * @since 2.0.0
 */
class ReaderEventAdapter final : public ReaderEvent {
public:
    /**
     * ReaderEvent constructor for simple insertion notification mode
     *
     * @param pluginName The name of the current plugin (should be not null).
     * @param readerName The name of the current reader (should be not null).
     * @param type The type of event (should be not null).
     * @param scheduledCardSelectionsResponse The responses received during the execution of the card
     *     selection scenario (can be null).
     * @since 2.0.0
     */
    ReaderEventAdapter(
        const std::string& pluginName,
        const std::string& readerName,
        const Type type,
        std::shared_ptr<ScheduledCardSelectionsResponse> scheduledCardSelectionsResponse);

    /**
     *
     */
    virtual const std::string& getPluginName() const override;

    /**
     *
     */
    virtual const std::string& getReaderName() const override;

    /**
     *
     */
    virtual Type getType() const override;

    /**
     *
     */
    virtual const std::shared_ptr<ScheduledCardSelectionsResponse> getScheduledCardSelectionsResponse()
        const override;

private:
    /**
     *
     */
    const std::string mPluginName;

    /**
     *
     */
    const std::string mReaderName;

    /**
     *
     */
    const std::shared_ptr<ScheduledCardSelectionsResponse> mScheduledCardSelectionsResponse;

    /**
     *
     */
    const Type mType;
};

}
}
}
