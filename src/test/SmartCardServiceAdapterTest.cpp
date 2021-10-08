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
#include "AutonomousObservableLocalPluginAdapter.h"
#include "KeyplePluginException.h"
#include "LocalPluginAdapter.h"
#include "LocalPoolPluginAdapter.h"
#include "ObservablePlugin.h"
#include "ObservableLocalPluginAdapter.h"
#include "PoolPlugin.h"
#include "Reader.h"
#include "SmartCardServiceAdapter.h"

/* Keyple Core Plugin */
#include "ObservablePluginSpi.h"
#include "PluginIOException.h"
#include "PluginSpi.h"
#include "ReaderSpi.h"

/* Keyple Core Common */
#include "KeypleCardExtension.h"
#include "KeyplePluginExtension.h"
#include "KeyplePluginExtensionFactory.h"

/* Mock */
#include "AutonomousObservablePluginSpiMock.h"
#include "ObservablePluginSpiMock.h"
#include "PluginFactoryMock.h"
#include "PluginSpiMock.h"
#include "PoolPluginFactoryMock.h"
#include "PoolPluginSpiMock.h"
#include "ReaderMock.h"

using namespace testing;

using namespace keyple::core::commons;
using namespace keyple::core::plugin::spi;
using namespace keyple::core::plugin::spi::reader;
using namespace keyple::core::service;

static const std::string PLUGIN_NAME = "plugin";
static const std::string OBSERVABLE_PLUGIN_NAME = "observablePlugin";
static const std::string AUTONOMOUS_OBSERVABLE_PLUGIN_NAME = "autonomousObservablePlugin";
static const std::string POOL_PLUGIN_NAME = "poolPlugin";
static const std::string REMOTE_PLUGIN_NAME = "remotePlugin";
static const std::string READER_NAME = "reader";
static const std::string LOCAL_SERVICE_NAME = "localService";

static const std::string SERVICE_API_VERSION = "2.0";
static const std::string COMMONS_API_VERSION = "2.0";
static const std::string PLUGIN_API_VERSION = "2.0";
static const std::string DISTRIBUTED_REMOTE_API_VERSION = "2.0";
static const std::string DISTRIBUTED_LOCAL_API_VERSION = "2.0";
static const std::string READER_API_VERSION = "1.0";
static const std::string CARD_API_VERSION = "1.0";

// class SCSAT_PluginFactoryMock final : public KeyplePluginExtensionFactory, public PluginFactorySpi {};

// class SCSAT_PoolPluginFactoryMock final
// : public KeyplePluginExtensionFactory, public PoolPluginFactorySpi {};

// class SCSAT_CardExtensionMock final : public KeypleCardExtension {};

static SmartCardServiceAdapter& service = SmartCardServiceAdapter::getInstance();

static std::shared_ptr<PluginSpiMock> plugin;
static std::shared_ptr<ObservablePluginSpiMock> observablePlugin;
static std::shared_ptr<AutonomousObservablePluginSpiMock> autonomousObservablePlugin;
static std::shared_ptr<PoolPluginSpiMock> poolPlugin;
// static std::shared_ptr<SCSAT_RemotePluginMock> remotePlugin;
// static std::shared_ptr<SCSAT_ObservableRemotePluginMock> observableRemotePlugin;
// static std::shared_ptr<SCSAT_RemotePoolPluginMock> remotePoolPlugin;
static std::shared_ptr<ReaderMock> _reader;
static std::shared_ptr<PluginFactoryMock> pluginFactory;
static std::shared_ptr<PluginFactoryMock> observablePluginFactory;
static std::shared_ptr<PluginFactoryMock> autonomousObservablePluginFactory;
static std::shared_ptr<PoolPluginFactoryMock> poolPluginFactory;
// static std::shared_ptr<SCSAT_RemotePluginFactoryMock> remotePluginFactory;
// static std::shared_ptr<SCSAT_ObservableRemotePluginFactoryMock> observableRemotePluginFactory;
// static std::shared_ptr<SCSAT_RemotePoolPluginFactoryMock> remotePoolPluginFactory;
// static std::shared_ptr<SCSAT_CardExtensionMock> cardExtension;
// static std::shared_ptr<SCSAT_DistributedLocalServiceMock> localService;
// static std::shared_ptr<SCSAT_DistributedLocalServiceFactoryMock> localServiceFactory;

