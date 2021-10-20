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
#include "KeypleCardExtension.h"

using namespace testing;

using namespace keyple::core::common;

class CardExtensionMock final : public KeypleCardExtension {
public:
    MOCK_METHOD(const std::string&, getReaderApiVersion,(), (const, override));
    MOCK_METHOD(const std::string&, getCardApiVersion, (), (const, override));
    MOCK_METHOD(const std::string&, getCommonApiVersion, (), (const, override));
};
