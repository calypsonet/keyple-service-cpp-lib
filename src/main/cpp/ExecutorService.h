/**************************************************************************************************
 * Copyright (c) 2020 Calypso Networks Association                                                *
 * https://www.calypsonet-asso.org/                                                               *
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

#include <future>
#include <typeinfo>
#include <vector>

/* Keyple Core Util */
#include "LoggerFactory.h"

namespace keyple {
namespace core {
namespace service {
namespace cpp {

class AbstractMonitoringJobAdapter;
class AbstractObservableStateAdapter;

using namespace keyple::core::service;
using namespace keyple::core::util::cpp;

class ExecutorService final {
public:
    /**
     *
     */
    ExecutorService();

    /**
     *
     */
    ~ExecutorService();

    /**
     *
     */
    std::future<void>* submit(std::shared_ptr<AbstractMonitoringJobAdapter> monitoringJob,
                              AbstractObservableStateAdapter* state,
                              std::atomic<bool>& cancellationFlag);

    /**
     * /!\ MSVC requires operator= to be deleted because of std::future
     * not being copyable.
     */
    ExecutorService& operator=(ExecutorService o) = delete;

    /**
     * /!\ MSVC requires copy constructor to be deleted because of std::future
     * not being copyable.
     */
    ExecutorService(const ExecutorService& o) = delete;

private:
    /**
     *
     */
    std::vector<std::future<void>> mPool;

    /**
     *
     */
    std::thread* mThread;

    /**
     *
     */
    void run();

    /**
     *
     */
    std::atomic<bool> mRunning;

    /**
     *
     */
    std::atomic<bool> mTerminated;
};

}
}
}
}
