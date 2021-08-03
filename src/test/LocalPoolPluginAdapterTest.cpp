/**************************************************************************************************
 * Copyright (c) 2021 Calypso Networks Association                                                *
 * https://www.calypsonet-asso.org/                                                               *
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

/* Keyple Core Common */
#include "KeyplePluginExtension.h"

/* Keyple Core Plugin */
#include "DontWaitForCardRemovalDuringProcessingSpi.h"
#include "KeyplePluginException.h"
#include "ObservableReaderSpi.h"
#include "PluginIOException.h"
#include "PluginSpi.h"
#include "PoolPluginSpi.h"
#include "WaitForCardInsertionBlockingSpi.h"
#include "WaitForCardRemovalBlockingSpi.h"

/* Keyple Core Service */
#include "LocalPluginAdapter.h"
#include "LocalPoolPluginAdapter.h"
#include "LocalReaderAdapter.h"
#include "ObservableReader.h"
#include "ObservableLocalReaderAdapter.h"

using namespace testing;

using namespace keyple::core::commons;
using namespace keyple::core::plugin;
using namespace keyple::core::plugin::spi;
using namespace keyple::core::plugin::spi::reader::observable;
using namespace keyple::core::plugin::spi::reader::observable::state::insertion;
using namespace keyple::core::plugin::spi::reader::observable::state::processing;
using namespace keyple::core::plugin::spi::reader::observable::state::removal;
using namespace keyple::core::service;

const std::string READER_NAME_1 = "reader1";
const std::string READER_NAME_2 = "reader2";
const std::string OBSERVABLE_READER_NAME = "observableReader";
const std::string PLUGIN_NAME = "plugin";
const std::string POOL_PLUGIN_NAME = "poolPlugin";
const std::string GROUP_1 = "group1";
const std::string GROUP_2 = "group2";
const std::string GROUP_3 = "group3";

class LPPAT_PoolPluginSpiMock final : public KeyplePluginExtension, public PoolPluginSpi {
public:
    virtual const std::string& getName() const override final
    {
        return POOL_PLUGIN_NAME;
    }

    MOCK_METHOD((const std::vector<std::string>&),
                getReaderGroupReferences,
                (),
                (const, override, final));

    MOCK_METHOD((std::shared_ptr<ReaderSpi>),
                allocateReader,
                (const std::string& readerGroupReference),
                (override, final));

    MOCK_METHOD(void,
                releaseReader,
                (std::shared_ptr<ReaderSpi> readerSpi),
                (override, final));

    virtual void onUnregister() override final {}
};

class LPAT_PluginSpiMock final : public KeyplePluginExtension, public PluginSpi {
public:
    virtual const std::string& getName() const override final
    {
        return PLUGIN_NAME;
    }

    MOCK_METHOD((const std::vector<std::shared_ptr<ReaderSpi>>),
                searchAvailableReaders,
                (),
                (override, final));

    virtual void onUnregister() override final {}
};

