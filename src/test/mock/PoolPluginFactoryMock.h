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

using namespace testing;

class PoolPluginFactoryMock final
: public KeyplePluginExtensionFactory, public PoolPluginFactorySpi {
public:
    MOCK_METHOD((const std::string&), getPluginApiVersion, (), (const, override));
    MOCK_METHOD((const std::string&), getCommonApiVersion, (), (const, override));
    MOCK_METHOD((const std::string&), getPoolPluginName, (), (const, override));
    MOCK_METHOD(std::shared_ptr<PoolPluginSpi>, getPoolPlugin, (), (override));
};
