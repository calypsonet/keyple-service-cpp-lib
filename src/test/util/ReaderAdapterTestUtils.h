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
#include "CardSelectionResponseApi.h"
#include "CardSelectorSpi.h"

/* Keyple Core Common */
#include "KeypleReaderExtension.h"

/* Keyple Core Plugin */
#include "ConfigurableReaderSpi.h"
#include "DontWaitForCardRemovalDuringProcessingSpi.h"
#include "ObservableReaderSpi.h"
#include "ReaderSpi.h"
#include "WaitForCardInsertionAutonomousSpi.h"
#include "WaitForCardRemovalAutonomousSpi.h"
#include "WaitForCardRemovalDuringProcessingBlockingSpi.h"

/* Mock */
#include "CardSelectionResponseApiMock.h"
#include "CardSelectorSpiMock.h"
#include "ConfigurableReaderSpiMock.h"
#include "ControllableReaderSpiMock.h"
#include "ObservableControllableReaderSpiMock.h"

using namespace testing;

using namespace calypsonet::terminal::card;
using namespace calypsonet::terminal::card::spi;
using namespace keyple::core::common;
using namespace keyple::core::plugin;
using namespace keyple::core::plugin::spi::reader;
using namespace keyple::core::plugin::spi::reader::observable;
using namespace keyple::core::plugin::spi::reader::observable::state::insertion;
using namespace keyple::core::plugin::spi::reader::observable::state::removal;
using namespace keyple::core::plugin::spi::reader::observable::state::processing;

class ReaderAdapterTestUtils {
public:
    static const std::string READER_NAME;
    static const std::string CARD_PROTOCOL;
    static const std::string OTHER_CARD_PROTOCOL;
    static const std::string POWER_ON_DATA;

    std::vector<std::shared_ptr<CardSelectionResponseApi>> NOT_MATCHING_RESPONSES;
    std::vector<std::shared_ptr<CardSelectionResponseApi>> MULTI_NOT_MATCHING_RESPONSES;
    std::vector<std::shared_ptr<CardSelectionResponseApi>> MATCHING_RESPONSES;
    std::vector<std::shared_ptr<CardSelectionResponseApi>> MULTI_MATCHING_RESPONSES;

    ReaderAdapterTestUtils();
    void reset();
    std::shared_ptr<ConfigurableReaderSpiMock> getReaderSpi();
    std::shared_ptr<ConfigurableReaderSpiMock> getReaderSpiSpy();
    std::shared_ptr<ObservableControllableReaderSpiMock> getObservableReaderSpi();
    std::shared_ptr<CardSelectorSpiMock> getCardSelectorSpi();
    std::shared_ptr<CardSelectionResponseApiMock> getMatchingResponse();
    std::shared_ptr<CardSelectionResponseApiMock> getNotMatchingResponse();
};
