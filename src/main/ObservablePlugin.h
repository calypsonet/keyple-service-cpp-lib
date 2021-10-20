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
#include "ExecutorService.h"
#include "Plugin.h"
#include "PluginObserverSpi.h"
#include "PluginObservationExceptionHandlerSpi.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::service::cpp;
using namespace keyple::core::service::spi;

/**
 * Plugin able to observe the connection/disconnection of {@link Reader}.
 *
 * <p>Allows registered observers to receive a {@link PluginEvent} when a reader is
 * connected/disconnected.
 *
 * @since 2.0.0
 */
class ObservablePlugin : virtual public Plugin {
public:
    /**
     * Registers a new observer to be notified when a plugin event occurs.
     *
     * <p>The provided observer must implement the PluginObserverSpi interface to be able to
     * receive the events produced by this plugin (reader connection, disconnection).
     *
     * <p>If applicable, the observation process shall be started when the first observer is added.
     *
     * @param observer An observer object implementing the required interface (should be not null).
     * @since 2.0.0
     * @throw IllegalArgumentException if observer is null.
     * @throw IllegalStateException if no exception handler is defined.
     */
    virtual void addObserver(const std::shared_ptr<PluginObserverSpi> observer) = 0;

    /**
     * Unregisters a plugin observer.
     *
     * <p>The observer will no longer receive any of the events produced by the plugin.
     *
     * <p>If applicable, the observation process shall be stopped when the last observer is removed.
     *
     * @param observer The observer object to be unregistered (should be not null).
     * @since 2.0.0
     * @throw IllegalArgumentException if observer is null.
     */
    virtual void removeObserver(const std::shared_ptr<PluginObserverSpi> observer) = 0;

    /**
     * Unregisters all observers at once.
     *
     * @since 2.0.0
     */
    virtual void clearObservers() = 0;

    /**
     * Provides the current number of registered observers.
     *
     * @return An int.
     * @since 2.0.0
     */
    virtual int countObservers() const = 0;

    /**
     * Sets the exception handler.
     *
     * <p>The invocation of this method is <b>mandatory</b> when the plugin has to be observed.
     *
     * <p>In case of a fatal error during the observation, the handler will receive a notification.
     *
     * @param exceptionHandler The exception handler implemented by the application.
     * @since 2.0.0
     */
    virtual void setPluginObservationExceptionHandler(
        std::shared_ptr<PluginObservationExceptionHandlerSpi> exceptionHandler) = 0;
};

}
}
}
