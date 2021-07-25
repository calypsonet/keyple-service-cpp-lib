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

#include "MonitoringState.h"

namespace keyple {
namespace core {
namespace service {

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
