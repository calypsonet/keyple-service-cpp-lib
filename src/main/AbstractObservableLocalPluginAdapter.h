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

/* Keyple Core Util */
#include "LoggerFactory.h"

/* Keyple Core Service */
#include "Job.h"
#include "LocalPluginAdapter.h"
#include "PluginObserverSpi.h"
#include "ObservationManagerAdapter.h"
#include "ObservablePlugin.h"
#include "PluginObservationExceptionHandlerSpi.h"

/* Keyple Core Plugin */
#include "PluginSpi.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::plugin;
using namespace keyple::core::util::cpp;
using namespace keyple::core::service::cpp;
using namespace keyple::core::service::spi;

/**
 * (package-private)<br>
 * Abstract class for all observable local plugin adapters.
 *
 * @since 2.0.0
 */
class AbstractObservableLocalPluginAdapter : public LocalPluginAdapter, public ObservablePlugin {
public:
    /**
     * (package-private)<br>
     * Constructor.
     *
     * @param pluginSpi The associated plugin SPI.
     * @since 2.0.0
     */
    AbstractObservableLocalPluginAdapter(std::shared_ptr<PluginSpi> pluginSpi);

    /**
     * 
     */
    virtual ~AbstractObservableLocalPluginAdapter() = default;

    /**
     * (package-private)<br>
     * Gets the associated observation manager.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    virtual std::shared_ptr<ObservationManagerAdapter<PluginObserverSpi,
                                                      PluginObservationExceptionHandlerSpi>>
        getObservationManager() const final;

    /**
     * (package-private)<br>
     * Notifies all registered observers with the provided {@link PluginEventAdapter}.
     *
     * <p>This method never throws an exception. Any errors at runtime are notified to the application
     * using the exception handler.
     *
     * @param event The plugin event.
     * @since 2.0.0
     */
    virtual void notifyObservers(const std::shared_ptr<PluginEvent> event) final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual void doUnregister() override final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual void addObserver(const std::shared_ptr<PluginObserverSpi> observer) override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual void removeObserver(const std::shared_ptr<PluginObserverSpi> observer) override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual void clearObservers() override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual int countObservers() const override final;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual void setPluginObservationExceptionHandler(
        std::shared_ptr<PluginObservationExceptionHandlerSpi> exceptionHandler) override final;

private:
    /**
     *
     */
    class ObservableLocalPluginAdapterJob final : public Job {
    public:
        /**
         *
         */
        ObservableLocalPluginAdapterJob(std::shared_ptr<PluginObserverSpi> observer,
                                        const std::shared_ptr<PluginEvent> event,
                                        AbstractObservableLocalPluginAdapter* parent);

        /**
         *
         */
        void execute() final;

    private:
        /**
         *
         */
        std::shared_ptr<PluginObserverSpi> mObserver;

        /**
         *
         */
        const std::shared_ptr<PluginEvent> mEvent;

        /**
         *
         */
        AbstractObservableLocalPluginAdapter* mParent;
    };

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
