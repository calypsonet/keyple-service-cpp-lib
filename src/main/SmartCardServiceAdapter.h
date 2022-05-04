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
#include <memory>
#include <mutex>
#include <string>
#include <typeinfo>

/* Keyple Core Util */
#include "LoggerFactory.h"

/* Keyple Core Service */
#include "AbstractPluginAdapter.h"
#include "SmartCardService.h"

/* Keyple Core Plugin */
#include "PluginFactorySpi.h"
#include "PoolPluginFactorySpi.h"

/* Calypsonet Terminal Reader */
#include "CardSelectionManager.h"

namespace keyple {
namespace core {
namespace service {

using namespace calypsonet::terminal::reader::selection;
using namespace keyple::core::plugin::spi;
using namespace keyple::core::util::cpp;

/**
 * (package-private)<br>
 * Implementation of {@link SmartCardService}.
 *
 * @since 2.0.0
 */
class SmartCardServiceAdapter final : public SmartCardService {
public:
    /**
     * (package-private)<br>
     * Gets the single instance of SmartCardServiceAdapter.
     *
     * @return single instance of SmartCardServiceAdapter
     * @since 2.0.0
     */
    static std::shared_ptr<SmartCardServiceAdapter> getInstance();

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual std::shared_ptr<Plugin> registerPlugin(
        const std::shared_ptr<KeyplePluginExtensionFactory> pluginFactory)
        override final;


    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual void unregisterPlugin(const std::string& pluginName) override final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    const std::vector<std::string> getPluginNames() const final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    const std::vector<std::shared_ptr<Plugin>> getPlugins() const final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    std::shared_ptr<Plugin> getPlugin(const std::string& pluginName) const final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    void checkCardExtension(const std::shared_ptr<KeypleCardExtension> cardExtension) const final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    // @Override
    // public DistributedLocalService registerDistributedLocalService(
    //     KeypleDistributedLocalServiceExtensionFactory distributedLocalServiceExtensionFactory) {

    //     Assert.getInstance()
    //         .notNull(
    //             distributedLocalServiceExtensionFactory, "distributedLocalServiceExtensionFactory");

    //     DistributedLocalServiceAdapter distributedLocalService;
    //     try {
    //     if (!(distributedLocalServiceExtensionFactory instanceof LocalServiceFactorySpi)) {
    //         throw new IllegalArgumentException("The factory doesn't implement the right SPI.");
    //     }

    //     LocalServiceFactorySpi factory =
    //         (LocalServiceFactorySpi) distributedLocalServiceExtensionFactory;

    //     synchronized (distributedLocalServiceMonitor) {
    //         String localServiceName = factory.getLocalServiceName();

    //         checkDistributedLocalServiceRegistration(localServiceName);
    //         checkDistributedLocalServiceVersion(factory);

    //         LocalServiceSpi localServiceSpi = factory.getLocalService();

    //         if (!localServiceSpi.getName().equals(localServiceName)) {
    //         throw new IllegalArgumentException(
    //             String.format(
    //                 "The local service name '%s' mismatches the expected name '%s' provided by the factory",
    //                 localServiceSpi.getName(), localServiceName));
    //         }

    //         distributedLocalService = new DistributedLocalServiceAdapter(localServiceSpi);
    //         distributedLocalService.register();

    //         distributedLocalServices.put(localServiceName, distributedLocalService);
    //     }
    //     } catch (IllegalArgumentException e) {
    //     throw new IllegalArgumentException(
    //         "The provided distributed local service factory doesn't implement the distributed local service API properly.",
    //         e);
    //     }
    //     return distributedLocalService;
    // }

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    // @Override
    // public void unregisterDistributedLocalService(String distributedLocalServiceName) {
    //     logger.info(
    //         "Unregistering a distributed local service from the service : {}",
    //         distributedLocalServiceName);
    //     synchronized (distributedLocalServiceMonitor) {
    //     DistributedLocalService localService =
    //         distributedLocalServices.remove(distributedLocalServiceName);
    //     if (localService != null) {
    //         ((DistributedLocalServiceAdapter) localService).unregister();
    //     } else {
    //         logger.warn(
    //             "The distributed local service '{}' is not registered", distributedLocalServiceName);
    //     }
    //     }
    // }

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    // @Override
    // public boolean isDistributedLocalServiceRegistered(String distributedLocalServiceName) {
    //     synchronized (distributedLocalServiceMonitor) {
    //     return distributedLocalServices.containsKey(distributedLocalServiceName);
    //     }
    // }

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    // @Override
    // public DistributedLocalService getDistributedLocalService(String distributedLocalServiceName) {
    //     synchronized (distributedLocalServiceMonitor) {
    //     return distributedLocalServices.get(distributedLocalServiceName);
    //     }
    // }

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual std::unique_ptr<CardSelectionManager> createCardSelectionManager() override final;

private:
    /**
     *
     */
    const std::unique_ptr<Logger> mLogger =
        LoggerFactory::getLogger(typeid(SmartCardServiceAdapter));

