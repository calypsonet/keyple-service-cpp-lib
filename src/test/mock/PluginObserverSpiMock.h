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

#include "gmock/gmock.h"
#include "gtest/gtest.h"

/* Keyple Core Service */
#include "PluginObserverSpi.h"

using namespace testing;

using namespace keyple::core::service::spi;

class PluginObserverSpiMock final : public PluginObserverSpi {
public:
    PluginObserverSpiMock(const std::shared_ptr<RuntimeException> e) : mThrowEx(e) {}

    virtual void onPluginEvent(const std::shared_ptr<PluginEvent> pluginEvent) override final
    {
        mEventTypeReceived.insert({pluginEvent->getType(), pluginEvent});
        if (mThrowEx) {
            throw mThrowEx;
        }
    }

    bool hasReceived(const PluginEvent::Type eventType) const
    {
        return mEventTypeReceived.find(eventType) != mEventTypeReceived.end();
    }

    const std::shared_ptr<PluginEvent> getLastEventOfType(const PluginEvent::Type eventType) const
    {
        return mEventTypeReceived.at(eventType);
    }

private:
    std::map<PluginEvent::Type, std::shared_ptr<PluginEvent>> mEventTypeReceived;
    const std::shared_ptr<RuntimeException> mThrowEx;
};
