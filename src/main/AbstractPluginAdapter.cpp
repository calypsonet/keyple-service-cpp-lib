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

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::util::cpp::exception;

AbstractPluginAdapter::AbstractPluginAdapter(const std::string& pluginName, Object pluginExtension)
: mPluginName(pluginName), mPluginExtension(pluginExtension) {}

void AbstractPluginAdapter::checkStatus() const
{
    if (!mIsRegistered) {
        throw IllegalStateException("The plugin " +
                                    mPluginName +
                                    " is not or no longer registered.");
    }
}

void AbstractPluginAdapter::register()
{
    mIsRegistered = true;
}

void AbstractPluginAdapter::unregister()
{
    mIsRegistered = false;

    for (const auto& pair : mReaders) {
        pair.second->unregister();
    }

    mReaders.clear();
}

const std::string& AbstractPluginAdapter::getName() const
{
    return mPluginName;
}

std::shared_ptr<KeyplePluginExtension> AbstractPluginAdapter::getExtension(
    const std::type_info& pluginExtensionClass) const
{
    checkStatus();

    return mPluginExtension;
}

const std::map<const std::string, std::shared_ptr<Reader>> AbstractPluginAdapter::getReadersMap()
    const
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

    return readers;
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

std::shared_ptr<Reader> getReader(const std::string& name) const
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