    /**
     *
     */
    std::mutex mMutex;

    /**
     *
     */
    std::map<std::string, std::shared_ptr<Plugin>> mPlugins;

    /**
     *
     */
    static std::shared_ptr<SmartCardServiceAdapter> mInstance;

    /**
     *
     */
    // private final Object pluginMonitor = new Object();

    /**
     *
     */
    // std::map<std::string, std::shared_ptr<DistributedLocalService>> distributedLocalServices;

    /**
     *
     */
    // private final Object distributedLocalServiceMonitor = new Object();

    // static {
    //     // Register additional JSON adapters.
    //     JsonUtil.registerTypeAdapter(
    //         AbstractApduException.class, new ApduExceptionJsonSerializerAdapter(), true);
    // }

    /**
     * Private constructor
     */
    SmartCardServiceAdapter() {}

    /**
     * (private)<br>
     * Compare versions.
     *
     * @param providedVersion The provided version string.
     * @param localVersion The local version string.
     * @return 0 if providedVersion equals localVersion, &lt; 0 if providedVersion &lt; localVersion,
     *     &gt; 0 if providedVersion &gt; localVersion.
     */
    int compareVersions(const std::string& providedVersion, const std::string& localVersion) const;

    /**
     * (private)<br>
     * Checks for consistency in the versions of external APIs shared by the plugin and the service.
     *
     * <p>Generates warnings into the log.
     *
     * @param pluginFactorySpi The plugin factory SPI.
     */
    void checkPluginVersion(const std::shared_ptr<PluginFactorySpi> pluginFactorySpi);

    /**
     * (private)<br>
     * Checks for consistency in the versions of external APIs shared by the remote plugin and the
     * service.
     *
     * <p>Generates warnings into the log.
     *
     * @param remotePluginFactorySpi The remote plugin factory SPI.
     */
    // private void checkRemotePluginVersion(RemotePluginFactorySpi remotePluginFactorySpi) {
    //     if (compareVersions(remotePluginFactorySpi.getCommonsApiVersion(), CommonsApiProperties.VERSION)
    //         != 0) {
    //     logger.warn(
    //         "The version of Commons API used by the provided plugin ({}:{}) mismatches the version used by the service ({}).",
    //         remotePluginFactorySpi.getRemotePluginName(),
    //         remotePluginFactorySpi.getCommonsApiVersion(),
    //         CommonsApiProperties.VERSION);
    //     }
    //     if (compareVersions(
    //             remotePluginFactorySpi.getDistributedRemoteApiVersion(),
    //             DistributedRemoteApiProperties.VERSION)
    //         != 0) {
    //     logger.warn(
    //         "The version of Distributed Remote Plugin API used by the provided plugin ({}:{}) mismatches the version used by the service ({}).",
    //         remotePluginFactorySpi.getRemotePluginName(),
    //         remotePluginFactorySpi.getDistributedRemoteApiVersion(),
    //         DistributedRemoteApiProperties.VERSION);
    //     }
    // }

    /**
     * (private)<br>
     * Checks for consistency in the versions of external APIs shared by the pool plugin and the
     * service.
     *
     * <p>Generates warnings into the log.
     *
     * @param poolPluginFactorySpi The plugin factory SPI.
     */
    void checkPoolPluginVersion(const std::shared_ptr<PoolPluginFactorySpi> poolPluginFactorySpi);

    /**
     * (private)<br>
     * Checks for consistency in the versions of external APIs shared by the card extension and the
     * service.
     *
     * <p>Generates warnings into the log.
     *
     * @param cardExtension The card extension.
     */
    void checkCardExtensionVersion(const std::shared_ptr<KeypleCardExtension> cardExtension) const;

