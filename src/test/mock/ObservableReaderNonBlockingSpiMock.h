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
#include "CardIOException.h"
#include "ConfigurableReaderSpi.h"
#include "DontWaitForCardRemovalDuringProcessingSpi.h"
#include "ObservableReaderSpi.h"
#include "ReaderIOException.h"
#include "WaitForCardInsertionNonBlockingSpi.h"
#include "WaitForCardRemovalNonBlockingSpi.h"
#include "WaitForCardRemovalDuringProcessingBlockingSpi.h"

/* Keyple Core Util */
#include "InterruptedException.h"
#include "Thread.h"

/* Keyple Core Service */
#include "ControllableReaderSpiMock.h"

using namespace testing;

using namespace keyple::core::common;
using namespace keyple::core::plugin;
using namespace keyple::core::plugin::spi::reader;
using namespace keyple::core::plugin::spi::reader::observable;
using namespace keyple::core::plugin::spi::reader::observable::state::insertion;
using namespace keyple::core::plugin::spi::reader::observable::state::processing;
using namespace keyple::core::plugin::spi::reader::observable::state::removal;
using namespace keyple::core::util::cpp;
using namespace keyple::core::util::cpp::exception;

class ObservableReaderNonBlockingSpiMock final
: public KeypleReaderExtension,
  public ConfigurableReaderSpi,
  public ObservableReaderSpi,
  public WaitForCardInsertionNonBlockingSpi,
  public WaitForCardRemovalNonBlockingSpi,
  public DontWaitForCardRemovalDuringProcessingSpi,
  public ControllableReaderSpiMock {
public:
    ObservableReaderNonBlockingSpiMock(const std::string& name)
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
        
        if (mCardPresent) {
            return std::vector<uint8_t>();
        } else {
            throw CardIOException("card is not present");
        }
    }

    bool isContactless()
    {
        return false;
    }

    void onUnregister() {}

    void setCardPresent(const bool cardPresent)
    {
        mCardPresent = cardPresent;
    }

private:
    bool mDetectionStarted;
    bool mPhysicalChannelOpen;
    std::atomic<bool> mCardPresent;
    std::string mName;
};

