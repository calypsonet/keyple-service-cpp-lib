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

#include "ExecutorService.h"

/* Keyple Core Service */
#include "AbstractObservableStateAdapter.h"

/* Keyple Core Util */
#include "Thread.h"

namespace keyple {
namespace core {
namespace service {
namespace cpp {

using namespace keyple::core::service;
using namespace keyple::core::util::cpp;

ExecutorService::ExecutorService() : mRunning(true), mTerminated(false)
{
    mThread = new std::thread(&ExecutorService::run, this);
}

ExecutorService::~ExecutorService()
{
    mRunning = false;

    while (!mTerminated) {
        Thread::sleep(10);
    }
}

void ExecutorService::run()
{
    /* Emulates a SingleThreadExecutor (e.g. only one thread at a time) */

    while (mRunning) {
        if (mPool.size()) {
            /* Start first service and wait until completion */
            std::shared_ptr<Job> job = mPool[0];
            job->run();

            /* Remove from vector */
            mPool.erase(mPool.begin());
        }

        Thread::sleep(100);
    }

    mTerminated = true;
}

void ExecutorService::execute(std::shared_ptr<Job> job)
{
    mPool.push_back(job);
}

std::shared_ptr<Job> ExecutorService::submit(std::shared_ptr<Job> job)
{
    mPool.push_back(job);

    return mPool.back();
}

void ExecutorService::shutdown()
{
    mRunning = false;

    while (!mTerminated) {
        Thread::sleep(10);
    }
}

}
}
}
}
