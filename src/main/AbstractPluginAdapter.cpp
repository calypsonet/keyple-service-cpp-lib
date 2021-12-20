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

#include "AbstractPluginAdapter.h"

/* Keyple Core Util */
#include "IllegalStateException.h"

/* Keyple Core Service */
#include "AbstractReaderAdapter.h"
#include "LocalConfigurableReaderAdapter.h"
#include "ObservableLocalConfigurableReaderAdapter.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::util::cpp::exception;

AbstractPluginAdapter::AbstractPluginAdapter(const std::string& pluginName,
                                             std::shared_ptr<KeyplePluginExtension> pluginExtension)
: mPluginName(pluginName), mPluginExtension(pluginExtension) {}

std::shared_ptr<LocalReaderAdapter> AbstractPluginAdapter::buildLocalReaderAdapter(
    std::shared_ptr<ReaderSpi> readerSpi)
{
    std::shared_ptr<LocalReaderAdapter> adapter = nullptr;

    const auto& observable = std::dynamic_pointer_cast<ObservableReaderSpi>(readerSpi);
    const auto& configurable = std::dynamic_pointer_cast<ConfigurableReaderSpi>(readerSpi);
    
    if (observable) {
        if (configurable) {
            adapter = 
                std::make_shared<ObservableLocalConfigurableReaderAdapter>(configurable, getName());
        } else {
            adapter = std::make_shared<ObservableLocalReaderAdapter>(observable, getName());
        }
    } else if (configurable) {
        adapter = std::make_shared<LocalConfigurableReaderAdapter>(configurable, getName());
    } else {
        adapter = std::make_shared<LocalReaderAdapter>(readerSpi, getName());
    }

    return adapter;
  }

void AbstractPluginAdapter::checkStatus() const
{
    if (!mIsRegistered) {
        throw IllegalStateException("The plugin " +
                                    mPluginName +
                                    " is not or no longer registered.");
    }
}

void AbstractPluginAdapter::doRegister()
{
    mIsRegistered = true;
}

void AbstractPluginAdapter::doUnregister()
{
    mIsRegistered = false;
    
    for (const auto& pair : mReaders) {
        try {
            std::dynamic_pointer_cast<AbstractReaderAdapter>(pair.second)->doUnregister();
        } catch (const Exception& e) {
            mLogger->error("Error during the unregistration of reader '%'\n", 
                           pair.second->getName(), 
                           e);
        }
    }
    
    mReaders.clear();
    mIsRegistered = false;
}

const std::string& AbstractPluginAdapter::getName() const
{
    return mPluginName;
}

std::shared_ptr<KeyplePluginExtension> AbstractPluginAdapter::getExtension(
    const std::type_info& pluginExtensionClass) const
{
    (void)pluginExtensionClass;

    checkStatus();

    return mPluginExtension;
}

std::map<const std::string, std::shared_ptr<Reader>>& AbstractPluginAdapter::getReadersMap()
{
    return mReaders;
}

const std::vector<std::string> AbstractPluginAdapter::getReaderNames() const
{
    checkStatus();

    std::vector<std::string> readerNames;
    for (const auto& pair : mReaders) {
        readerNames.push_back(pair.first);
    }

    return readerNames;
}

const std::vector<std::shared_ptr<Reader>> AbstractPluginAdapter::getReaders() const
{
    checkStatus();

    std::vector<std::shared_ptr<Reader>> readers;
    for (const auto& pair : mReaders) {
        readers.push_back(pair.second);
    }

    return readers;
}

std::shared_ptr<Reader> AbstractPluginAdapter::getReader(const std::string& name) const
{
    checkStatus();

    if (mReaders.find(name) != mReaders.end()) {
        return mReaders.at(name);
    }

    return nullptr;
}

}
}
}