    /**
     * (private)<br>
     * Checks for consistency in the versions of external APIs shared by the distributed local service
     * and the service.
     *
     * <p>Generates warnings into the log.
     *
     * @param localServiceFactorySpi The distributed local service factory SPI.
     */
    // private void checkDistributedLocalServiceVersion(LocalServiceFactorySpi localServiceFactorySpi) {
    //     if (compareVersions(localServiceFactorySpi.getCommonsApiVersion(), CommonsApiProperties.VERSION)
    //         != 0) {
    //     logger.warn(
    //         "The version of Commons API used by the provided distributed local service ({}:{}) mismatches the version used by the service ({}).",
    //         localServiceFactorySpi.getLocalServiceName(),
    //         localServiceFactorySpi.getCommonsApiVersion(),
    //         CommonsApiProperties.VERSION);
    //     }
    //     if (compareVersions(
    //             localServiceFactorySpi.getDistributedLocalApiVersion(),
    //             DistributedLocalApiProperties.VERSION)
    //         != 0) {
    //     logger.warn(
    //         "The version of Distributed Local API used by the provided distributed local service ({}:{}) mismatches the version used by the service ({}).",
    //         localServiceFactorySpi.getLocalServiceName(),
    //         localServiceFactorySpi.getDistributedLocalApiVersion(),
    //         DistributedLocalApiProperties.VERSION);
    //     }
    // }

    /**
     * Checks if the plugin is already registered.
     *
     * @param pluginName The plugin name.
     * @throw IllegalStateException if the plugin is already registered.
     */
    void checkPluginRegistration(const std::string& pluginName);

    /**
     * Checks if the distributed local service is already registered.
     *
     * @param distributedLocalServiceName The distributed local service name.
     * @throw IllegalStateException if the distributed local service is already registered.
     */
    // private void checkDistributedLocalServiceRegistration(String distributedLocalServiceName) {
    //     logger.info(
    //         "Registering a new distributed local service to the service : {}",
    //         distributedLocalServiceName);
    //     Assert.getInstance().notEmpty(distributedLocalServiceName, "distributedLocalServiceName");
    //     if (isDistributedLocalServiceRegistered(distributedLocalServiceName)) {
    //     throw new IllegalStateException(
    //         String.format(
    //             "The distributed local service '%s' has already been registered to the service.",
    //             distributedLocalServiceName));
    //     }
    // }

    /**
     * (private)<br>
     * Creates an instance of local plugin.
     *
     * @param pluginFactorySpi The plugin factory SPI.
     * @return A not null reference.
     */
    std::shared_ptr<AbstractPluginAdapter> createLocalPlugin(
        std::shared_ptr<PluginFactorySpi> pluginFactorySpi);

    /**
     * (private)<br>
     * Creates an instance of local pool plugin.
     *
     * @param poolPluginFactorySpi The pool plugin factory SPI.
     * @return A not null reference.
     */
    std::shared_ptr<AbstractPluginAdapter> createLocalPoolPlugin(
        std::shared_ptr<PoolPluginFactorySpi> poolPluginFactorySpi);

    /**
     * (private)<br>
     * Creates an instance of remote plugin.
     *
     * @param remotePluginFactorySpi The plugin factory SPI.
     * @return A not null reference.
     */
    // private AbstractPluginAdapter createRemotePlugin(RemotePluginFactorySpi remotePluginFactorySpi) {

    //     checkPluginRegistration(remotePluginFactorySpi.getRemotePluginName());
    //     checkRemotePluginVersion(remotePluginFactorySpi);

    //     AbstractRemotePluginSpi remotePluginSpi = remotePluginFactorySpi.getRemotePlugin();

    //     if (!remotePluginSpi.getName().equals(remotePluginFactorySpi.getRemotePluginName())) {
    //     throw new IllegalArgumentException(
    //         String.format(
    //             "The remote plugin name '%s' mismatches the expected name '%s' provided by the factory",
    //             remotePluginSpi.getName(), remotePluginFactorySpi.getRemotePluginName()));
    //     }

    //     AbstractPluginAdapter plugin;
    //     if (remotePluginSpi instanceof RemotePoolPluginSpi) {
    //     plugin = new RemotePoolPluginAdapter((RemotePoolPluginSpi) remotePluginSpi);
    //     } else if (remotePluginSpi instanceof ObservableRemotePluginSpi) {
    //     plugin = new ObservableRemotePluginAdapter((ObservableRemotePluginSpi) remotePluginSpi);
    //     } else {
    //     plugin = new RemotePluginAdapter((RemotePluginSpi) remotePluginSpi);
    //     }
    //     return plugin;
    // }

};

}
}
}
