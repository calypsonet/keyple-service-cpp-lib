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

/* Calypsonet Terminal Reader */
#include "SmartCard.h"

/* Calypsonet Terminal Card */
#include "SmartCardSpi.h"

/* Keyple Core Service */
#include "CardSelectionResultAdapter.h"

/* Keyple Core Util */
#include "IllegalStateException.h"

using namespace testing;

using namespace calypsonet::terminal::card::spi;
using namespace calypsonet::terminal::reader::selection::spi;
using namespace keyple::core::service;
using namespace keyple::core::util::cpp::exception;

class CSRAT_SmartCardMock final : public SmartCard, public SmartCardSpi {
public:
    virtual const std::string& getPowerOnData() const override final
    {
        return mPowerOnData;
    }

    virtual const std::vector<uint8_t>& getSelectApplicationResponse() const override final
    {
        return mApplicationResponse;
    }

private:
    const std::string mPowerOnData = "12345678";
    const std::vector<uint8_t> mApplicationResponse;
};

static std::shared_ptr<SmartCard> smartCard;

static void setUp()
{
    smartCard = std::make_shared<CSRAT_SmartCardMock>();
}

TEST(CardSelectionResultAdapterTest, getActiveSelectionIndex_whenNoSmartCard_shouldReturnMinusOne)
{
    setUp();

    CardSelectionResultAdapter cardSelectionResult;

    ASSERT_EQ(cardSelectionResult.getActiveSelectionIndex(), -1);
}

TEST(CardSelectionResultAdapterTest,
     getActiveSelectionIndex_whenNullSmartCardAndIsSelected_shouldReturnIndex)
{
    setUp();

    CardSelectionResultAdapter cardSelectionResult;
    cardSelectionResult.addSmartCard(0, nullptr);

    ASSERT_EQ(cardSelectionResult.getActiveSelectionIndex(), 0);
}

TEST(CardSelectionResultAdapterTest,
     getActiveSelectionIndex_whenNotNullSmartCardAndIsSelected_shouldReturnIndex)
{
    setUp();

    CardSelectionResultAdapter cardSelectionResult;
    cardSelectionResult.addSmartCard(0, smartCard);

    ASSERT_EQ(cardSelectionResult.getActiveSelectionIndex(), 0);
}

TEST(CardSelectionResultAdapterTest, getSmartCards_whenNoSmartCard_shouldReturnEmptyMap)
{
    setUp();

    CardSelectionResultAdapter cardSelectionResult;

    ASSERT_EQ(cardSelectionResult.getSmartCards().size(), 0);
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
}

TEST(CardSelectionResultAdapterTest, getSmartCards_whenNoSmartCard_shouldReturnNull)
{
    setUp();

    CardSelectionResultAdapter cardSelectionResult;

    const auto& smartCards = cardSelectionResult.getSmartCards();
    ASSERT_EQ(smartCards.find(0), smartCards.end());
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
}

TEST(CardSelectionResultAdapterTest, getActiveSmartCard_whenNoSmartCard_shouldISE)
{
    setUp();

    CardSelectionResultAdapter cardSelectionResult;

    EXPECT_THROW(cardSelectionResult.getActiveSmartCard(), IllegalStateException);
}

TEST(CardSelectionResultAdapterTest, getActiveSmartCard_whenNotSmartCard_shouldReturnSmartcard)
{
    setUp();

    CardSelectionResultAdapter cardSelectionResult;
    cardSelectionResult.addSmartCard(0, smartCard);

    ASSERT_EQ(cardSelectionResult.getActiveSmartCard(), smartCard);
}
