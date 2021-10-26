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

/* Keyple Core Service */
#include "Reader.h"

/* Calypsnet Terminal Reader */
#include "ConfigurableCardReader.h"

namespace keyple {
namespace core {
namespace service {

using namespace calypsonet::terminal::reader;

/**
 * Drives the underlying hardware to configure the protocol to use.
 *
 * @since 2.0.0
 */
class ConfigurableReader : virtual public Reader, virtual public ConfigurableCardReader {
public:
    /**
     * 
     */
    virtual ~ConfigurableReader() = default;
};

}
}
}