static std::vector<std::shared_ptr<ReaderSpi>> emptyReaderSet;

static void setUp()
{
    _reader = std::make_shared<ReaderMock>();
    EXPECT_CALL(*_reader.get(), getName()).WillRepeatedly(ReturnRef(READER_NAME));

    plugin = std::make_shared<PluginSpiMock>();
    EXPECT_CALL(*plugin.get(), getName()).WillRepeatedly(ReturnRef(PLUGIN_NAME));
    EXPECT_CALL(*plugin.get(), searchAvailableReaders()).WillRepeatedly(Return(emptyReaderSet));
    EXPECT_CALL(*plugin.get(), onUnregister()).WillRepeatedly(Return());

    observablePlugin = std::make_shared<ObservablePluginSpiMock>();
    EXPECT_CALL(*observablePlugin.get(), getName()).WillRepeatedly(ReturnRef(OBSERVABLE_PLUGIN_NAME));
    EXPECT_CALL(*observablePlugin.get(), searchAvailableReaders()).WillRepeatedly(Return(emptyReaderSet));
    EXPECT_CALL(*observablePlugin.get(), onUnregister()).WillRepeatedly(Return());

    autonomousObservablePlugin = std::make_shared<AutonomousObservablePluginSpiMock>();
    EXPECT_CALL(*autonomousObservablePlugin.get(), getName()).WillRepeatedly(ReturnRef(AUTONOMOUS_OBSERVABLE_PLUGIN_NAME));
    EXPECT_CALL(*autonomousObservablePlugin.get(), searchAvailableReaders()).WillRepeatedly(Return(emptyReaderSet));
    EXPECT_CALL(*autonomousObservablePlugin.get(), connect(_)).WillRepeatedly(Return());
    EXPECT_CALL(*autonomousObservablePlugin.get(), onUnregister()).WillRepeatedly(Return());

    poolPlugin = std::make_shared<PoolPluginSpiMock>();
    EXPECT_CALL(*poolPlugin.get(), getName()).WillRepeatedly(ReturnRef(POOL_PLUGIN_NAME));
    EXPECT_CALL(*poolPlugin.get(), allocateReader(_)).WillRepeatedly(Return(_reader));
    EXPECT_CALL(*poolPlugin.get(), onUnregister()).WillRepeatedly(Return());

    // remotePlugin = mock(RemotePluginMock.class);
    // when(remotePlugin.getName()).thenReturn(REMOTE_PLUGIN_NAME);

    // observableRemotePlugin = mock(ObservableRemotePluginMock.class);
    // when(observableRemotePlugin.getName()).thenReturn(REMOTE_PLUGIN_NAME);

    // remotePoolPlugin = mock(RemotePoolPluginMock.class);
    // when(remotePoolPlugin.getName()).thenReturn(REMOTE_PLUGIN_NAME);

    pluginFactory = std::make_shared<PluginFactoryMock>();
    EXPECT_CALL(*pluginFactory.get(), getPluginName()).WillRepeatedly(ReturnRef(PLUGIN_NAME));
    EXPECT_CALL(*pluginFactory.get(), getCommonsApiVersion()).WillRepeatedly(ReturnRef(COMMONS_API_VERSION));
    EXPECT_CALL(*pluginFactory.get(), getPluginApiVersion()).WillRepeatedly(ReturnRef(PLUGIN_API_VERSION));
    EXPECT_CALL(*pluginFactory.get(), getPlugin()).WillRepeatedly(ReturnRef(*plugin));

    observablePluginFactory = std::make_shared<PluginFactoryMock>();
    EXPECT_CALL(*observablePluginFactory.get(), getPluginName()).WillRepeatedly(ReturnRef(OBSERVABLE_PLUGIN_NAME));
    EXPECT_CALL(*observablePluginFactory.get(), getCommonsApiVersion()).WillRepeatedly(ReturnRef(COMMONS_API_VERSION));
    EXPECT_CALL(*observablePluginFactory.get(), getPluginApiVersion()).WillRepeatedly(ReturnRef(PLUGIN_API_VERSION));
    EXPECT_CALL(*observablePluginFactory.get(), getPlugin()).WillRepeatedly(ReturnRef(*observablePlugin));

    autonomousObservablePluginFactory = std::make_shared<PluginFactoryMock>();
    EXPECT_CALL(*autonomousObservablePluginFactory.get(), getPluginName()).WillRepeatedly(ReturnRef(AUTONOMOUS_OBSERVABLE_PLUGIN_NAME));
    EXPECT_CALL(*autonomousObservablePluginFactory.get(), getCommonsApiVersion()).WillRepeatedly(ReturnRef(COMMONS_API_VERSION));
    EXPECT_CALL(*autonomousObservablePluginFactory.get(), getPluginApiVersion()).WillRepeatedly(ReturnRef(PLUGIN_API_VERSION));
    EXPECT_CALL(*autonomousObservablePluginFactory.get(), getPlugin()).WillRepeatedly(ReturnRef(*autonomousObservablePlugin));
    
    poolPluginFactory = std::make_shared<PoolPluginFactoryMock>();
    EXPECT_CALL(*poolPluginFactory.get(), getPoolPluginName()).WillRepeatedly(ReturnRef(POOL_PLUGIN_NAME));
    EXPECT_CALL(*poolPluginFactory.get(), getCommonsApiVersion()).WillRepeatedly(ReturnRef(COMMONS_API_VERSION));
    EXPECT_CALL(*poolPluginFactory.get(), getPluginApiVersion()).WillRepeatedly(ReturnRef(PLUGIN_API_VERSION));
    EXPECT_CALL(*poolPluginFactory.get(), getPoolPlugin()).WillRepeatedly(ReturnRef(*poolPlugin));

    // remotePluginFactory = mock(RemotePluginFactoryMock.class);
    // when(remotePluginFactory.getRemotePluginName()).thenReturn(REMOTE_PLUGIN_NAME);
    // when(remotePluginFactory.getCommonsApiVersion()).thenReturn(COMMONS_API_VERSION);
    // when(remotePluginFactory.getDistributedRemoteApiVersion())
    //     .thenReturn(DISTRIBUTED_REMOTE_API_VERSION);
    // when(remotePluginFactory.getRemotePlugin()).thenReturn(remotePlugin);

    // observableRemotePluginFactory = mock(ObservableRemotePluginFactoryMock.class);
    // when(observableRemotePluginFactory.getRemotePluginName()).thenReturn(REMOTE_PLUGIN_NAME);
    // when(observableRemotePluginFactory.getCommonsApiVersion()).thenReturn(COMMONS_API_VERSION);
    // when(observableRemotePluginFactory.getDistributedRemoteApiVersion())
    //     .thenReturn(DISTRIBUTED_REMOTE_API_VERSION);
    // when(observableRemotePluginFactory.getRemotePlugin()).thenReturn(observableRemotePlugin);

    // remotePoolPluginFactory = mock(RemotePoolPluginFactoryMock.class);
    // when(remotePoolPluginFactory.getRemotePluginName()).thenReturn(REMOTE_PLUGIN_NAME);
    // when(remotePoolPluginFactory.getCommonsApiVersion()).thenReturn(COMMONS_API_VERSION);
    // when(remotePoolPluginFactory.getDistributedRemoteApiVersion())
    //     .thenReturn(DISTRIBUTED_REMOTE_API_VERSION);
    // when(remotePoolPluginFactory.getRemotePlugin()).thenReturn(remotePoolPlugin);

    // cardExtension = mock(CardExtensionMock.class);
    // when(cardExtension.getCommonsApiVersion()).thenReturn(COMMONS_API_VERSION);
    // when(cardExtension.getCardApiVersion()).thenReturn(CARD_API_VERSION);
    // when(cardExtension.getReaderApiVersion()).thenReturn(READER_API_VERSION);

    // localService = mock(DistributedLocalServiceMock.class);
    // when(localService.getName()).thenReturn(LOCAL_SERVICE_NAME);

    // localServiceFactory = mock(DistributedLocalServiceFactoryMock.class);
    // when(localServiceFactory.getLocalServiceName()).thenReturn(LOCAL_SERVICE_NAME);
    // when(localServiceFactory.getCommonsApiVersion()).thenReturn(COMMONS_API_VERSION);
    // when(localServiceFactory.getDistributedLocalApiVersion())
    //     .thenReturn(DISTRIBUTED_LOCAL_API_VERSION);
    // when(localServiceFactory.getLocalService()).thenReturn(localService);
}

