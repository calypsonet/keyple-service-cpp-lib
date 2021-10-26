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

/* Keyple Core Service */
#include "AbstractReaderAdapter.h"

/* Keyple Core Commons */
#include "KeypleReaderExtension.h"

using namespace testing;

using namespace keyple::core::common;
using namespace keyple::core::service;

class AbstractReaderAdapterMock final : public AbstractReaderAdapter {
public:
    AbstractReaderAdapterMock(const std::string& readerName,
                              const std::shared_ptr<KeypleReaderExtension> readerExtension,
                              const std::string& pluginName)
    : AbstractReaderAdapter(readerName, readerExtension, pluginName) {}

    MOCK_METHOD(void, releaseChannel,(), (override));
    MOCK_METHOD(bool, isContactless,(), (override));
    MOCK_METHOD(bool, isCardPresent,(), (override));
    MOCK_METHOD((std::vector<std::shared_ptr<CardSelectionResponseApi>>),
                processCardSelectionRequests,
                (const std::vector<std::shared_ptr<CardSelectionRequestSpi>>& cardSelectionRequests,
                 const MultiSelectionProcessing multiSelectionProcessing,
                 const ChannelControl channelControl),
                (override));
    MOCK_METHOD((std::shared_ptr<CardResponseApi>),
                processCardRequest,
                (const std::shared_ptr<CardRequestSpi> cardRequest,
                 const ChannelControl channelControl),
                (override));
};
