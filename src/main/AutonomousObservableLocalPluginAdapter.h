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

#include <memory>

/* Keyple Core Service */
#include "AbstractObservableLocalPluginAdapter.h"

/* Keyple Core Plugin */
#include "AutonomousObservablePluginApi.h"
#include "AutonomousObservablePluginSpi.h"

/* Keyple Core Util */
#include "LoggerFactory.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::plugin;
using namespace keyple::core::plugin::spi;
using namespace keyple::core::util::cpp;

/**
 * (package-private)<br>
 * Implementation of an autonomous local {@link ObservablePlugin}.
 *
 * @since 2.0.0
 */
class AutonomousObservableLocalPluginAdapter final
: public AbstractObservableLocalPluginAdapter,
  public AutonomousObservablePluginApi,
  public std::enable_shared_from_this<AutonomousObservableLocalPluginAdapter> {
public:
    /**
     * (package-private)<br>
     * Constructor.
     *
     * @param autonomousObservablePluginSpi The associated plugin SPI.
     * @since 2.0.0
     */
    AutonomousObservableLocalPluginAdapter(
        std::shared_ptr<AutonomousObservablePluginSpi> autonomousObservablePluginSpi);

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual void onReaderConnected(const std::vector<std::shared_ptr<ReaderSpi>>& readers) override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual void onReaderDisconnected(const std::vector<std::string>& readerNames) override;

private:
    /**
     *
     */
    const std::unique_ptr<Logger> mLogger =
        LoggerFactory::getLogger(typeid(AutonomousObservableLocalPluginAdapter));

    /**
     * Create and add a reader to the reader list from a readerSpi
     *
     * @param readerSpi spi to create the reader from
     */
    void addReader(std::shared_ptr<ReaderSpi> readerSpi);
};

}
}
}
