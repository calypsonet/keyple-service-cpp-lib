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

/* Keyple Service */
#include "PluginEvent.h"

namespace keyple {
namespace core {
namespace service {
namespace spi {

using namespace keyple::core::service;

/**
 * Plugin observer recipient of the {@link PluginEvent} from a {@link
 * org.eclipse.keyple.core.service.ObservablePlugin}.
 *
 * @since 2.0
 */
class PluginObserverSpi {
public:
    /**
     * Invoked when a plugin event occurs.
     *
     * @param pluginEvent The plugin event.
     * @since 2.0
     */
    virtual void onPluginEvent(const std::shared_ptr<PluginEvent> pluginEvent) = 0;
};

}
}
}
}
