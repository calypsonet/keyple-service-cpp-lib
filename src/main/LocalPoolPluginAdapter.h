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
#include <typeinfo>

/* Keyple Core Util */
#include "LoggerFactory.h"

/* Keyple Core Service */
#include "AbstractPluginAdapter.h"
#include "PoolPlugin.h"

/* Keyple Core Plugin */
#include "PoolPluginSpi.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::plugin::spi;
using namespace keyple::core::util::cpp;

/**
 * (package-private)<br>
 * Implementation of a local {@link PoolPlugin}.
 *
 * @since 2.0.0
 */
class LocalPoolPluginAdapter final : public AbstractPluginAdapter, public PoolPlugin {
public:
    /**
     * (package-private)<br>
     * Constructor.
     *
     * @param poolPluginSpi The associated SPI.
     * @since 2.0.0
     */
    LocalPoolPluginAdapter(std::shared_ptr<PoolPluginSpi> poolPluginSpi);

    /**
     * {@inheritDoc}
     *
     * <p>Unregisters the associated SPI.
     *
     * @since 2.0.0
     */
    void doUnregister() final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    const std::vector<std::string> getReaderGroupReferences() const final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    std::shared_ptr<Reader> allocateReader(const std::string& readerGroupReference) final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void releaseReader(const std::shared_ptr<Reader> reader) final;

private:
    /**
     *
     */
    const std::unique_ptr<Logger> mLogger =
        LoggerFactory::getLogger(typeid(LocalPoolPluginAdapter));

    /**
     *
     */
    std::shared_ptr<PoolPluginSpi> mPoolPluginSpi;
};

}
}
}
