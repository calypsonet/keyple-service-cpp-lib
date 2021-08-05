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

/* Calypsonet Terminal Card */
#include "CardRequestSpi.h"

/* Keyple Core Service */
#include "AbstractReaderAdapter.h"

/* Keyple Core Commons */
#include "KeypleReaderExtension.h"

/* Keyple Core Plugin */
#include "ReaderSpi.h"

/* Keyple Core Util */
#include "IllegalStateException.h"

using namespace testing;

using namespace calypsonet::terminal::card::spi;
using namespace keyple::core::commons;
using namespace keyple::core::plugin::spi::reader;
using namespace keyple::core::service;
using namespace keyple::core::util::cpp::exception;

class ARAT_ReaderSpiMock final : public KeypleReaderExtension, public ReaderSpi {
public:
    virtual const std::string& getName() const override final
    {
        return mName;
    }

    virtual bool isProtocolSupported(const std::string& readerProtocol) const override final
    {
        (void)readerProtocol;
        return true;
    }

    virtual void activateProtocol(const std::string& readerProtocol) override final
    {
        (void)readerProtocol;
    }

    virtual void deactivateProtocol(const std::string& readerProtocol) override final
    {
        (void)readerProtocol;
    }

    virtual bool isCurrentProtocol(const std::string& readerProtocol) const override final
    {
        (void)readerProtocol;
        return true;
    }

    virtual void openPhysicalChannel() override final {}

    virtual void closePhysicalChannel() override final {}

    virtual bool isPhysicalChannelOpen() const override final
    {
        return true;
    }

    virtual bool checkCardPresence() override final
    {
        return true;
    }

    virtual const std::string getPowerOnData() const override final
    {
        return "";
    }

    virtual const std::vector<uint8_t> transmitApdu(const std::vector<uint8_t>& apduIn) override
        final
    {
        (void)apduIn;
        return std::vector<uint8_t>();
    }

    virtual bool isContactless() override final
    {
        return true;
    }

    virtual void onUnregister() override final {}

private:
    const std::string mName = "readerSpi";
};

class ARAT_CardResponseApiMock final : public CardResponseApi {
public:
    virtual const std::vector<std::shared_ptr<ApduResponseApi>>& getApduResponses() const override
        final
    {
        return mResponses;
    }

    virtual bool isLogicalChannelOpen() const override final
    {
        return true;
    }

private:
    const std::vector<std::shared_ptr<ApduResponseApi>> mResponses;
};

class ARAT_CardRequestApiMock final : public CardRequestSpi {
public:
    virtual const std::vector<std::shared_ptr<ApduRequestSpi>> getApduRequests() const override
        final
    {
        return {};
    }

    virtual bool stopOnUnsuccessfulStatusWord() override final
    {
        return true;
    }
};

class ARAT_DefaultAbstractReaderAdapterMock final : public AbstractReaderAdapter {
public:
    ARAT_DefaultAbstractReaderAdapterMock(const std::string& readerName,
                                          std::shared_ptr<KeypleReaderExtension> readerExtension,
                                          const std::string& pluginName)
    : AbstractReaderAdapter(readerName, readerExtension, pluginName) {}

    virtual std::vector<std::shared_ptr<CardSelectionResponseApi>> processCardSelectionRequests(
        const std::vector<std::shared_ptr<CardSelectionRequestSpi>>& cardSelectionRequests,
        const MultiSelectionProcessing multiSelectionProcessing,
        const ChannelControl channelControl) override final
    {
        (void)cardSelectionRequests;
        (void)multiSelectionProcessing;
        (void)channelControl;
        return {};
    }

    MOCK_METHOD((std::shared_ptr<CardResponseApi>),
                processCardRequest,
                (const std::shared_ptr<CardRequestSpi> cardRequest,
                 const ChannelControl channelControl),
                (override));

    virtual void releaseChannel() override final {}

    virtual bool isContactless() const override final
    {
        return false;
    }

    virtual bool isCardPresent() override final
    {
        return false;
    }

    virtual void activateProtocol(const std::string& readerProtocol,
                                  const std::string& cardProtocol) override final
    {
        (void)readerProtocol;
        (void)cardProtocol;
    }

    virtual void deactivateProtocol(const std::string& readerProtocol) override final
    {
        (void)readerProtocol;
    }
};

static const std::string PLUGIN_NAME = "plugin";
static const std::string READER_NAME = "reader";
static std::shared_ptr<ReaderSpi> readerSpi;
static std::shared_ptr<CardRequestSpi> cardRequestSpi;
static std::shared_ptr<AbstractReaderAdapter> readerAdapter;

static void setUp()
{
    readerSpi = std::make_shared<ARAT_ReaderSpiMock>();
    cardRequestSpi = std::make_shared<ARAT_CardRequestApiMock>();
    readerAdapter =
        std::make_shared<ARAT_DefaultAbstractReaderAdapterMock>(
            READER_NAME,
            std::dynamic_pointer_cast<KeypleReaderExtension>(readerSpi),
            PLUGIN_NAME);
}

TEST(AbstractReaderAdapterTest, getPluginName_shouldReturnPluginName)
{
    setUp();

    ASSERT_EQ(readerAdapter->getPluginName(), PLUGIN_NAME);
}

TEST(AbstractReaderAdapterTest, getName_shouldReturnReaderName)
{
    setUp();

    ASSERT_EQ(readerAdapter->getName(), READER_NAME);
}

TEST(AbstractReaderAdapterTest, getExtension_whenReaderIsRegistered_shouldReturnExtension)
{
    setUp();

    readerAdapter->doRegister();

    const std::type_info& classInfo = typeid(ARAT_ReaderSpiMock);

    ASSERT_EQ(readerAdapter->getExtension(classInfo),
              std::dynamic_pointer_cast<KeypleReaderExtension>(readerSpi));
}

TEST(AbstractReaderAdapterTest, getExtension_whenReaderIsNotRegistered_shouldISE)
{
    setUp();

    const std::type_info& classInfo = typeid(ARAT_ReaderSpiMock);

    EXPECT_THROW(readerAdapter->getExtension(classInfo), IllegalStateException);
}

TEST(AbstractReaderAdapterTest, transmitCardRequest_whenReaderIsNotRegistered_shouldISE)
{
    setUp();

    auto reader = std::dynamic_pointer_cast<ARAT_DefaultAbstractReaderAdapterMock>(readerAdapter);
    EXPECT_CALL(*reader.get(), processCardRequest(_,_))
        .Times(AtMost(1));

    EXPECT_THROW(readerAdapter->transmitCardRequest(cardRequestSpi, ChannelControl::KEEP_OPEN),
                 IllegalStateException);
}

TEST(AbstractReaderAdapterTest, transmitCardRequest_shouldInvoke_processCardRequest)
{
    setUp();

    readerAdapter->doRegister();

    auto reader = std::dynamic_pointer_cast<ARAT_DefaultAbstractReaderAdapterMock>(readerAdapter);
    EXPECT_CALL(*reader.get(), processCardRequest(_,_))
        .Times(1)
        .WillOnce(Return(std::make_shared<ARAT_CardResponseApiMock>()));

    readerAdapter->transmitCardRequest(cardRequestSpi, ChannelControl::KEEP_OPEN);

    /* Force count down to force pointer deletion and expectations check */
    readerAdapter.reset();
}