class LPPAT_ObservableReaderSpiMock final
: public ObservableReaderSpi,
  public WaitForCardInsertionBlockingSpi,
  public WaitForCardRemovalBlockingSpi,
  public DontWaitForCardRemovalDuringProcessingSpi {
public:
    LPPAT_ObservableReaderSpiMock() {}

    virtual const std::string& getName() const override final
    {
        return OBSERVABLE_READER_NAME;
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

    virtual void onStartDetection() override final {}

    virtual void onStopDetection() override final {}

    virtual void waitForCardInsertion() override final {}

    virtual void stopWaitForCardInsertion() override final {}

    virtual void waitForCardRemoval() override final {}

    virtual void stopWaitForCardRemoval() override final {}
};

class LPPAT_ReaderSpiMock final : public ReaderSpi {
public:
    LPPAT_ReaderSpiMock(const std::string& name) : mName(name) {}

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
    const std::string mName;
};

static std::shared_ptr<ReaderSpi> readerSpi1;
static std::shared_ptr<ReaderSpi> readerSpi2;
static std::shared_ptr<LPPAT_ObservableReaderSpiMock> observableReader;
static std::shared_ptr<LPPAT_PoolPluginSpiMock> poolPluginSpi;
static std::vector<std::string> groupReference({GROUP_1, GROUP_2});

static void setUp()
{
    readerSpi1 = std::make_shared<LPPAT_ReaderSpiMock>(READER_NAME_1);
    readerSpi2 = std::make_shared<LPPAT_ReaderSpiMock>(READER_NAME_2);
    observableReader = std::make_shared<LPPAT_ObservableReaderSpiMock>();
    poolPluginSpi = std::make_shared<LPPAT_PoolPluginSpiMock>();
    EXPECT_CALL(*poolPluginSpi.get(), getReaderGroupReferences())
        .WillRepeatedly(ReturnRef(groupReference));
    EXPECT_CALL(*poolPluginSpi.get(), allocateReader(GROUP_1))
        .WillRepeatedly(Return(readerSpi1));
    EXPECT_CALL(*poolPluginSpi.get(), allocateReader(GROUP_2))
        .WillRepeatedly(Return(readerSpi2));
    EXPECT_CALL(*poolPluginSpi.get(), releaseReader(_))
        .WillRepeatedly(Return());
}

TEST(LocalPoolPluginAdapterTest, getReaderGroupReferences_whenGettingReferencesFails_shouldKPE)
{
    setUp();

    EXPECT_CALL(*poolPluginSpi.get(), getReaderGroupReferences())
        .Times(1)
        .WillOnce(Throw(PluginIOException("Plugin IO Exception")));

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    EXPECT_THROW(localPluginAdapter.getReaderGroupReferences(), KeyplePluginException);

    /* Force count down to force pointer deletion and expectations check */
    poolPluginSpi.reset();
}

TEST(LocalPoolPluginAdapterTest, getReaderGroupReferences_whenNotRegistered_shouldISE)
{
    setUp();

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);

    EXPECT_THROW(localPluginAdapter.getReaderGroupReferences(), IllegalStateException);
}

TEST(LocalPoolPluginAdapterTest, getReaderGroupReferences_whenSucceeds_shouldReturnReferences)
{
    setUp();

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    const std::vector<std::string>& groupReferences = localPluginAdapter.getReaderGroupReferences();
    ASSERT_EQ(groupReferences.size(), 2);
    ASSERT_TRUE(std::count(groupReferences.begin(),
                           groupReferences.end(),
                           GROUP_1));
    ASSERT_TRUE(std::count(groupReferences.begin(),
                           groupReferences.end(),
                           GROUP_2));

    /* Force count down to force pointer deletion and expectations check */
    poolPluginSpi.reset();
}

TEST(LocalPoolPluginAdapterTest, allocateReader_whenNotRegistered_shouldISE)
{
    setUp();

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    EXPECT_THROW(localPluginAdapter.allocateReader(GROUP_1), IllegalStateException);
}

TEST(LocalPoolPluginAdapterTest, allocateReader_whenAllocatingReaderFails_shouldKPE)
{
    setUp();

    EXPECT_CALL(*poolPluginSpi.get(), allocateReader(_))
        .Times(1)
        .WillOnce(Throw(PluginIOException("Plugin IO Exception")));

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    EXPECT_THROW(localPluginAdapter.allocateReader(GROUP_1), KeyplePluginException);

    /* Force count down to force pointer deletion and expectations check */
    poolPluginSpi.reset();
}

