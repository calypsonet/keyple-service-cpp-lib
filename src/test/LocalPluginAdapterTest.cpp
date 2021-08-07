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

/* Keyple Core Common */
#include "KeyplePluginExtension.h"

/* Keyple Core Plugin */
#include "DontWaitForCardRemovalDuringProcessingSpi.h"
#include "ObservableReaderSpi.h"
#include "PluginIOException.h"
#include "PluginSpi.h"
#include "WaitForCardInsertionBlockingSpi.h"
#include "WaitForCardRemovalBlockingSpi.h"

/* Keyple Core Service */
#include "LocalPluginAdapter.h"
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

static const std::string PLUGIN_NAME = "plugin";
static const std::string READER_NAME_1 = "reader1";
static const std::string READER_NAME_2 = "reader2";
static const std::string OBSERVABLE_READER_NAME = "observableReader";

class LPAT_PluginSpiMock final : public KeyplePluginExtension, public PluginSpi {
public:
    virtual const std::string& getName() const override final
    {
        return PLUGIN_NAME;
    }

    MOCK_METHOD((const std::vector<std::shared_ptr<ReaderSpi>>),
                searchAvailableReaders,
                (),
                (const, override, final));

    virtual void onUnregister() override final {}
};

class LPAT_ObservableReaderSpiMock final
: public ObservableReaderSpi,
  public WaitForCardInsertionBlockingSpi,
  public WaitForCardRemovalBlockingSpi,
  public DontWaitForCardRemovalDuringProcessingSpi {
public:
    LPAT_ObservableReaderSpiMock(const std::string& name) : mName(name) {}

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

    virtual void onStartDetection() override final {}

    virtual void onStopDetection() override final {}

    virtual void waitForCardInsertion() override final {}

    virtual void stopWaitForCardInsertion() override final {}

    virtual void waitForCardRemoval() override final {}

    virtual void stopWaitForCardRemoval() override final {}

private:
    const std::string mName;
};

