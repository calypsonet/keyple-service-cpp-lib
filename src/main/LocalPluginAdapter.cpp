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

namespace keyple {
namespace core {
namespace service {

LocalPluginAdapter::LocalPluginAdapter(std::shared_ptr<PluginSpi> pluginSpi)
: AbstractPluginAdapter(pluginSpi->getName(), pluginSpi), mPluginSpi(pluginSpi) {}

void LocalPluginAdapter::register()
{
    AbstractPluginAdapter::register();

    const std::vector<std::shared_ptr<ReaderSpi>> readerSpiList =
        mPluginSpi->searchAvailableReaders();

    for (const auto& readerSpi : readerSpiList) {
        std::shared_ptr<LocalReaderAdapter> localReaderAdapter;

        if (std::dynamic_pointer_cast<ObservableReaderSpi>(readerSpi)) {
            localReaderAdapter = std::make_shared<ObservableLocalReaderAdapter>(readerSpi,
                                                                                getName());
        } else {
            localReaderAdapter = std::make_shared<LocalReaderAdapter>(readerSpi, getName());
        }

        getReadersMap().push_back(readerSpi->getName(), localReaderAdapter);
        localReaderAdapter->register();
    }
}

void LocalPluginAdapter::unregister()
{
    AbstractPluginAdapter::unregister();
    mPluginSpi->onUnregister();
}

}
}
}
