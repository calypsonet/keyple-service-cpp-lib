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

/* Keyple Core Common */
#include "KeypleReaderExtension.h"

/* Keyple Core Plugin */
#include "ConfigurableReaderSpi.h"
#include "DontWaitForCardRemovalDuringProcessingSpi.h"
#include "ObservableReaderSpi.h"
#include "WaitForCardInsertionAutonomousSpi.h"
#include "WaitForCardRemovalAutonomousSpi.h"

/* Keyple Core Service */
#include "ControllableReaderSpiMock.h"

using namespace testing;

using namespace keyple::core::common;
using namespace keyple::core::plugin::spi::reader;
using namespace keyple::core::plugin::spi::reader::observable;
using namespace keyple::core::plugin::spi::reader::observable::state::insertion;
using namespace keyple::core::plugin::spi::reader::observable::state::processing;
using namespace keyple::core::plugin::spi::reader::observable::state::removal;

class ObservableReaderAutonomousSpiMock final
: public KeypleReaderExtension,
  public ConfigurableReaderSpi,
  public ObservableReaderSpi,
  public WaitForCardInsertionAutonomousSpi,
  public WaitForCardRemovalAutonomousSpi,
  public DontWaitForCardRemovalDuringProcessingSpi,
  public ControllableReaderSpiMock {
public:
    ObservableReaderAutonomousSpiMock(const std::string& name)
    : mDetectionStarted(false), mPhysicalChannelOpen(false), mCardPresent(false), mName(name) {}

    void onStartDetection()
    {
        mDetectionStarted = true;
    }

    void onStopDetection()
    {
        mDetectionStarted = false;
    }

    const std::string& getName() const 
    {
        return mName;
    }

    bool isProtocolSupported(const std::string& readerProtocol) const
    {
        (void)readerProtocol;

        return true;
    }

    void activateProtocol(const std::string& readerProtocol) 
    {
        (void)readerProtocol;
    }

    void deactivateProtocol(const std::string& readerProtocol) 
    {
        (void)readerProtocol;
    }

    bool isCurrentProtocol(const std::string& readerProtocol) const
    {
        (void)readerProtocol;

        return false;
    }

    void openPhysicalChannel()
    {
        mPhysicalChannelOpen = true;
    }

    void closePhysicalChannel() 
    {
        mPhysicalChannelOpen = false;
    }

    bool isPhysicalChannelOpen() const
    {
        return mPhysicalChannelOpen;
    }

    bool checkCardPresence()
    {
        return mCardPresent;
    }

    const std::string getPowerOnData() const
    {
        return "";
    }

    const std::vector<uint8_t> transmitApdu(const std::vector<uint8_t>& apduIn)
    {
        (void)apduIn;

        return std::vector<uint8_t>();
    }

    bool isContactless()
    {
        return false;
    }

    void onUnregister() {}

    void connect(WaitForCardInsertionAutonomousReaderApi* waitForCardInsertionAutonomousReaderApi)
    {
        mWaitForCardInsertionAutonomousReaderApi = waitForCardInsertionAutonomousReaderApi;
    }

    void connect(WaitForCardRemovalAutonomousReaderApi* waitForCardRemovalAutonomousReaderApi)
    {
        mWaitForCardRemovalAutonomousReaderApi = waitForCardRemovalAutonomousReaderApi;
    }

    void setCardPresent(const bool cardPresent)
    {
        mCardPresent = cardPresent;
        if (cardPresent) {
            mWaitForCardInsertionAutonomousReaderApi->onCardInserted();
        } else {
            mWaitForCardRemovalAutonomousReaderApi->onCardRemoved();
        }
    }

private:
    WaitForCardInsertionAutonomousReaderApi* mWaitForCardInsertionAutonomousReaderApi;
    WaitForCardRemovalAutonomousReaderApi* mWaitForCardRemovalAutonomousReaderApi;
    bool mDetectionStarted;
    bool mPhysicalChannelOpen;
    std::atomic<bool> mCardPresent;
    std::string mName;
};