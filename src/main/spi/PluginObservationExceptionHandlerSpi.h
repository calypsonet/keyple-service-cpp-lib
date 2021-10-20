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

#include <string>

/* Keyple Core Util */
#include "Exception.h"

namespace keyple {
namespace core {
namespace service {
namespace spi {

using namespace keyple::core::util::cpp::exception;

/**
 * Plugin observation error handler to be notified of exceptions that may occur during operations
 * carried out by the monitoring processes.
 *
 * <p>These exceptions can be thrown either in the internal monitoring layers of the readers or in
 * the application itself.
 *
 * @since 2.0.0
 */
class PluginObservationExceptionHandlerSpi {
public:
    /**
     * Invoked when a runtime exception occurs in the observed plugin.
     *
     * @param pluginName The plugin name
     * @param e The original exception
     * @since 2.0.0
     */
    virtual void onPluginObservationError(const std::string& pluginName,
                                          const std::shared_ptr<Exception> e) = 0;
};

}
}
}
}