TEST(LocalPoolPluginAdapterTest, allocateReader_whenSucceeds_shouldReturnReader)
{
    setUp();

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    std::shared_ptr<Reader> reader = localPluginAdapter.allocateReader(GROUP_1);
    ASSERT_EQ(reader->getName(), READER_NAME_1);
    ASSERT_NE(std::dynamic_pointer_cast<Reader>(reader), nullptr);
    ASSERT_NE(std::dynamic_pointer_cast<LocalReaderAdapter>(reader), nullptr);

    const std::vector<std::string>& readerNames = localPluginAdapter.getReaderNames();
    ASSERT_EQ(readerNames.size(), 1);
    ASSERT_TRUE(std::count(readerNames.begin(),
                           readerNames.end(),
                           READER_NAME_1));

    const std::vector<std::shared_ptr<Reader>>& readers = localPluginAdapter.getReaders();
    ASSERT_EQ(readers.size(), 1);
    ASSERT_TRUE(std::count(readers.begin(),
                           readers.end(),
                           localPluginAdapter.getReader(READER_NAME_1)));

    /* Force count down to force pointer deletion and expectations check */
    poolPluginSpi.reset();
}

TEST(LocalPoolPluginAdapterTest, allocateReader_whenReaderIsObservable_shouldReturnObservableReader)
{
    setUp();

    EXPECT_CALL(*poolPluginSpi.get(), allocateReader(GROUP_3))
        .Times(1)
        .WillOnce(Return(observableReader));

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    std::shared_ptr<Reader> reader = localPluginAdapter.allocateReader(GROUP_3);
    ASSERT_EQ(reader->getName(), OBSERVABLE_READER_NAME);
    ASSERT_NE(std::dynamic_pointer_cast<ObservableLocalReaderAdapter>(reader), nullptr);

    const std::vector<std::shared_ptr<Reader>>& readers = localPluginAdapter.getReaders();
    ASSERT_EQ(readers.size(), 1);
    ASSERT_TRUE(std::count(readers.begin(),
                           readers.end(),
                           localPluginAdapter.getReader(OBSERVABLE_READER_NAME)));

    /* Force count down to force pointer deletion and expectations check */
    poolPluginSpi.reset();
}

TEST(LocalPoolPluginAdapterTest, releaseReader_whenNotRegistered_shouldISE)
{
    setUp();

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    std::shared_ptr<Reader> reader = localPluginAdapter.allocateReader(GROUP_1);
    localPluginAdapter.doUnregister();

    EXPECT_THROW(localPluginAdapter.releaseReader(reader), IllegalStateException);

    /* Force count down to force pointer deletion and expectations check */
    poolPluginSpi.reset();
}

TEST(LocalPoolPluginAdapterTest, releaseReader_whenSucceeds_shouldRemoveReader)
{
    setUp();

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    std::shared_ptr<Reader> reader = localPluginAdapter.allocateReader(GROUP_1);
    localPluginAdapter.releaseReader(reader);

    ASSERT_EQ(localPluginAdapter.getReaderNames().size(), 0);
    ASSERT_EQ(localPluginAdapter.getReaders().size(), 0);

    /* Force count down to force pointer deletion and expectations check */
    poolPluginSpi.reset();
}

TEST(LocalPoolPluginAdapterTest, releaseReader_whenReleaseReaderFails_shouldKPE_and_RemoveReader)
{
    setUp();

    EXPECT_CALL(*poolPluginSpi.get(), releaseReader(_))
        .Times(1)
        .WillOnce(Throw(PluginIOException("Plugin IO Exception")));

    LocalPoolPluginAdapter localPluginAdapter(poolPluginSpi);
    localPluginAdapter.doRegister();

    std::shared_ptr<Reader> reader = localPluginAdapter.allocateReader(GROUP_1);

    EXPECT_THROW(localPluginAdapter.releaseReader(reader), KeyplePluginException);
    ASSERT_EQ(localPluginAdapter.getReaderNames().size(), 0);
    ASSERT_EQ(localPluginAdapter.getReaders().size(), 0);

    /* Force count down to force pointer deletion and expectations check */
    poolPluginSpi.reset();
}
