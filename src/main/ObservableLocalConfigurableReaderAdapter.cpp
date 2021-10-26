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

#include "ObservableLocalConfigurableReaderAdapter.h"

namespace keyple {
namespace core {
namespace service {

ObservableLocalConfigurableReaderAdapter::ObservableLocalConfigurableReaderAdapter(
  std::shared_ptr<ConfigurableReaderSpi> configurableReaderSpi, const std::string& pluginName)
: ObservableLocalReaderAdapter(
      std::dynamic_pointer_cast<ObservableReaderSpi>(configurableReaderSpi), pluginName) {}

   
void ObservableLocalConfigurableReaderAdapter::activateProtocol(
    const std::string& readerProtocol, const std::string& applicationProtocol)
{
    activateReaderProtocol(readerProtocol, applicationProtocol);
}

void ObservableLocalConfigurableReaderAdapter::deactivateProtocol(const std::string& readerProtocol)
{
    deactivateReaderProtocol(readerProtocol);
}

}
}
}
