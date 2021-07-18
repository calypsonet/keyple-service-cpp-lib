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

#include <ostream>

namespace keyple {
namespace core {
namespace service {

/**
 * (package-private)<br>
 * The states that the reader monitoring state machine can have
 *
 * @since 2.0
 */
enum class MonitoringState {
    /**
     * The reader is idle and waiting for a start signal to enter the card detection mode.
     *
     * @since 2.0
     */
    WAIT_FOR_START_DETECTION,

    /**
     * The reader is in card detection mode and is waiting for a card to be presented.
     *
     * @since 2.0
     */
    WAIT_FOR_CARD_INSERTION,

    /**
     * The reader waits for the application to finish processing the card.
     *
     * @since 2.0
     */
    WAIT_FOR_CARD_PROCESSING,

    /**
     * The reader waits for the removal of the card.
     *
     * @since 2.0
     */
    WAIT_FOR_CARD_REMOVAL
};

std::ostream& operator<<(std::ostream& os, const MonitoringState ms)
{
    switch (ms) {
    case MonitoringState::WAIT_FOR_START_DETECTION:
        os << "MONITORING_STATE = WAIT_FOR_START_DETECTION";
        break;
    case MonitoringState::WAIT_FOR_CARD_PROCESSING:
        os << "MONITORING_STATE = WAIT_FOR_CARD_PROCESSING";
        break;
    case MonitoringState::WAIT_FOR_CARD_REMOVAL:
        os << "MONITORING_STATE = WAIT_FOR_CARD_REMOVAL";
        break;
    case MonitoringState::WAIT_FOR_CARD_INSERTION:
        os << "MONITORING_STATE = WAIT_FOR_CARD_INSERTION";
        break;
    }

    return os;
}

}
}
}
