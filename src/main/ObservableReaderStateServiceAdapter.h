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

#include <map>
#include <memory>
#include <mutex>
#include <typeinfo>

/* Keyple Core Util */
#include "LoggerFactory.h"

/* Keyple Core Service */
#include "AbstractObservableStateAdapter.h"
#include "ExecutorService.h"
#include "MonitoringState.h"
#include "ObservableLocalReaderAdapter.h"

/* Keyple Core Plugin */
#include "ObservableReaderSpi.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::plugin::spi::reader::observable;
using namespace keyple::core::service::cpp;
using namespace keyple::core::util::cpp;

using InternalEvent = ObservableLocalReaderAdapter::InternalEvent;

/**
 * (package-private)<br>
 * Manages the internal state of an ObservableLocalReaderAdapter Process InternalEvent
 * against the current state
 *
 * @since 2.0.0
 */
class ObservableReaderStateServiceAdapter final {
public:
    /**
     * (package-private)<br>
     * Initializes the states according to the interfaces implemented by the provided reader.
     *
     * /!\ C++: cannot use a shared_ptr for reader as this is called from constructors
     *
     * @param reader The observable local reader adapter.
     * @since 2.0.0
     */
    ObservableReaderStateServiceAdapter(ObservableLocalReaderAdapter* reader);

    /**
     * (package-private)<br>
     * Thread safe method to communicate an internal event to this reader Use this method to inform
     * the reader of external event like a tag discovered or a card inserted
     *
     * @param event internal event
     * @since 2.0.0
     */
    void onEvent(const InternalEvent event);

    /**
     * (package-private)<br>
     * Thread safe method to switch the state of this reader should only be invoked by this reader or
     * its state
     *
     * @param stateId next state to onActivate
     * @since 2.0.0
     */
    void switchState(const MonitoringState stateId);

    /**
     * (package-private)<br>
     * Get reader current state
     *
     * @return reader current state
     * @since 2.0.0
     */
    std::shared_ptr<AbstractObservableStateAdapter> getCurrentState();

    /**
     * (package-private)<br>
     * Get the reader current monitoring state
     *
     * @return current monitoring state
     * @since 2.0.0
     */
    MonitoringState getCurrentMonitoringState();

    /**
     * (package-private)<br>
     * Shuts down the ExecutorService of this reader.
     *
     * <p>This method should be invoked when the reader monitoring ends in order to stop any
     * remaining threads.
     *
     * @since 2.0.0
     */
    void shutdown();

private:
    /**
     * Logger
     */
    const std::shared_ptr<Logger> mLogger =
        LoggerFactory::getLogger(typeid(ObservableReaderStateServiceAdapter));

    /**
     * ObservableLocalReaderAdapter to manage event and states
     */
    ObservableLocalReaderAdapter* mReader;

    /**
     *
     */
    std::shared_ptr<ObservableReaderSpi> mReaderSpi;

    /**
     * Executor service to provide a unique thread used by the various monitoring jobs
     */
    std::shared_ptr<ExecutorService> mExecutorService;

    /**
     * Map of all instantiated states possible
     */
    std::map<MonitoringState, std::shared_ptr<AbstractObservableStateAdapter>> mStates;

    /**
     * Current currentState of the Observable Reader
     */
    std::shared_ptr<AbstractObservableStateAdapter> mCurrentState;

    /**
     *
     */
    std::mutex mMutex;
};

}
}
}
