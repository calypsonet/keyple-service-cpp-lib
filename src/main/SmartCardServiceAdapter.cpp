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

#include "SmartCardServiceAdapter.h"

/* Keyple Core Util */
#include "IllegalArgumentException.h"
#include "IllegalStateException.h"
#include "KeypleAssert.h"
#include "KeypleStd.h"
#include "NumberFormatException.h"

/* Kepyle Core Commons */
#include "CommonsApiProperties.h"

/* Keyple Core Plugin */
#include "ObservablePluginSpi.h"
#include "PluginApiProperties.h"
#include "PluginIOException.h"
#include "PoolPluginFactorySpi.h"

/* Keyple Core Service */
#include "AutonomousObservableLocalPluginAdapter.h"
#include "AbstractPluginAdapter.h"
#include "CardSelectionManagerAdapter.h"
#include "KeyplePluginException.h"
#include "LocalPoolPluginAdapter.h"
#include "ObservableLocalPluginAdapter.h"

/* Calypsonet Terminal Reader */
#include "ReaderApiProperties.h"

/* Calypsonet Terminal Card */
#include "CardApiProperties.h"


namespace keyple {
namespace core {
namespace service {

using namespace calypsonet::terminal::reader;
using namespace keyple::core::commons;
using namespace keyple::core::plugin;
using namespace keyple::core::plugin::spi;
using namespace keyple::core::util;
using namespace keyple::core::util::cpp::exception;

SmartCardServiceAdapter& SmartCardServiceAdapter::getInstance()
{
    static SmartCardServiceAdapter instance;

    return instance;
}

int SmartCardServiceAdapter::compareVersions(const std::string& providedVersion,
                                             const std::string& localVersion)
{
    const std::regex re("[.]");

    std::vector<std::string> providedVersions;
    std::split(providedVersion, re, std::back_inserter(providedVersions));
    
    std::vector<std::string> localVersions;
    std::split(localVersion, re, std::back_inserter(localVersions));

    if (providedVersions.size() != localVersions.size()) {
        throw IllegalStateException("Inconsistent version numbers: provided = " +
                                    providedVersion +
                                    ", local = " +
                                    localVersion);
        }

        int provided = 0;
        int local = 0;

        try {
            for (const auto& v : providedVersions) {
                provided += std::stoi(v);
                provided *= 1000;
            }
            for (const auto& v : localVersions) {
                local += std::stoi(v);
                local *= 1000;
            }

        /* Replaced NumberFormatException by std::invalid_argument */
        } catch (const std::invalid_argument& e) {
            throw IllegalStateException("Bad version numbers: provided = " +
                                        providedVersion +
                                        ", local = " +
                                        localVersion,
                                        nullptr);
        }

        /* Java uses compareTo() which returns 0 when equal */
        return provided != local;
}

void SmartCardServiceAdapter::checkPluginVersion(
    const std::shared_ptr<PluginFactorySpi> pluginFactorySpi)
{
    if (compareVersions(pluginFactorySpi->getCommonsApiVersion(),
                        CommonsApiProperties::VERSION) != 0) {
        mLogger->warn("The version of Commons API used by the provided plugin (%:%) mismatches " \
                      "the version used by the service (%)\n",
                      pluginFactorySpi->getPluginName(),
                      pluginFactorySpi->getCommonsApiVersion(),
                      CommonsApiProperties::VERSION);
    }

    if (compareVersions(pluginFactorySpi->getPluginApiVersion(),
                        PluginApiProperties::VERSION) != 0) {
        mLogger->warn("The version of Plugin API used by the provided plugin (%:%) mismatches " \
                      "the version used by the service (%)\n",
                      pluginFactorySpi->getPluginName(),
                      pluginFactorySpi->getPluginApiVersion(),
                      PluginApiProperties::VERSION);
    }
}

std::shared_ptr<Plugin> SmartCardServiceAdapter::registerPlugin(
    const std::shared_ptr<KeyplePluginExtensionFactory> pluginFactory)
{
    Assert::getInstance().notNull(pluginFactory, "pluginFactory");

    std::shared_ptr<AbstractPluginAdapter> plugin = nullptr;

    const std::lock_guard<std::mutex> lock(mMutex);

    try {
            const auto factorySpi = std::dynamic_pointer_cast<PluginFactorySpi>(pluginFactory);
            const auto poolSpi = std::dynamic_pointer_cast<PoolPluginFactorySpi>(pluginFactory);
            if (factorySpi) {
                plugin = createLocalPlugin(factorySpi);
            } else if (poolSpi) {
                plugin = createLocalPoolPlugin(poolSpi);
            // } else if (pluginFactory instanceof RemotePluginFactorySpi) {
            // plugin = createRemotePlugin((RemotePluginFactorySpi) pluginFactory);
            } else {
                throw IllegalArgumentException("The factory doesn't implement the right SPI.");
            }

            plugin->doRegister();
            mPlugins.insert({plugin->getName(), plugin});

    } catch (const IllegalArgumentException& e) {
        throw IllegalArgumentException("The provided plugin factory doesn't implement the plugin" \
                                       " API properly.",
                                       std::make_shared<IllegalArgumentException>(e));

    } catch (const PluginIOException& e) {
        throw KeyplePluginException("Unable to register the plugin '" +
                                    plugin->getName() +
                                    "' : " +
                                    e.getMessage(),
                                    std::make_shared<PluginIOException>(e));
    }

    return plugin;
}

void SmartCardServiceAdapter::unregisterPlugin(const std::string& pluginName)
{
    mLogger->info("Unregistering a plugin from the service : %\n", pluginName);

    const std::lock_guard<std::mutex> lock(mMutex);

    auto i = mPlugins.find(pluginName);
    if (i != mPlugins.end()) {
        std::shared_ptr<Plugin> removedPlugin = std::move(i->second);
        mPlugins.erase(i);
        std::dynamic_pointer_cast<AbstractPluginAdapter>(removedPlugin)->doUnregister();
    } else {
        mLogger->warn("The plugin '%' is not registered\n", pluginName);
    }
}

const std::vector<std::string> SmartCardServiceAdapter::getPluginNames()
{
    const std::lock_guard<std::mutex> lock(mMutex);

    std::vector<std::string> pluginNames;
    for (const auto& pair : mPlugins) {
        pluginNames.push_back(pair.first);
    }

    return pluginNames;
}

const std::vector<std::shared_ptr<Plugin>> SmartCardServiceAdapter::getPlugins()
{
    const std::lock_guard<std::mutex> lock(mMutex);

    std::vector<std::shared_ptr<Plugin>> plugins;
    for (const auto& pair : mPlugins) {
        plugins.push_back(pair.second);
    }

    return plugins;
}

std::shared_ptr<Plugin> SmartCardServiceAdapter::getPlugin(const std::string& pluginName)
{
    const std::lock_guard<std::mutex> lock(mMutex);

    return mPlugins.at(pluginName);
}

void SmartCardServiceAdapter::checkCardExtension(
    const std::shared_ptr<KeypleCardExtension> cardExtension)
{
    checkCardExtensionVersion(cardExtension);
}

std::unique_ptr<CardSelectionManager> SmartCardServiceAdapter::createCardSelectionManager()
{
    return std::unique_ptr<CardSelectionManager>(new CardSelectionManagerAdapter());
}

void SmartCardServiceAdapter::checkPoolPluginVersion(
    const std::shared_ptr<PoolPluginFactorySpi> poolPluginFactorySpi)
{
    if (compareVersions(poolPluginFactorySpi->getCommonsApiVersion(),
                        CommonsApiProperties::VERSION) != 0) {
        mLogger->warn("The version of Commons API used by the provided pool plugin (%:%) " \
                      "mismatches the version used by the service (%)\n",
                      poolPluginFactorySpi->getPoolPluginName(),
                      poolPluginFactorySpi->getCommonsApiVersion(),
                      CommonsApiProperties::VERSION);
    }

    if (compareVersions(poolPluginFactorySpi->getPluginApiVersion(),
                        PluginApiProperties::VERSION) != 0) {
        mLogger->warn("The version of Plugin API used by the provided pool plugin (%:%) " \
                      "mismatches the version used by the service (%)\n",
                      poolPluginFactorySpi->getPoolPluginName(),
                      poolPluginFactorySpi->getPluginApiVersion(),
                      PluginApiProperties::VERSION);
    }
}

void SmartCardServiceAdapter::checkCardExtensionVersion(
    const std::shared_ptr<KeypleCardExtension> cardExtension)
{
    if (compareVersions(cardExtension->getCommonsApiVersion(), CommonsApiProperties::VERSION) != 0) {
        mLogger->warn("The version of Commons API used by the provided card extension (%) " \
                      "mismatches the version used by the service (%)\n",
                      cardExtension->getCommonsApiVersion(),
                      CommonsApiProperties::VERSION);
    }

    if (compareVersions(cardExtension->getCardApiVersion(), CardApiProperties::VERSION) != 0) {
        mLogger->warn("The version of Card API used by the provided card extension (%) " \
                      "mismatches the version used by the service (%)\n",
                      cardExtension->getCardApiVersion(),
                      CardApiProperties::VERSION);
    }

    if (compareVersions(cardExtension->getReaderApiVersion(), ReaderApiProperties::VERSION) != 0) {
        mLogger->warn("The version of Service API used by the provided card extension (%) " \
                      "mismatches the version used by the service (%)\n",
                      cardExtension->getReaderApiVersion(),
                      ReaderApiProperties::VERSION);
    }
}

void SmartCardServiceAdapter::checkPluginRegistration(const std::string& pluginName)
{
    mLogger->info("Registering a new Plugin to the service : %\n", pluginName);

    const auto it = mPlugins.find(pluginName);
    if (it != mPlugins.end()) {
        throw IllegalStateException("The plugin '" +
                                    pluginName +
                                    "' has already been registered to the service.");
        }
}

std::unique_ptr<AbstractPluginAdapter> SmartCardServiceAdapter::createLocalPlugin(
        std::shared_ptr<PluginFactorySpi> pluginFactorySpi)
{
    checkPluginRegistration(pluginFactorySpi->getPluginName());
    checkPluginVersion(pluginFactorySpi);

    PluginSpi& pluginSpi = pluginFactorySpi->getPlugin();

    if (pluginSpi.getName() != pluginFactorySpi->getPluginName()) {
        throw IllegalArgumentException("The plugin name '" +
                                       pluginSpi.getName() +
                                       "' mismatches the expected name '" +
                                       pluginFactorySpi->getPluginName() +
                                       "' provided by the factory");
    }

    std::unique_ptr<AbstractPluginAdapter> plugin = nullptr;
    try {
        auto observable = std::shared_ptr<ObservablePluginSpi>(
                              &dynamic_cast<ObservablePluginSpi&>(pluginSpi));
        plugin = std::unique_ptr<ObservableLocalPluginAdapter>(
                     new ObservableLocalPluginAdapter(observable));
    } catch (std::bad_cast) {
        try {
            auto autonomous = std::shared_ptr<AutonomousObservablePluginSpi>(
                                  &dynamic_cast<AutonomousObservablePluginSpi&>(pluginSpi));
            plugin = std::unique_ptr<AutonomousObservableLocalPluginAdapter>(
                         new AutonomousObservableLocalPluginAdapter(autonomous));
        } catch (std::bad_cast) {
            plugin = std::unique_ptr<LocalPluginAdapter>(
                         new LocalPluginAdapter(
                             std::shared_ptr<PluginSpi>(&pluginSpi)));
        }
    }
    
    return plugin;
}

std::unique_ptr<AbstractPluginAdapter> SmartCardServiceAdapter::createLocalPoolPlugin(
        std::shared_ptr<PoolPluginFactorySpi> poolPluginFactorySpi)
{
    checkPluginRegistration(poolPluginFactorySpi->getPoolPluginName());
    checkPoolPluginVersion(poolPluginFactorySpi);

    PoolPluginSpi& poolPluginSpi = poolPluginFactorySpi->getPoolPlugin();

    if (poolPluginSpi.getName() != poolPluginFactorySpi->getPoolPluginName()) {
        throw IllegalArgumentException("The pool plugin name '" +
                                       poolPluginSpi.getName() +
                                       "' mismatches the expected name '" +
                                       poolPluginFactorySpi->getPoolPluginName() +
                                       "' provided by the factory");
    }

    return std::unique_ptr<LocalPoolPluginAdapter>(
               new LocalPoolPluginAdapter(
                    std::shared_ptr<PoolPluginSpi>(&poolPluginSpi)));
}


}
}
}
