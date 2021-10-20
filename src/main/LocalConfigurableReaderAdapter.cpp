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

#include "LocalConfigurableReaderAdapter.h"

namespace keyple {
namespace core {
namespace service {

LocalConfigurableReaderAdapter::LocalConfigurableReaderAdapter(
  std::shared_ptr<ConfigurableReaderSpi> configurableReaderSpi, const std::string& pluginName)
: LocalReaderAdapter(configurableReaderSpi, pluginName) {}

void LocalConfigurableReaderAdapter::activateProtocol(const std::string& readerProtocol, 
                                                      const std::string& applicationProtocol)
{
    activateReaderProtocol(readerProtocol, applicationProtocol);
}

void LocalConfigurableReaderAdapter::deactivateProtocol(const std::string& readerProtocol)
{
    deactivateReaderProtocol(readerProtocol);
}

}
}
}
