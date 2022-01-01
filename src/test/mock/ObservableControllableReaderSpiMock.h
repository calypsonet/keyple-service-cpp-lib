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
#include "ObservableReaderSpi.h"
#include "WaitForCardInsertionAutonomousSpi.h"
#include "WaitForCardRemovalAutonomousSpi.h"
#include "DontWaitForCardRemovalDuringProcessingSpi.h"

/* Util */
#include "ControllableReaderSpiMock.h"

using namespace testing;

using namespace keyple::core::common;
using namespace keyple::core::plugin::spi::reader;
using namespace keyple::core::plugin::spi::reader::observable;
using namespace keyple::core::plugin::spi::reader::observable::state::insertion;
using namespace keyple::core::plugin::spi::reader::observable::state::processing;
using namespace keyple::core::plugin::spi::reader::observable::state::removal;

class ObservableControllableReaderSpiMock final
: public KeypleReaderExtension,
  public ConfigurableReaderSpi,
  public ObservableReaderSpi,
  public WaitForCardInsertionAutonomousSpi,
  public WaitForCardRemovalAutonomousSpi,
  public DontWaitForCardRemovalDuringProcessingSpi,
  public ControllableReaderSpiMock {
public:
    MOCK_METHOD(const std::string&, getName, (), (const, override));
    MOCK_METHOD(void, openPhysicalChannel, (), (override));
    MOCK_METHOD(void, closePhysicalChannel, (), (override));
    MOCK_METHOD(bool, isPhysicalChannelOpen, (), (const, override));
    MOCK_METHOD(bool, checkCardPresence, (), (override));
    MOCK_METHOD((const std::string), getPowerOnData, (), (const, override));
    MOCK_METHOD(bool, isContactless, (), (override));
    MOCK_METHOD(void, onUnregister, (), (override));
    MOCK_METHOD(void, onStartDetection, (), (override));
    MOCK_METHOD(void, onStopDetection, (), (override));
    MOCK_METHOD((const std::vector<uint8_t>), transmitApdu, (const std::vector<uint8_t>& apduIn), (override));
    MOCK_METHOD(bool, isProtocolSupported, (const std::string&), (const, override));
    MOCK_METHOD(void, activateProtocol, (const std::string&), (override));
    MOCK_METHOD(void, deactivateProtocol, (const std::string&), (override));
    MOCK_METHOD(bool, isCurrentProtocol, (const std::string&), (const, override));
    MOCK_METHOD(void, connect, (WaitForCardInsertionAutonomousReaderApi*), (override));
    MOCK_METHOD(void, connect, (WaitForCardRemovalAutonomousReaderApi*), (override));
    MOCK_METHOD(void, setCardPresent, (const bool), (override));
};