static void tearDown()
{
    service.unregisterPlugin(PLUGIN_NAME);
    service.unregisterPlugin(OBSERVABLE_PLUGIN_NAME);
    service.unregisterPlugin(AUTONOMOUS_OBSERVABLE_PLUGIN_NAME);
    service.unregisterPlugin(POOL_PLUGIN_NAME);
    service.unregisterPlugin(REMOTE_PLUGIN_NAME);
    // service.unregisterDistributedLocalService(LOCAL_SERVICE_NAME);

    pluginFactory.reset();
    plugin.reset();
    observablePluginFactory.reset();
    observablePlugin.reset();
    autonomousObservablePluginFactory.reset();
    autonomousObservablePlugin.reset();
    poolPluginFactory.reset();
    poolPlugin.reset();
    _reader.reset();
}

TEST(SmartCardServiceAdapterTest, getInstance_whenIsInvokedTwice_shouldReturnSameInstance)
{
    setUp();

    ASSERT_EQ(&SmartCardServiceAdapter::getInstance(), &service);

    tearDown();
}

/* Register regular plugin */

TEST(SmartCardServiceAdapterTest, 
     registerPlugin_whenPluginIsCorrect_shouldProducePlugin_BeRegistered_withoutWarning)
{
    setUp();

    std::shared_ptr<Plugin> p = service.registerPlugin(pluginFactory);
    ASSERT_NE(std::dynamic_pointer_cast<Plugin>(p), nullptr);
    ASSERT_NE(std::dynamic_pointer_cast<LocalPluginAdapter>(p), nullptr);
    const auto plugins = service.getPluginNames();
    ASSERT_TRUE(std::count(plugins.begin(), plugins.end(), PLUGIN_NAME));

    tearDown();
}

