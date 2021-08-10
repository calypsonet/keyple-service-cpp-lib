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

#include "gmock/gmock.h"
#include "gtest/gtest.h"

/* Keyple Core Service */
#include "CardSelectionResultAdapter.h"

/* Keyple Core Util */
#include "IllegalStateException.h"

/* Mock */
#include "SmartCardMock.h"

using namespace testing;

using namespace keyple::core::service;
using namespace keyple::core::util::cpp::exception;

static const std::string POWER_ON_DATA = "12345678";

static std::shared_ptr<SmartCardMock> smartCard;

static void setUp()
{
    smartCard = std::make_shared<SmartCardMock>();
    EXPECT_CALL(*smartCard.get(), getPowerOnData()).WillRepeatedly(ReturnRef(POWER_ON_DATA));
}

static void tearDown()
{
    smartCard.reset();
}

TEST(CardSelectionResultAdapterTest, getActiveSelectionIndex_whenNoSmartCard_shouldReturnMinusOne)
{
    setUp();

    CardSelectionResultAdapter cardSelectionResult;

    ASSERT_EQ(cardSelectionResult.getActiveSelectionIndex(), -1);

    tearDown();
}

TEST(CardSelectionResultAdapterTest,
     getActiveSelectionIndex_whenNullSmartCardAndIsSelected_shouldReturnIndex)
{
    setUp();

    CardSelectionResultAdapter cardSelectionResult;
    cardSelectionResult.addSmartCard(0, nullptr);

    ASSERT_EQ(cardSelectionResult.getActiveSelectionIndex(), 0);

    tearDown();
}

TEST(CardSelectionResultAdapterTest,
     getActiveSelectionIndex_whenNotNullSmartCardAndIsSelected_shouldReturnIndex)
{
    setUp();

    CardSelectionResultAdapter cardSelectionResult;
    cardSelectionResult.addSmartCard(0, smartCard);

    ASSERT_EQ(cardSelectionResult.getActiveSelectionIndex(), 0);

    tearDown();
}

TEST(CardSelectionResultAdapterTest, getSmartCards_whenNoSmartCard_shouldReturnEmptyMap)
{
    setUp();

    CardSelectionResultAdapter cardSelectionResult;

    ASSERT_EQ(cardSelectionResult.getSmartCards().size(), 0);

    tearDown();
}

TEST(CardSelectionResultAdapterTest, getSmartCards_whenNotNullSmartCard_shouldReturnNotEmptyMap)
{
    setUp();

    CardSelectionResultAdapter cardSelectionResult;
    cardSelectionResult.addSmartCard(0, smartCard);

    auto smartCards = cardSelectionResult.getSmartCards();
    ASSERT_NE(smartCards.size(), 0);

    std::map<int, std::shared_ptr<SmartCard>>::iterator it;
    for (it = smartCards.begin(); it != smartCards.end(); it++) {
        if (it->second == smartCard)
            break;
    }
    ASSERT_NE(it, smartCards.end());

    tearDown();
}

TEST(CardSelectionResultAdapterTest, getSmartCards_whenNoSmartCard_shouldReturnNull)
{
    setUp();

    CardSelectionResultAdapter cardSelectionResult;

    const auto& smartCards = cardSelectionResult.getSmartCards();
    ASSERT_EQ(smartCards.find(0), smartCards.end());

    tearDown();
}

TEST(CardSelectionResultAdapterTest, getSmartCards_whenNotNullSmartCard_shouldReturnSmartCard)
{
    setUp();

    CardSelectionResultAdapter cardSelectionResult;
    cardSelectionResult.addSmartCard(0, smartCard);

    const auto& smartCards = cardSelectionResult.getSmartCards();
    const auto& it = smartCards.find(0);
    ASSERT_NE(it, smartCards.end());
    ASSERT_EQ(it->second, smartCard);

    tearDown();
}

TEST(CardSelectionResultAdapterTest, getActiveSmartCard_whenNoSmartCard_shouldISE)
{
    setUp();

    CardSelectionResultAdapter cardSelectionResult;

    EXPECT_THROW(cardSelectionResult.getActiveSmartCard(), IllegalStateException);

    tearDown();
}

TEST(CardSelectionResultAdapterTest, getActiveSmartCard_whenNotSmartCard_shouldReturnSmartcard)
{
    setUp();

    CardSelectionResultAdapter cardSelectionResult;
    cardSelectionResult.addSmartCard(0, smartCard);

    ASSERT_EQ(cardSelectionResult.getActiveSmartCard(), smartCard);

    tearDown();
}
