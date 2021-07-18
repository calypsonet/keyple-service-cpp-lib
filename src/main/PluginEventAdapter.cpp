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

#include "PluginEventAdapter.h"

namespace keyple {
namespace core {
namespace service {

PluginEventAdapter::PluginEventAdapter(const std::string& pluginName,
                                       const std::string& readerName,
                                       const Type type)
: mPluginName(pluginName), mReaderNames({readerName}), mType(type) {}

PluginEventAdapter::PluginEventAdapter(const std::string& pluginName,
                                       const std::vector<std::string>& readerNames,
                                       const Type type)
: mPluginName(pluginName), mReaderNames(readerNames), mType(type) {}

const std::string& PluginEventAdapter::getPluginName() const
{
    return mPluginName;
}

const std::vector<std::string> PluginEventAdapter::getReaderNames() const
{
    return mReaderNames;
}

Type PluginEventAdapter::getType() const
{
    return mType;
}

}
}
}