TEST(SmartCardServiceAdapterTest, 
    registerPlugin_whenPluginIsObservable_shouldProduceObservablePlugin_BeRegistered_withoutWarning)
{
    setUp();

    std::shared_ptr<Plugin> p = service.registerPlugin(observablePluginFactory);
    ASSERT_NE(std::dynamic_pointer_cast<ObservablePlugin>(p), nullptr);
    ASSERT_NE(std::dynamic_pointer_cast<ObservableLocalPluginAdapter>(p), nullptr);
    const auto plugins = service.getPluginNames();
    ASSERT_TRUE(std::count(plugins.begin(), plugins.end(), OBSERVABLE_PLUGIN_NAME));
    
    tearDown();
}

TEST(SmartCardServiceAdapterTest, 
     registerPlugin_whenPluginIsAutonomousObservable_shouldProduceAutonomousObservablePlugin_BeRegistered_withoutWarning)
{
    setUp();

    std::shared_ptr<Plugin> p = service.registerPlugin(autonomousObservablePluginFactory);
    ASSERT_NE(std::dynamic_pointer_cast<ObservablePlugin>(p), nullptr);
    ASSERT_NE(std::dynamic_pointer_cast<AutonomousObservableLocalPluginAdapter>(p), nullptr);
    const auto plugins = service.getPluginNames();
    ASSERT_TRUE(std::count(plugins.begin(), plugins.end(), AUTONOMOUS_OBSERVABLE_PLUGIN_NAME));

    tearDown();
}

