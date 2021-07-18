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

#include "Future.h"

namespace keyple {
namespace core {
namespace service {
namespace cpp {

Future::Future() : mRunning(false), mCancelled(false) {}

bool Future::cancel(const bool mayInterruptIfRunning)
{
    if (!mRunning) {
        return false;
    }

    mRunning = false;
    mCancelled = true;

    return true;
}

bool Future::isDone() const
{
    // mMonitoringEvent->wait_for(std::chrono::seconds(0)) != std::future_status::ready)
    return !mRunning;
}

bool Future::isCancelled() const
{
    return mCancelled;
}

}
}
}