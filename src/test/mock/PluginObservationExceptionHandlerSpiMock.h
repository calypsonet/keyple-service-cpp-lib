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

#include "gmock/gmock.h"
#include "gtest/gtest.h"

/* Keyple Core Service */
#include "PluginObservationExceptionHandlerSpi.h"

using namespace testing;

using namespace keyple::core::service::spi;

class PluginObservationExceptionHandlerSpiMock final : public PluginObservationExceptionHandlerSpi {
public:
    PluginObservationExceptionHandlerSpiMock(const std::shared_ptr<RuntimeException> throwEx)
    : mInvoked(false), mThrowEx(throwEx) {}

    virtual void onPluginObservationError(const std::string& pluginName,
                                          const std::shared_ptr<Exception> e) override final
    {
        mInvoked = true;
        if (mThrowEx != nullptr) {
            throw *mThrowEx.get();
        }

        mPluginName = pluginName;
        mE = e;
    }

    bool isInvoked() const { return mInvoked; }
    const std::string& getPluginName() const { return mPluginName; }
    const std::shared_ptr<Exception> getE() const { return mE; }

private:
    bool mInvoked = false;
    std::string mPluginName;
    std::shared_ptr<Exception> mE;
    const std::shared_ptr<RuntimeException> mThrowEx;
};