TEST(SmartCardServiceAdapterTest, registerPlugin_whenFactoryIsNull_shouldThrowIAE)
{
    setUp();

    EXPECT_THROW(service.registerPlugin(nullptr), IllegalArgumentException);

    tearDown();
}

TEST(SmartCardServiceAdapterTest, registerPlugin_whenFactoryDoesNotImplementSpi_shouldThrowIAE)
{
    setUp();

    EXPECT_THROW(service.registerPlugin(std::make_shared<KeyplePluginExtensionFactory>()), 
                 IllegalArgumentException);

    tearDown();
}

TEST(SmartCardServiceAdapterTest, 
     registerPlugin_whenFactoryPluginNameMismatchesPluginName_shouldIAE_and_notRegister)
{
    setUp();

    const std::string pluginName = "otherPluginName";
    EXPECT_CALL(*pluginFactory.get(), getPluginName()).WillRepeatedly(ReturnRef(pluginName));
    
    
    EXPECT_THROW(service.registerPlugin(pluginFactory), IllegalArgumentException);

    const std::vector<std::string>& pluginNames = service.getPluginNames();
    ASSERT_FALSE(std::count(pluginNames.begin(), pluginNames.end(), PLUGIN_NAME));

    tearDown();
}

TEST(SmartCardServiceAdapterTest,
     registerPlugin_whenCommonsApiVersionDiffers_shouldRegister_and_LogWarn)
{
    setUp();

    const std::string apiVersion = "2.1";
    EXPECT_CALL(*pluginFactory.get(), getCommonsApiVersion()).WillRepeatedly(ReturnRef(apiVersion));
    
    service.registerPlugin(pluginFactory);
    
    const std::vector<std::string>& pluginNames = service.getPluginNames();
    ASSERT_TRUE(std::count(pluginNames.begin(), pluginNames.end(), PLUGIN_NAME));

    // verify(logger).warn(anyString(), eq(PLUGIN_NAME), eq("2.1"), eq(COMMONS_API_VERSION));

    tearDown();
}

TEST(SmartCardServiceAdapterTest,
     registerPlugin_whenPluginApiVersionDiffers_shouldRegister_and_LogWarn)
{
    setUp();

    const std::string apiVersion = "2.1";
    EXPECT_CALL(*pluginFactory.get(), getPluginApiVersion()).WillRepeatedly(ReturnRef(apiVersion));

    service.registerPlugin(pluginFactory);
   
    const std::vector<std::string>& pluginNames = service.getPluginNames();
    ASSERT_TRUE(std::count(pluginNames.begin(), pluginNames.end(), PLUGIN_NAME));

    // verify(logger).warn(anyString(), eq(PLUGIN_NAME), eq("2.1"), eq(PLUGIN_API_VERSION));

    tearDown();
}

TEST(SmartCardServiceAdapterTest, registerPlugin_whenInvokedTwice_shouldISE)
{
    setUp();

    service.registerPlugin(pluginFactory);
    EXPECT_THROW(service.registerPlugin(pluginFactory), IllegalStateException);

    tearDown();
}

TEST(SmartCardServiceAdapterTest, registerPlugin_whenIoException_shouldThrowKeyplePluginException)
{
    setUp();

    EXPECT_CALL(*plugin.get(), searchAvailableReaders())
        .WillRepeatedly(Throw(PluginIOException("Plugin IO Exception")));
    
    EXPECT_THROW(service.registerPlugin(pluginFactory), KeyplePluginException);

    tearDown();
}

/* Register Pool Plugin */

