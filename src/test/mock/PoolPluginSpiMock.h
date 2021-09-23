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
#include "KeyplePluginExtension.h"

/* Keyple Core Plugin */
#include "PoolPluginSpi.h"

using namespace testing;

using namespace keyple::core::commons;

class PoolPluginSpiMock final : public KeyplePluginExtension, public PoolPluginSpi {
public:
    MOCK_METHOD((const std::string&), getName, (), (const, override));
    MOCK_METHOD((const std::vector<std::string>&), getReaderGroupReferences, (), (const, override));
    MOCK_METHOD(void, releaseReader, (std::shared_ptr<ReaderSpi> readerSpi), (override));
    MOCK_METHOD(void, onUnregister, (), (override));
    MOCK_METHOD((std::shared_ptr<ReaderSpi>),
                allocateReader,
                (const std::string& readerGroupReference),
                (override));
};
