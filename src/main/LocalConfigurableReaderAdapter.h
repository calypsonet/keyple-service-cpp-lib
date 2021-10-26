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
#include "ConfigurableReader.h"
#include "LocalReaderAdapter.h"

/* Keyple Core Plugin */
#include "ConfigurableReaderSpi.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::plugin::spi::reader;

/**
 * (package-private)<br>
 * Local configurable reader adapter.
 *
 * @since 2.0.0
 */
class LocalConfigurableReaderAdapter final
: public LocalReaderAdapter, public ConfigurableReader {
public:
    /**
     * (package-private)<br>
     * Constructor.
     *
     * @param configurableReaderSpi The configurable reader SPI.
     * @param pluginName The name of the plugin.
     * @since 2.0.0
     */
    LocalConfigurableReaderAdapter(std::shared_ptr<ConfigurableReaderSpi> configurableReaderSpi, 
                                   const std::string& pluginName);

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void activateProtocol(const std::string& readerProtocol, const std::string& applicationProtocol)
        override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void deactivateProtocol(const std::string& readerProtocol) override;
};

}
}
}
