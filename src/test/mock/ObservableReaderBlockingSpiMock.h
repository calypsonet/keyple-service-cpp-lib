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
#include "WaitForCardInsertionBlockingSpi.h"
#include "WaitForCardRemovalBlockingSpi.h"
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

class ObservableReaderBlockingSpiMock final
: public KeypleReaderExtension,
  public ConfigurableReaderSpi,
  public ObservableReaderSpi,
  public WaitForCardInsertionBlockingSpi,
  public WaitForCardRemovalBlockingSpi,
  public WaitForCardRemovalDuringProcessingBlockingSpi,
  public ControllableReaderSpiMock {
public:
    ObservableReaderBlockingSpiMock(const std::string name,
                                    const long waitInsertion,
                                    const long waitRemoval)
    : mDetectionStarted(false),
      mPhysicalChannelOpen(false),
      mCardPresent(false),
      mInsertions(0),
      mRemovals(0),
      mWaitInsertion(waitInsertion),
      mWaitRemoval(waitRemoval),
      mName(name) {}

    void setCardPresent(const bool cardPresent)
    {
        mCardPresent = cardPresent;
    }

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

    /**
     * Wait for a one time card insertion
     *
     * @throws ReaderIOException
     * @throws TaskCanceledException
     */
    void waitForCardInsertion()
    {
        try {
            /* Card is detected after a timeout */
            Thread::sleep(mWaitInsertion);
            mInsertions++;
        } catch (const InterruptedException& e) {
        }

        /* If card already inserted, throw ex */
        if (mInsertions > 1) {
            throw ReaderIOException("no card present");
        }
    }

    void stopWaitForCardInsertion()
    {
        // FIXME: Thread.currentThread().interrupt();
    }

    void waitForCardRemoval()
    {
        try {
            Thread::sleep(mWaitRemoval);
            /* Card removal is detected after a timeout */
            mRemovals++;
        } catch (const InterruptedException& e) {
        }

        if (mRemovals > 1) {
            throw ReaderIOException("card not removed ?!");
        }
    }

    void stopWaitForCardRemoval()
    {
        //FIXME: Thread.currentThread().interrupt();
    }

    void waitForCardRemovalDuringProcessing()
    {
        waitForCardRemoval();
    }

    void stopWaitForCardRemovalDuringProcessing()
    {
        stopWaitForCardRemoval();
    }

private:
    bool mDetectionStarted;
    bool mPhysicalChannelOpen;
    std::atomic<bool> mCardPresent;
    std::atomic<int> mInsertions;
    std::atomic<int> mRemovals;
    long mWaitInsertion;
    long mWaitRemoval;
    std::string mName;
};
