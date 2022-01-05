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

#include "Job.h"

#include <iostream>

/* Keyple Core Util */
#include "IllegalArgumentException.h"

namespace keyple {
namespace core {
namespace service {
namespace cpp {

using namespace keyple::core::util::cpp::exception;

Job::Job(const std::string& name) : Thread(name), mCancelled(false) {}

bool Job::cancel(const bool mayInterruptIfRunning)
{
    if (mayInterruptIfRunning) {
        throw IllegalArgumentException("Unsupported value for mayInterruptIfRunning (true)");
    }

    if (!isAlive()) {
        return false;
    }

    mCancelled = true;

    return true;
}

bool Job::isDone()
{
    return mDone;
}

bool Job::isCancelled() const
{
    return mCancelled;
}

}
}
}
}
