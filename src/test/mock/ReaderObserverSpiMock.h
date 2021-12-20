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

/* Calypsonet Terminal Reader */
#include "CardReaderEvent.h"
#include "CardReaderObserverSpi.h"

/* Keyple Core Util */
#include "RuntimeException.h"


using namespace testing;

using namespace calypsonet::terminal::reader;
using namespace calypsonet::terminal::reader::spi;

using namespace keyple::core::util::cpp::exception;

class ReaderObserverSpiMock final : public CardReaderObserverSpi {
public:
    ReaderObserverSpiMock(const std::shared_ptr<RuntimeException> e) : mThrowEx(e) {}

    void onReaderEvent(std::shared_ptr<CardReaderEvent> readerEvent)
    {
        mEventTypeReceived.insert({readerEvent->getType(), readerEvent});
        if (mThrowEx != nullptr) {
            throw *mThrowEx.get();
        }
    }

    bool hasReceived(const CardReaderEvent::Type eventType)
    {
        for (const auto& pair : mEventTypeReceived) {
            if (eventType == pair.first) {
                return true;
            }
        }
        
        return false;
    }

    const std::shared_ptr<CardReaderEvent> getLastEventOfType(const CardReaderEvent::Type eventType)
    {
        const auto it = mEventTypeReceived.find(eventType);
        if (it != mEventTypeReceived.end()) {
            return it->second;
        }
        
        return nullptr;
    }

private:
    std::map<CardReaderEvent::Type, std::shared_ptr<CardReaderEvent>> mEventTypeReceived;
    std::shared_ptr<RuntimeException> mThrowEx;
};