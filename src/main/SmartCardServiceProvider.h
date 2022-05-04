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

#include "SmartCardService.h"

 /* Keyple Core Service */
#include "KeypleServiceExport.h"

namespace keyple {
namespace core {
namespace service {

/**
 * Provider of the SmartCardService.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API SmartCardServiceProvider final {
public:
    /**
     * Gets the unique instance of SmartCardService.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    static std::shared_ptr<SmartCardService> getService();

private:
    /**
     * Private constructor
     */
    SmartCardServiceProvider();

};

}
}
}
