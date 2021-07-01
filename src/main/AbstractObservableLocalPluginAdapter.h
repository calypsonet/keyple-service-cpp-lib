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

/* Keyple Util */
#include "LoggerFactory.h"

/* Keyple Service */
#include "LocalPluginAdapter.h"
#include "PluginObserverSpi.h"
#include "ObservationManagerAdapter.h"
#include "PluginObservationExceptionHandlerSpi.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::util::cpp;
using namespace keyple::core::service::spi;

/**
 * (package-private)<br>
 * Abstract class for all observable local plugin adapters.
 *
 * @since 2.0
 */
class AbstractObservableLocalPluginAdapter : public LocalPluginAdapter, public ObservablePlugin {
public:
    /**
     * (package-private)<br>
     * Constructor.
     *
     * @param pluginSpi The associated plugin SPI.
     * @since 2.0
     */
    AbstractObservableLocalPluginAdapter(std:shared_ptr<PluginSpi> pluginSpi);

    /**
     * (package-private)<br>
     * Gets the associated observation manager.
     *
     * @return A not null reference.
     * @since 2.0
     */
    virtual std::shared_ptr<ObservationManagerAdapter<PluginObserverSpi,
                                                      PluginObservationExceptionHandlerSpi>
        getObservationManager() const final;

    /**
     * (package-private)<br>
     * Notifies all registered observers with the provided {@link PluginEventAdapter}.
     *
     * <p>This method never throws an exception. Any errors at runtime are notified to the application
     * using the exception handler.
     *
     * @param event The plugin event.
     * @since 2.0
     */
    virtual void notifyObservers(const std::shared_ptr<PluginEvent> event) final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0
     */
    virtual void unregister() override final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0
     */
    virtual void addObserver(const std::shared_ptr<PluginObserverSpi> observer) override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0
     */
    virtual void removeObserver(const std::shared_ptr<PluginObserverSpi> observer) override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0
     */
    virtual void clearObservers() override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0
     */
    virtual int countObservers() override final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0
     */
    virtual void setEventNotificationExecutorService(
        std::shared_ptr<ExecutorService> eventNotificationExecutorService) override final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0
     */
    virtual void setPluginObservationExceptionHandler(
        std::shared_ptr<PluginObservationExceptionHandlerSpi> exceptionHandler) override final;

private:
    /**
     *
     */
    const std::shared_ptr<Logger> mLogger =
        LoggerFactory::getLogger(typeid(AbstractObservableLocalPluginAdapter));

    /**
     *
     */
    std::shared_ptr<ObservationManagerAdapter<PluginObserverSpi,
                                              PluginObservationExceptionHandlerSpi>>
        mObservationManager;

    /**
     * Notifies a single observer of an event.
     *
     * @param observer The observer to notify.
     * @param event The event.
     */
    virtual void notifyObserver(std::shared_ptr<PluginObserverSpi> observer,
                                const std::shared_ptr<PluginEvent> event);
};

}
}
}