TEST(SmartCardServiceAdapterTest, 
     registerPlugin_Pool_whenPluginIsCorrect_shouldProducePlugin_BeRegistered_withoutWarning)
{
    setUp();

    std::shared_ptr<Plugin> p = service.registerPlugin(poolPluginFactory);
    ASSERT_NE(std::dynamic_pointer_cast<PoolPlugin>(p), nullptr);
    ASSERT_NE(std::dynamic_pointer_cast<LocalPoolPluginAdapter>(p), nullptr);
    const auto plugins = service.getPluginNames();
    ASSERT_TRUE(std::count(plugins.begin(), plugins.end(), POOL_PLUGIN_NAME));

    // verify(logger, times(0)).warn(anyString(), anyString(), anyString(), anyString());

    tearDown();
}

TEST(SmartCardServiceAdapterTest,
     registerPlugin_Pool_whenPluginIsObservable_shouldBeRegistered_withoutWarning)
{
    setUp();

    service.registerPlugin(poolPluginFactory);
    const auto plugins = service.getPluginNames();
    ASSERT_TRUE(std::count(plugins.begin(), plugins.end(), POOL_PLUGIN_NAME));

    // verify(logger, times(0)).warn(anyString(), anyString(), anyString(), anyString());

    tearDown();
}

TEST(SmartCardServiceAdapterTest,
     registerPlugin_Pool_whenFactoryPluginNameMismatchesPluginName_shouldIAE_and_notRegister)
{
    setUp();

    const std::string pluginName = "otherPluginName";

    EXPECT_CALL(*poolPluginFactory.get(), getPoolPluginName())
        .WillRepeatedly(ReturnRef(pluginName));

    EXPECT_THROW(service.registerPlugin(poolPluginFactory), IllegalArgumentException);
    const auto plugins = service.getPluginNames();
    ASSERT_FALSE(std::count(plugins.begin(), plugins.end(), POOL_PLUGIN_NAME));
    
    tearDown();
}

TEST(SmartCardServiceAdapterTest,
     registerPlugin_Pool_whenCommonsApiVersionDiffers_shouldRegister_and_LogWarn)
{
    setUp();

    const std::string apiVersion = "2.1";
    EXPECT_CALL(*poolPluginFactory.get(), getCommonsApiVersion())
        .WillRepeatedly(ReturnRef(apiVersion));

    service.registerPlugin(poolPluginFactory);
    const auto plugins = service.getPluginNames();
    ASSERT_TRUE(std::count(plugins.begin(), plugins.end(), POOL_PLUGIN_NAME));
    
    // verify(logger).warn(anyString(), eq(POOL_PLUGIN_NAME), eq("2.1"), eq(COMMONS_API_VERSION));

    tearDown();
}

TEST(SmartCardServiceAdapterTest,
     registerPlugin_Pool_whenPluginApiVersionDiffers_shouldRegister_and_LogWarn)
{
    setUp();

    const std::string apiVersion = "2.1";
    EXPECT_CALL(*poolPluginFactory.get(), getPluginApiVersion())
        .WillRepeatedly(ReturnRef(apiVersion));

    service.registerPlugin(poolPluginFactory);
    const auto plugins = service.getPluginNames();
    ASSERT_TRUE(std::count(plugins.begin(), plugins.end(), POOL_PLUGIN_NAME));
    
    // verify(logger).warn(anyString(), eq(POOL_PLUGIN_NAME), eq("2.1"), eq(PLUGIN_API_VERSION));

    tearDown();
}

TEST(SmartCardServiceAdapterTest, registerPlugin_Pool_whenInvokedTwice_shouldISE)
{
    setUp();

    service.registerPlugin(poolPluginFactory);
    EXPECT_THROW(service.registerPlugin(poolPluginFactory), IllegalStateException);

    tearDown();
}

/* Register remote plugin */

// @Test
//   public void registerPlugin_Remote_whenPluginIsCorrect_shouldBeRegistered_withoutWarning() {
//     Plugin p = service.registerPlugin(remotePluginFactory);
//     assertThat(p).isInstanceOf(Plugin.class).isInstanceOf(RemotePluginAdapter.class);
//     assertThat(service.getPluginNames().contains(REMOTE_PLUGIN_NAME)).isTrue();
//     verify(logger, times(0)).warn(anyString(), anyString(), anyString(), anyString());
//   }

