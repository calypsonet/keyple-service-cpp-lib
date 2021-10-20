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

/* Keyple Core Service */
#include "AbstractPluginAdapter.h"

/* Keyple Core Plugin */
#include "PluginSpi.h"

/* Keyple Core Util */
#include "LoggerFactory.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::plugin::spi;

/**
 * (package-private)<br>
 * Implementation of a local {@link Plugin}.
 *
 * @since 2.0.0
 */
class LocalPluginAdapter : public AbstractPluginAdapter {
public:    
    /**
     * (package-private)<br>
     * Constructor.
     *
     * @param pluginSpi The associated SPI.
     * @since 2.0.0
     */
    LocalPluginAdapter(std::shared_ptr<PluginSpi> pluginSpi);

    /**
     * 
     */
    virtual ~LocalPluginAdapter() = default;

    /**
     * {@inheritDoc}
     *
     * <p>Populates its list of available readers and registers each of them.
     *
     * @since 2.0.0
     */
    virtual void doRegister() override final;

    /**
     * {@inheritDoc}
     *
     * <p>Unregisters the associated SPI.
     *
     * @since 2.0.0
     */
    virtual void doUnregister() override;

private:
    /**
     * 
     */
    const std::unique_ptr<Logger> mLogger = LoggerFactory::getLogger(typeid(LocalPluginAdapter));
    
    /**
     *
     */
    std::shared_ptr<PluginSpi> mPluginSpi;
};

}
}
}
