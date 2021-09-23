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

#include "CardSelectorSpi.h"

using namespace testing;

using namespace calypsonet::terminal::card::spi;

class CardSelectorSpiMock final : public CardSelectorSpi {
public:
    MOCK_METHOD((const std::string&), getCardProtocol, (), (const, override));
    MOCK_METHOD((const std::string&), getPowerOnDataRegex, (), (const, override));
    MOCK_METHOD((const std::vector<uint8_t>), getAid, (), (const, override));
    MOCK_METHOD(FileOccurrence, getFileOccurrence, (), (const, override));
    MOCK_METHOD(FileControlInformation, getFileControlInformation, (), (const, override));
    MOCK_METHOD((const std::vector<int>&), getSuccessfulSelectionStatusWords, (), (const,override));
};
