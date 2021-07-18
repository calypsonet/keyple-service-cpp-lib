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

#include "LocalPluginAdapter.h"

/* Keyple Core Plugin */
#include "ObservableReaderSpi.h"

/* Keyple Core Service */
#include "LocalReaderAdapter.h"
#include "ObservableLocalReaderAdapter.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::plugin::spi::reader::observable;

LocalPluginAdapter::LocalPluginAdapter(std::shared_ptr<PluginSpi> pluginSpi)
: AbstractPluginAdapter(pluginSpi->getName(),
                        std::dynamic_pointer_cast<KeyplePluginExtension>(pluginSpi)),
  mPluginSpi(pluginSpi) {}

void LocalPluginAdapter::doRegister()
{
    AbstractPluginAdapter::doRegister();

    const std::vector<std::shared_ptr<ReaderSpi>> readerSpiList =
        mPluginSpi->searchAvailableReaders();

    for (const auto& readerSpi : readerSpiList) {
        std::shared_ptr<LocalReaderAdapter> localReaderAdapter;

        if (std::dynamic_pointer_cast<ObservableReaderSpi>(readerSpi)) {
            localReaderAdapter =
                std::make_shared<ObservableLocalReaderAdapter>(
                    std::dynamic_pointer_cast<ObservableReaderSpi>(readerSpi), getName());
        } else {
            localReaderAdapter = std::make_shared<LocalReaderAdapter>(readerSpi, getName());
        }

        getReadersMap().insert({readerSpi->getName(), localReaderAdapter});
        localReaderAdapter->doRegister();
    }
}

void LocalPluginAdapter::doUnregister()
{
    AbstractPluginAdapter::doUnregister();
    mPluginSpi->onUnregister();
}

}
}
}
