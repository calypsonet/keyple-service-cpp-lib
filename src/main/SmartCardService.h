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
#include <vector>

/* Calypsonet Terminal Reader */
#include "CardSelectionManager.h"

/* Keyple Core Service */
#include "KeypleServiceExport.h"
#include "Plugin.h"

/* Keyple Core Commons */
#include "KeypleCardExtension.h"
#include "KeyplePluginExtensionFactory.h"

namespace keyple {
namespace core {
namespace service {

using namespace calypsonet::terminal::reader::selection;
using namespace keyple::core::common;

/**
 * Keyple main service.
 *
 * @since 2.0.0
 */
class KEYPLESERVICE_API SmartCardService {
public:
    /**
     *
     */
    virtual ~SmartCardService() = default;

    /**
     * Registers a new plugin to the service.
     *
     * @param pluginFactory The plugin factory.
     * @return A not null reference to the registered {@link Plugin}.
     * @throws KeyplePluginException If instantiation failed.
     * @throws IllegalStateException If the plugin has already been registered.
     * @since 2.0.0
     */
    virtual std::shared_ptr<Plugin> registerPlugin(
        std::shared_ptr<KeyplePluginExtensionFactory> pluginFactory) = 0;

    /**
     * Attempts to unregister the plugin having the provided name from the service.
     *
     * @param pluginName The plugin name.
     * @since 2.0.0
     */
    virtual void unregisterPlugin(const std::string& pluginName) = 0;

    /**
     * Gets the names of all registered plugins.
     *
     * @return A not null Set String.
     * @since 2.0.0
     */
    virtual const std::vector<std::string> getPluginNames() const = 0;

    /**
     * Gets all registered plugins.
     *
     * @return A not null Set of Plugin.
     * @since 2.0.0
     */
    virtual const  std::vector<std::shared_ptr<Plugin>> getPlugins() const = 0;

    /**
     * Gets the plugin whose name is provided as an argument.
     *
     * @param pluginName The plugin name.
     * @return Null if the plugin is not found or no longer registered.
     * @since 2.0.0
     */
    virtual std::shared_ptr<Plugin> getPlugin(const std::string& pluginName) const = 0;

    /**
     * Verifies the compatibility with the service of the provided card extension.
     *
     * <p>The verification is based on the comparison of the respective API versions.
     *
     * @param cardExtension A not null {@link KeypleCardExtension} reference object
     * @since 2.0.0
     */
    virtual void checkCardExtension(const std::shared_ptr<KeypleCardExtension> cardExtension) const
        = 0;

    /**
     * Registers a new distributed local service to the service.
     *
     * @param distributedLocalServiceExtensionFactory Factory to use to instantiate a Distributed
     *     Local Service extension
     * @return A not null reference to the registered {@link DistributedLocalService}.
     * @throw IllegalStateException If the distributed local service has already been registered.
     * @since 2.0.0
     */
    //DistributedLocalService registerDistributedLocalService(
    //    KeypleDistributedLocalServiceExtensionFactory distributedLocalServiceExtensionFactory);

    /**
     * Attempts to unregister the distributed local service having the provided name from the service.
     *
     * @param distributedLocalServiceName The distributed local service name.
     * @since 2.0.0
     */
    //void unregisterDistributedLocalService(String distributedLocalServiceName);

    /**
     * Checks whether a distributed local service is already registered to the service or not.
     *
     * @param distributedLocalServiceName The name of the distributed local service to be checked.
     * @return True if the distributed local service is registered.
     * @since 2.0.0
     */
    //boolean isDistributedLocalServiceRegistered(String distributedLocalServiceName);

    /**
     * Gets the distributed local service having the provided name.
     *
     * @param distributedLocalServiceName The name of the distributed local service.
     * @return Null if the distributed local service is not found or no longer registered.
     * @since 2.0.0
     */
    //DistributedLocalService getDistributedLocalService(String distributedLocalServiceName);

    /**
     * Create a new instance of a CardSelectionManager in order to perform a card selection.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    virtual std::unique_ptr<CardSelectionManager> createCardSelectionManager() = 0;
};

}
}
}
