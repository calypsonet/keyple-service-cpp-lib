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

/* Keyple Core Util */
#include "IllegalArgumentException.h"

namespace keyple {
namespace core {
namespace service {
namespace cpp {

using namespace keyple::core::util::cpp::exception;

Job::Job() : mRunning(false), mCancelled(false) {}

bool Job::cancel(const bool mayInterruptIfRunning)
{
    if (mayInterruptIfRunning) {
        throw IllegalArgumentException("Unsupported value for mayInterruptIfRunning (true)");
    }

    if (!mRunning) {
        return false;
    }

    mRunning = false;
    mCancelled = true;

    return true;
}

bool Job::isDone() const
{
    return !mRunning;
}

bool Job::isCancelled() const
{
    return mCancelled;
}

}
}
}
}
