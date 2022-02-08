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

/* Calypsonet Terminal Card */
#include "ApduRequestSpi.h"

using namespace testing;

using namespace calypsonet::terminal::card::spi;

class ApduRequestSpiMock : public ApduRequestSpi {
public:
    MOCK_METHOD(const std::vector<uint8_t>&, getApdu, (), (const, override));
    MOCK_METHOD(const std::vector<int>&, getSuccessfulStatusWords, (), (const, override));
    MOCK_METHOD(const std::string&, getInfo, (), (const, override));
};
