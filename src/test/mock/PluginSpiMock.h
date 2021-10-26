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

/* Keyple Core Commons */
#include "KeyplePluginExtension.h"

/* Keyple Core Plugin */
#include "PluginSpi.h"

using namespace testing;

using namespace keyple::core::common;
using namespace keyple::core::plugin::spi;

class PluginSpiMock final : public KeyplePluginExtension, public PluginSpi {
public:
    MOCK_METHOD((const std::string&), getName, (), (const, override));
    MOCK_METHOD(void, onUnregister, (), (override));
    MOCK_METHOD((const std::vector<std::shared_ptr<ReaderSpi>>),
                searchAvailableReaders,
                (),
                (override));
};
