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

#include "AutonomousObservableLocalPluginAdapter.h"

/* Keyple Core Util */
#include "KeypleAssert.h"

/* Keyple Core Plugin */
#include "PluginEventAdapter.h"

/* Keyple Core Service */
#include "LocalReaderAdapter.h"
#include "ObservableLocalReaderAdapter.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::plugin;
using namespace keyple::core::plugin::spi::reader::observable;
using namespace keyple::core::util::cpp;

AutonomousObservableLocalPluginAdapter::AutonomousObservableLocalPluginAdapter(
  std::shared_ptr<AutonomousObservablePluginSpi> autonomousObservablePluginSpi)
: AbstractObservableLocalPluginAdapter(autonomousObservablePluginSpi)
{
    autonomousObservablePluginSpi->connect(this);
}

void AutonomousObservableLocalPluginAdapter::onReaderConnected(
    const std::vector<std::shared_ptr<ReaderSpi>>& readers)
{
    Assert::getInstance().notEmpty(readers, "readers");
    std::vector<std::string> notifyReaders;

    for (const auto& readerSpi : readers) {
        /* Add reader to plugin */
        addReader(readerSpi);
        notifyReaders.push_back(readerSpi->getName());
    }

    notifyObservers(
        std::make_shared<PluginEventAdapter>(getName(),
                                             notifyReaders,
                                             PluginEvent::Type::READER_CONNECTED));
}

void AutonomousObservableLocalPluginAdapter::onReaderDisconnected(
    const std::vector<std::string>& readerNames)
{
    Assert::getInstance().notEmpty(readerNames, "readerNames");
    std::vector<std::string> notifyReaders;

    for (const auto& readerName : readerNames) {
        const std::shared_ptr<Reader> reader = getReader(readerName);
        if (reader == nullptr) {
            mLogger->warn("[%] ObservableLocalPlugin => Impossible to remove reader, reader not " \
                          "found with name : %\n",
                          getName(),
                          readerName);
        } else {
            /* Unregister and remove reader */
            std::dynamic_pointer_cast<LocalReaderAdapter>(reader)->doUnregister();
            getReadersMap().erase(reader->getName());
            mLogger->trace("[%] ObservableLocalPlugin => Remove reader '%' from readers list\n",
                           getName(),
                           reader->getName());

            notifyReaders.push_back(readerName);
        }
    }

    notifyObservers(
        std::make_shared<PluginEventAdapter>(getName(),
                                             notifyReaders,
                                             PluginEvent::Type::READER_DISCONNECTED));
}

void AutonomousObservableLocalPluginAdapter::addReader(std::shared_ptr<ReaderSpi> readerSpi)
{
    std::shared_ptr<LocalReaderAdapter> reader = buildLocalReaderAdapter(readerSpi);
    reader->doRegister();
    getReadersMap().insert({reader->getName(), reader});

    mLogger->trace("[%] ObservableLocalPlugin => Add reader '%' to readers list\n",
                   getName(),
                   readerSpi->getName());
}

}
}
}
