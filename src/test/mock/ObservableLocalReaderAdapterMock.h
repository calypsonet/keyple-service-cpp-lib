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
#include "ObservableLocalReaderAdapter.h"

using namespace testing;

using namespace keyple::core::service;

class ObservableLocalReaderAdapterMock final : public ObservableLocalReaderAdapter {
public:
    ObservableLocalReaderAdapterMock(
      std::shared_ptr<ObservableReaderSpi> observableReaderSpi, const std::string& pluginName)
    : ObservableLocalReaderAdapter(observableReaderSpi, pluginName) {}

    MOCK_METHOD(const std::vector<std::shared_ptr<CardSelectionResponseApi>>, 
                transmitCardSelectionRequests,
                (const std::vector<std::shared_ptr<CardSelectionRequestSpi>>& cardSelectionRequests,
                 const MultiSelectionProcessing multiSelectionProcessing,
                 const ChannelControl channelControl),
                (override, final));
};
