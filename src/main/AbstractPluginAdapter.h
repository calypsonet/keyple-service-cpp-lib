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
#include <string>
#include <typeinfo>

/* Keyple Core Util */
#include "LoggerFactory.h"

/* Keyple Core Common */
#include "KeyplePluginExtension.h"

/* Keyple Core Service */
#include "LocalReaderAdapter.h"
#include "Plugin.h"
#include "Reader.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::common;
using namespace keyple::core::util::cpp;

/**
 * (package-private)<br>
 * Abstract class for all plugins.
 *
 * @since 2.0.0
 */
class AbstractPluginAdapter : virtual public Plugin {
public:
    /**
     * (package-private)<br>
     * Constructor.
     *
     * @param pluginName The name of the plugin.
     * @param pluginExtension The associated plugin extension SPI.
     * @since 2.0.0
     */
    AbstractPluginAdapter(const std::string& pluginName,
                          std::shared_ptr<KeyplePluginExtension> pluginExtension);

    /**
     * 
     */
    virtual ~AbstractPluginAdapter() = default;

    /**
     * (package-private)<br>
     * Builds a local reader adapter using the provided SPI.
     *
     * @param readerSpi The associated reader SPI.
     * @return A new instance.
     */
    std::shared_ptr<LocalReaderAdapter> buildLocalReaderAdapter(
        std::shared_ptr<ReaderSpi> readerSpi);

    /**
     * (package-private)<br>
     * Check if the plugin is registered.
     *
     * @throw IllegalStateException is thrown when plugin is not or no longer registered.
     * @since 2.0.0
     */
    virtual void checkStatus() const final;

    /**
     * (package-private)<br>
     * Changes the plugin status to registered.
     *
     * @throw PluginIOException If registration failed.
     * @since 2.0.0
     */
    virtual void doRegister();

    /**
     * (package-private)<br>
     * Unregisters the plugin and the readers present in its list.
     *
     * @since 2.0.0
     */
    virtual void doUnregister();

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    const std::string& getName() const final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    std::shared_ptr<KeyplePluginExtension> getExtension(const std::type_info& pluginExtensionClass) 
        const final;

    /**
     * (package-private)<br>
     * Gets the Map of all connected readers.
     *
     * @since 2.0.0
     */
    std::map<const std::string, std::shared_ptr<Reader>>& getReadersMap();

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    const std::vector<std::string> getReaderNames() const final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    const std::vector<std::shared_ptr<Reader>> getReaders() const final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    std::shared_ptr<Reader> getReader(const std::string& name) const final;

private:
    /**
     * 
     */
    const std::unique_ptr<Logger> mLogger = LoggerFactory::getLogger(typeid(AbstractPluginAdapter));
    
    /**
     *
     */
    const std::string mPluginName;

    /**
     *
     */
    std::shared_ptr<KeyplePluginExtension> mPluginExtension;

    /**
     *
     */
    bool mIsRegistered;

    /**
     *
     */
    std::map<const std::string, std::shared_ptr<Reader>> mReaders;
};

}
}
}
