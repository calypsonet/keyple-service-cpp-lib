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

#include "AbstractObservableLocalPluginAdapter.h"

#include <string>
#include <vector>

/* Keyple Core Plugin */
#include "PluginEventAdapter.h"

/* Keyple Core Util */
#include "Exception.h"

namespace keyple {
namespace core {
namespace service {

/* ABSTRACT OBSERVABLE LOCAL PLUGIN ADAPTER JOB ------------------------------------------------- */

AbstractObservableLocalPluginAdapter::ObservableLocalPluginAdapterJob
    ::ObservableLocalPluginAdapterJob(std::shared_ptr<PluginObserverSpi> observer,
                                      const std::shared_ptr<PluginEvent> event,
                                      AbstractObservableLocalPluginAdapter* parent)
: mObserver(observer), mEvent(event), mParent(parent) {}

void* AbstractObservableLocalPluginAdapter::ObservableLocalPluginAdapterJob::run()
{
    mParent->notifyObserver(mObserver, mEvent);

    return nullptr;
}

/* ABSTRACT OBSERVABLE LOCAL PLUGIN ADAPTER ----------------------------------------------------- */

AbstractObservableLocalPluginAdapter::AbstractObservableLocalPluginAdapter(
  std::shared_ptr<PluginSpi> pluginSpi)
: LocalPluginAdapter(pluginSpi),
  mObservationManager(
      std::make_shared<ObservationManagerAdapter<PluginObserverSpi,
                                                 PluginObservationExceptionHandlerSpi>>("", "")) {}

std::shared_ptr<ObservationManagerAdapter<PluginObserverSpi, PluginObservationExceptionHandlerSpi>>
    AbstractObservableLocalPluginAdapter::getObservationManager() const
{
    return mObservationManager;
}

void AbstractObservableLocalPluginAdapter::notifyObservers(const std::shared_ptr<PluginEvent> event)
{
    mLogger->debug("The plugin '%' is notifying the plugin event '%' to % observers\n",
                   getName(),
                   event->getType(),
                   countObservers());

    const std::vector<std::shared_ptr<PluginObserverSpi>>& observers =
        mObservationManager->getObservers();

    if (mObservationManager->getEventNotificationExecutorService() == nullptr) {
        /* Synchronous notification */
        for (const auto& observer : observers) {
            notifyObserver(observer, event);
        }
    } else {
        /* Asynchronous notification */
        for (const auto& observer : observers) {
            auto job = std::make_shared<ObservableLocalPluginAdapterJob>(observer, event, this);
            mObservationManager->getEventNotificationExecutorService()->execute(job);
        }
    }
}

void AbstractObservableLocalPluginAdapter::notifyObserver(
    std::shared_ptr<PluginObserverSpi> observer, const std::shared_ptr<PluginEvent> event)
{
    try {
        observer->onPluginEvent(event);
    } catch (const Exception& e) {
        try {
            mObservationManager->getObservationExceptionHandler()
                               ->onPluginObservationError(getName(),
                                                          std::make_shared<Exception>(e));
        } catch (const Exception& e2) {
            mLogger->error("Exception during notification: %\n", e2);
            mLogger->error("Original cause: %\n", e);
        }
    }
}

void AbstractObservableLocalPluginAdapter::doUnregister()
{
    const std::vector<std::string>& unregisteredReaderNames = getReaderNames();

    LocalPluginAdapter::doUnregister();

    notifyObservers(
        std::make_shared<PluginEventAdapter>(
            getName(), unregisteredReaderNames, PluginEvent::Type::UNAVAILABLE));

    clearObservers();
}

void AbstractObservableLocalPluginAdapter::addObserver(
    const std::shared_ptr<PluginObserverSpi> observer)
{
    checkStatus();
    mObservationManager->addObserver(observer);
}

void AbstractObservableLocalPluginAdapter::removeObserver(
    const std::shared_ptr<PluginObserverSpi> observer)
{
    mObservationManager->removeObserver(observer);
}

void AbstractObservableLocalPluginAdapter::clearObservers()
{
    mObservationManager->clearObservers();
}

int AbstractObservableLocalPluginAdapter::countObservers() const
{
    return mObservationManager->countObservers();
}

void AbstractObservableLocalPluginAdapter::setEventNotificationExecutorService(
    std::shared_ptr<ExecutorService> eventNotificationExecutorService)
{
    checkStatus();
    mObservationManager->setEventNotificationExecutorService(eventNotificationExecutorService);
}

void AbstractObservableLocalPluginAdapter::setPluginObservationExceptionHandler(
    std::shared_ptr<PluginObservationExceptionHandlerSpi> exceptionHandler)
{
    checkStatus();
    mObservationManager->setObservationExceptionHandler(exceptionHandler);
}

}
}
}
