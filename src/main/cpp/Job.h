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

#include <atomic>
#include <future>

/* Keyple Core Service */
#include "Thread.h"

namespace keyple {
namespace core {
namespace service {
namespace cpp {

using namespace keyple::core::util::cpp;

class Job : public Thread {
public:
    /**
     *
     */
    Job(const std::string& name);

    /**
     *
     */
    bool cancel(const bool mayInterruptIfRunning);

    /**
     * Returns true if the task was cancelled
     */
    bool isCancelled() const;

    /**
     * Returns true if the task is completed
     */
    bool isDone();

private:
    /**
     *
     */
    bool mCancelled;
};

}
}
}
}
