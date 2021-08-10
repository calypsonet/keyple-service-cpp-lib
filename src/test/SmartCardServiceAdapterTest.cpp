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
#include "Reader.h"

/* Keyple Core Plugin */
#include "ObservablePluginSpi.h"
#include "PluginSpi.h"
#include "ReaderSpi.h"

/* Keyple Core Common */
#include "KeypleCardExtension.h"
#include "KeyplePluginExtension.h"
#include "KeyplePluginExtensionFactory.h"

/* Mock */
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


// class SCSAT_PluginMock final : public KeyplePluginExtension, public PluginSpi {};

// class SCSAT_ObservablePluginMock final : public KeyplePluginExtension, public ObservablePluginSpi {};

// class SCSAT_AutonomousObservablePluginMock final
// : public KeyplePluginExtension, public AutonomousObservablePluginSpi {};

// class SCSAT_PoolPluginMock final : public KeyplePluginExtension, public PoolPluginSpi {};

// class SCSAT_RemotePluginMock final : public KeyplePluginExtension, public RemotePluginSpi {};

// class SCSAT_ObservableRemotePluginMock final
// : public KeyplePluginExtension, public ObservableRemotePluginSpi {};

// class SCSAT_RemotePoolPluginMock final : public KeyplePluginExtension, public RemotePoolPluginSpi {};

// class SCSAT_PluginFactoryMock final : public KeyplePluginExtensionFactory, public PluginFactorySpi {};

// class SCSAT_PoolPluginFactoryMock final
// : public KeyplePluginExtensionFactory, public PoolPluginFactorySpi {};

// class SCSAT_RemotePluginFactoryMock final
// : public KeyplePluginExtensionFactory, public RemotePluginFactorySpi {};

// class SCSAT_ObservableRemotePluginFactoryMock final
// : public KeyplePluginExtensionFactory, public RemotePluginFactorySpi {};

// class SCSAT_RemotePoolPluginFactoryMock final :
// public KeyplePluginExtensionFactory, public RemotePluginFactorySpi {};

// class SCSAT_CardExtensionMock final : public KeypleCardExtension {};

// class SCSAT_DistributedLocalServiceMock final
// : public KeypleDistributedLocalServiceExtension, public LocalServiceSpi {};

// class SCSAT_DistributedLocalServiceFactoryMock final
// : public KeypleDistributedLocalServiceExtensionFactory, public LocalServiceFactorySpi {};

// static std::shared_ptr<SCSAT_PluginMock> plugin;
// static std::shared_ptr<SCSAT_ObservablePluginMock> observablePlugin;
// static std::shared_ptr<SCSAT_AutonomousObservablePluginMock> autonomousObservablePlugin;
// static std::shared_ptr<SCSAT_PoolPluginMock> poolPlugin;
// static std::shared_ptr<SCSAT_RemotePluginMock> remotePlugin;
// static std::shared_ptr<SCSAT_ObservableRemotePluginMock> observableRemotePlugin;
// static std::shared_ptr<SCSAT_RemotePoolPluginMock> remotePoolPlugin;
// static std::shared_ptr<ReaderMock> reader;
// static std::shared_ptr<SCSAT_PluginFactoryMock> pluginFactory;
// static std::shared_ptr<SCSAT_PluginFactoryMock> observablePluginFactory;
// static std::shared_ptr<SCSAT_PluginFactoryMock> autonomousObservablePluginFactory;
// static std::shared_ptr<SCSAT_PoolPluginFactoryMock> poolPluginFactory;
// static std::shared_ptr<SCSAT_RemotePluginFactoryMock> remotePluginFactory;
// static std::shared_ptr<SCSAT_ObservableRemotePluginFactoryMock> observableRemotePluginFactory;
// static std::shared_ptr<SCSAT_RemotePoolPluginFactoryMock> remotePoolPluginFactory;
// static std::shared_ptr<SCSAT_CardExtensionMock> cardExtension;
// static std::shared_ptr<SCSAT_DistributedLocalServiceMock> localService;
// static std::shared_ptr<SCSAT_DistributedLocalServiceFactoryMock> localServiceFactory;