//   @Test
//   public void
//       registerPlugin_Remote_whenPluginIsObservable_shouldProduceObservablePlugin_BeRegistered_withoutWarning() {
//     Plugin p = service.registerPlugin(observableRemotePluginFactory);
//     assertThat(p)
//         .isInstanceOf(ObservablePlugin.class)
//         .isInstanceOf(ObservableRemotePluginAdapter.class);
//     assertThat(service.getPluginNames().contains(REMOTE_PLUGIN_NAME)).isTrue();
//     verify(logger, times(0)).warn(anyString(), anyString(), anyString(), anyString());
//   }

//   @Test
//   public void
//       registerPlugin_Remote_whenPluginIsPool_shouldProducePoolPlugin_BeRegistered_withoutWarning() {
//     Plugin p = service.registerPlugin(remotePoolPluginFactory);
//     assertThat(p).isInstanceOf(PoolPlugin.class).isInstanceOf(RemotePoolPluginAdapter.class);
//     assertThat(service.getPluginNames().contains(REMOTE_PLUGIN_NAME)).isTrue();
//     verify(logger, times(0)).warn(anyString(), anyString(), anyString(), anyString());
//   }

//   @Test
//   public void
//       registerPlugin_Remote_whenFactoryPluginNameMismatchesPluginName_shouldIAE_and_notRegister() {
//     when(remotePluginFactory.getRemotePluginName()).thenReturn("otherPluginName");
//     try {
//       service.registerPlugin(remotePluginFactory);
//       shouldHaveThrown(IllegalArgumentException.class);
//     } catch (IllegalArgumentException e) {
//     }
//     assertThat(service.getPluginNames().contains(REMOTE_PLUGIN_NAME)).isFalse();
//   }

//   @Test
//   public void registerPlugin_Remote_whenCommonsApiVersionDiffers_shouldRegister_and_LogWarn() {
//     when(remotePluginFactory.getCommonsApiVersion()).thenReturn("2.1");
//     service.registerPlugin(remotePluginFactory);
//     assertThat(service.getPluginNames().contains(REMOTE_PLUGIN_NAME)).isTrue();
//     verify(logger).warn(anyString(), eq(REMOTE_PLUGIN_NAME), eq("2.1"), eq(COMMONS_API_VERSION));
//   }

//   @Test
//   public void registerPlugin_Remote_whenPluginApiVersionDiffers_shouldRegister_and_LogWarn() {
//     when(remotePluginFactory.getDistributedRemoteApiVersion()).thenReturn("2.1");
//     service.registerPlugin(remotePluginFactory);
//     assertThat(service.getPluginNames().contains(REMOTE_PLUGIN_NAME)).isTrue();
//     verify(logger)
//         .warn(anyString(), eq(REMOTE_PLUGIN_NAME), eq("2.1"), eq(DISTRIBUTED_REMOTE_API_VERSION));
//   }

//   @Test(expected = IllegalStateException.class)
//   public void registerPlugin_Remote_whenInvokedTwice_shouldISE() {
//     service.registerPlugin(remotePluginFactory);
//     service.registerPlugin(remotePluginFactory);
//   }

/* Bad version format */

TEST(SmartCardServiceAdapterTest, registerPlugin_whenApiVersionHasBadLength_shouldISE)
{
    setUp();

    const std::string apiVersion = "2.0.0";
    EXPECT_CALL(*pluginFactory.get(), getCommonsApiVersion()).WillRepeatedly(ReturnRef(apiVersion));
    
    EXPECT_THROW(service.registerPlugin(pluginFactory), IllegalStateException);

    tearDown();
}

TEST(SmartCardServiceAdapterTest, registerPlugin_whenApiVersionHasBadFormat_shouldISE)
{
    setUp();

    const std::string apiVersion = "2.A";
    EXPECT_CALL(*pluginFactory.get(), getCommonsApiVersion()).WillRepeatedly(ReturnRef(apiVersion));
    
    EXPECT_THROW(service.registerPlugin(pluginFactory), IllegalStateException);

    tearDown();
}
