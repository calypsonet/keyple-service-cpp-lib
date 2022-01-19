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

#include <algorithm>
#include <memory>
#include <mutex>
#include <vector>

/* Keyple Core Util */
#include "IllegalStateException.h"
#include "KeypleAssert.h"
#include "LoggerFactory.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::service::cpp;
using namespace keyple::core::util;

/**
 * (package-private)<br>
 * Manager of event observations for plugins and readers.
 *
 * @param <T> The type of the observers (keyple::core::service::spi::PluginObserverSpi or
 *        calypsonet::terminal::reader::spi::CardReaderObserverSpi).
 * @param <S> The type of the exception handler to use during the observation process
 *        (keyple::core::service::spi::PluginObservationExceptionHandlerSpi or
 *        calypsonet::terminal::reader::spi::CardReaderObservationExceptionHandlerSpi).
 * @since 2.0.0
 */
template<class T, class S>
class ObservationManagerAdapter final {
public:
    /**
     * (package-private)<br>
     * Constructor.
     *
     * @param pluginName The name of the associated plugin (used for log only).
     * @param readerName The name of the associated reader (used for log only) (optional).
     * @since 2.0.0
     */
    ObservationManagerAdapter(const std::string& pluginName, const std::string& readerName)
    : mOwnerComponent(readerName == "" ?
                      "The plugin '" + pluginName + "'" :
                      "The reader '" + readerName + "' of plugin '" + pluginName + "'") {}

    /**
     * (package-private)<br>
     * Adds the provided observer if it is not already present.
     *
     * @param observer The observer to add.
     * @throw IllegalArgumentException If the provided observer is null.
     * @throw IllegalStateException If no observation exception handler has been set.
     * @since 2.0.0
     */
    void addObserver(std::shared_ptr<T> observer)
    {
        mLogger->debug("% is adding the observer '%'\n",
                       mOwnerComponent,
                       observer != nullptr ? typeid(*observer.get()).name() : "null");

        Assert::getInstance().notNull(observer, "observer");

        if (mExceptionHandler == nullptr) {
            throw IllegalStateException("No exception handler defined.");
        }

        const std::lock_guard<std::mutex> lock(mMonitor);

        mObservers.push_back(observer);
    }

    /**
     * (package-private)<br>
     * Removes the provided observer if it is present.
     *
     * @param observer The observer to remove.
     * @throw IllegalArgumentException If the provided observer is null.
     * @since 2.0.0
     */
    void removeObserver(std::shared_ptr<T> observer)
    {
        mLogger->debug("% is removing the observer '%'\n",
                       mOwnerComponent,
                       observer != nullptr ? typeid(*observer.get()).name() : "null");

        const std::lock_guard<std::mutex> lock(mMonitor);

        mObservers.erase(std::remove(mObservers.begin(), mObservers.end(), observer),
                         mObservers.end());
    }

    /**
     * (package-private)<br>
     * Removes all observers.
     *
     * @since 2.0.0
     */
    void clearObservers()
    {
        mLogger->debug("% is removing all observers\n", mOwnerComponent);

        const std::lock_guard<std::mutex> lock(mMonitor);

        mObservers.clear();
    }

    /**
     * (package-private)<br>
     * Gets the number of observers.
     *
     * @return The number of observers.
     * @since 2.0.0
     */
    int countObservers() const
    {
        return static_cast<int>(mObservers.size());
    }

    /**
     * (package-private)<br>
     * Sets the observation exception handler.
     *
     * @param exceptionHandler the observation exception handler.
     * @throw IllegalArgumentException If the provided handler is null.
     * @since 2.0.0
     */
    void setObservationExceptionHandler(std::shared_ptr<S> exceptionHandler)
    {
        Assert::getInstance().notNull(exceptionHandler, "exceptionHandler");

        mExceptionHandler = exceptionHandler;
    }

    /**
     * (package-private)<br>
     * Gets a copy of the set of all observers.
     *
     * @return A not null copy.
     * @since 2.0.0
     */
    const std::vector<std::shared_ptr<T>> getObservers()
    {
        const std::lock_guard<std::mutex> lock(mMonitor);

        /* Voluntary copy of the vector */
        return mObservers;
    }

    /**
     * (package-private)<br>
     * Gets the exception handler used to notify the application of exceptions raised during the
     * observation process.
     *
     * @return Null if no exception handler has been set.
     * @since 2.0.0
     */
    std::shared_ptr<S> getObservationExceptionHandler() const
    {
        return mExceptionHandler;
    }

private:
    /**
     *
     */
    const std::unique_ptr<Logger> mLogger =
        LoggerFactory::getLogger(typeid(ObservationManagerAdapter));

    /**
     *
     */
    const std::string mOwnerComponent;

    /**
     *
     */
    std::vector<std::shared_ptr<T>> mObservers;

    /**
     *
     */
    std::shared_ptr<S> mExceptionHandler;

    /**
     *
     */
    std::mutex mMonitor;
};

}
}
}