class LPAT_ReaderSpiMock final : public ReaderSpi {
public:
    LPAT_ReaderSpiMock(const std::string& name) : mName(name) {}

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

static std::shared_ptr<PluginSpi> pluginSpi;
static std::shared_ptr<ReaderSpi> readerSpi1;
static std::shared_ptr<ReaderSpi> readerSpi2;
static std::shared_ptr<LPAT_ObservableReaderSpiMock> observableReader;

static void setUp()
{
    pluginSpi = std::make_shared<LPAT_PluginSpiMock>();
    readerSpi1 = std::make_shared<LPAT_ReaderSpiMock>(READER_NAME_1);
    readerSpi2 = std::make_shared<LPAT_ReaderSpiMock>(READER_NAME_2);
    observableReader = std::make_shared<LPAT_ObservableReaderSpiMock>(OBSERVABLE_READER_NAME);
}

TEST(LocalPluginAdapterTest, register_whenSearchReaderFails_shouldPIO)
{
    setUp();

    auto plugin = std::dynamic_pointer_cast<LPAT_PluginSpiMock>(pluginSpi);
    EXPECT_CALL(*plugin.get(), searchAvailableReaders())
        .Times(1)
        .WillOnce(Throw(PluginIOException("Plugin IO Exception")));

    LocalPluginAdapter localPluginAdapter(pluginSpi);

    EXPECT_THROW(localPluginAdapter.doRegister(), PluginIOException);

    /* Force count down to force pointer deletion and expectations check */
    pluginSpi.reset();
}

TEST(LocalPluginAdapterTest, register_whenSearchReaderReturnsReader_shouldRegisterReader)
{
    setUp();

    std::vector<std::shared_ptr<ReaderSpi>> readerSpis;
    readerSpis.push_back(readerSpi1);
    readerSpis.push_back(readerSpi2);

    auto plugin = std::dynamic_pointer_cast<LPAT_PluginSpiMock>(pluginSpi);
    EXPECT_CALL(*plugin.get(), searchAvailableReaders())
        .WillRepeatedly(Return(readerSpis));

    LocalPluginAdapter localPluginAdapter(pluginSpi);
    ASSERT_EQ(localPluginAdapter.getName(), PLUGIN_NAME);

    localPluginAdapter.doRegister();
    localPluginAdapter.checkStatus();

    const std::vector<std::string> readerNames = localPluginAdapter.getReaderNames();
    auto it = std::find(readerNames.begin(), readerNames.end(), READER_NAME_1);
    ASSERT_NE(it, readerNames.end());
    it = std::find(readerNames.begin(), readerNames.end(), READER_NAME_2);
    ASSERT_NE(it, readerNames.end());

    const std::vector<std::shared_ptr<Reader>> readers = localPluginAdapter.getReaders();
    ASSERT_EQ(readers.size(), 2);
    auto itt = std::find(readers.begin(),
                         readers.end(),
                         localPluginAdapter.getReader(READER_NAME_1));
    ASSERT_NE(itt, readers.end());
    itt = std::find(readers.begin(), readers.end(), localPluginAdapter.getReader(READER_NAME_2));
    ASSERT_NE(itt, readers.end());

    const auto& reader1 = localPluginAdapter.getReader(READER_NAME_1);
    const auto reader1Class = std::dynamic_pointer_cast<Reader>(reader1);
    ASSERT_NE(reader1Class, nullptr);
    const auto localReader1Class = std::dynamic_pointer_cast<LocalReaderAdapter>(reader1);
    ASSERT_NE(localReader1Class, nullptr);

    const auto& reader2 = localPluginAdapter.getReader(READER_NAME_2);
    const auto reader2Class = std::dynamic_pointer_cast<Reader>(reader2);
    ASSERT_NE(reader2Class, nullptr);
    const auto localReader2Class = std::dynamic_pointer_cast<LocalReaderAdapter>(reader2);
    ASSERT_NE(localReader2Class, nullptr);

    ASSERT_NE(reader1, reader2);

    /* Force count down to force pointer deletion and expectations check */
    pluginSpi.reset();
}

TEST(LocalPluginAdapterTest,
     register_whenSearchReaderReturnsObservableReader_shouldRegisterObservableReader)
{
    setUp();

    std::vector<std::shared_ptr<ReaderSpi>> readerSpis;
    readerSpis.push_back(observableReader);

    auto plugin = std::dynamic_pointer_cast<LPAT_PluginSpiMock>(pluginSpi);
    EXPECT_CALL(*plugin.get(), searchAvailableReaders())
        .WillRepeatedly(Return(readerSpis));

    LocalPluginAdapter localPluginAdapter(pluginSpi);
    localPluginAdapter.doRegister();
    localPluginAdapter.checkStatus();

    const std::vector<std::string> readerNames = localPluginAdapter.getReaderNames();
    auto it = std::find(readerNames.begin(), readerNames.end(), OBSERVABLE_READER_NAME);
    ASSERT_NE(it, readerNames.end());

    const std::vector<std::shared_ptr<Reader>> readers = localPluginAdapter.getReaders();
    ASSERT_EQ(readers.size(), 1);

    const auto& reader = localPluginAdapter.getReader(OBSERVABLE_READER_NAME);
    const auto readerClass = std::dynamic_pointer_cast<ObservableReader>(reader);
    ASSERT_NE(readerClass, nullptr);
    const auto localReaderClass = std::dynamic_pointer_cast<ObservableLocalReaderAdapter>(reader);
    ASSERT_NE(localReaderClass, nullptr);

    /* Force count down to force pointer deletion and expectations check */
    pluginSpi.reset();
}

TEST(LocalPluginAdapterTest, getReaders_whenNotRegistered_shouldISE)
{
    setUp();

    LocalPluginAdapter localPluginAdapter(pluginSpi);

    EXPECT_THROW(localPluginAdapter.getReaders(), IllegalStateException);
}

TEST(LocalPluginAdapterTest, getReader_whenNotRegistered_shouldISE)
{
    setUp();

    LocalPluginAdapter localPluginAdapter(pluginSpi);

    EXPECT_THROW(localPluginAdapter.getReader(READER_NAME_1), IllegalStateException);
}

TEST(LocalPluginAdapterTest, getReaderNames_whenNotRegistered_shouldISE)
{
    setUp();

    LocalPluginAdapter localPluginAdapter(pluginSpi);

    EXPECT_THROW(localPluginAdapter.getReaderNames(), IllegalStateException);
}

TEST(LocalPluginAdapterTest, unregister_shouldDisableMethodsWithISE)
{
    setUp();

    std::vector<std::shared_ptr<ReaderSpi>> readerSpis;
    readerSpis.push_back(readerSpi1);

    auto plugin = std::dynamic_pointer_cast<LPAT_PluginSpiMock>(pluginSpi);
    EXPECT_CALL(*plugin.get(), searchAvailableReaders())
        .WillRepeatedly(Return(readerSpis));

    LocalPluginAdapter localPluginAdapter(pluginSpi);
    localPluginAdapter.doRegister();
    localPluginAdapter.doUnregister();

    EXPECT_THROW(localPluginAdapter.getReaders(), IllegalStateException);

    /* Force count down to force pointer deletion and expectations check */
    pluginSpi.reset();
}

TEST(LocalPluginAdapterTest, getExtension_whenNotRegistered_shouldISE)
{
    setUp();

    LocalPluginAdapter localPluginAdapter(pluginSpi);

    EXPECT_THROW(localPluginAdapter.getExtension(typeid(LPAT_PluginSpiMock)),
                 IllegalStateException);
}

TEST(LocalPluginAdapterTest, getExtension_whenRegistered_shouldReturnExtension)
{
    setUp();

    std::vector<std::shared_ptr<ReaderSpi>> readerSpis;
    auto plugin = std::dynamic_pointer_cast<LPAT_PluginSpiMock>(pluginSpi);
    EXPECT_CALL(*plugin.get(), searchAvailableReaders())
        .WillRepeatedly(Return(readerSpis));

    LocalPluginAdapter localPluginAdapter(pluginSpi);
    localPluginAdapter.doRegister();

    const auto extension = localPluginAdapter.getExtension(typeid(LPAT_PluginSpiMock));
    const auto pluginMock = std::static_pointer_cast<LPAT_PluginSpiMock>(extension);
    ASSERT_NE(pluginMock, nullptr);

    /* Force count down to force pointer deletion and expectations check */
    pluginSpi.reset();
}
