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

#include "PluginFactorySpi.h"
#include "KeyplePluginExtensionFactory.h"

using namespace testing;

using namespace keyple::core::common;
using namespace keyple::core::plugin::spi;

class PluginFactoryMock final : public KeyplePluginExtensionFactory, public PluginFactorySpi {
public:
    MOCK_METHOD(const std::string&, getPluginApiVersion, (), (const, override));
    MOCK_METHOD(const std::string&, getCommonApiVersion,(), (const, override));
    MOCK_METHOD(const std::string&, getPluginName,(), (const, override));
    MOCK_METHOD(std::shared_ptr<PluginSpi>, getPlugin, (), (override));
};