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

/* Keyple Core Common */
#include "KeyplePluginExtension.h"

/* Keyple Core Plugin */
#include "ObservablePluginSpi.h"
#include "PluginIOException.h"

/* Mock */
#include "ReaderSpiMock.h"

using namespace testing;

using namespace keyple::core::plugin;
using namespace keyple::core::plugin::spi;

class ObservableLocalPluginSpiMock final
: public KeyplePluginExtension, public ObservablePluginSpi {
public:
    ObservableLocalPluginSpiMock(const std::string& name,
                                 const std::shared_ptr<PluginIOException> pluginError)
    : mName(name), mPluginError(pluginError) {}

    virtual int getMonitoringCycleDuration() const override { return mMonitoringCycleDuration; }
    virtual const std::string& getName() const override { return mName; }
    virtual void onUnregister() override {}

    virtual const std::vector<std::string> searchAvailableReaderNames() override
    {
        if (mPluginError != nullptr) {
            throw *mPluginError.get();
        }

        std::lock_guard<std::mutex> lock(mMutex);

        std::vector<std::string> readerNames;
        for (const auto& reader : mStubReaders) {
            readerNames.push_back(reader.first);
        }

        return readerNames;
    }

    virtual std::shared_ptr<ReaderSpi> searchReader(const std::string& readerName) override
    {
        if (mPluginError != nullptr) {
            throw *mPluginError.get();
        }

        std::lock_guard<std::mutex> lock(mMutex);

        const auto it = mStubReaders.find(readerName);
        if (it != mStubReaders.end()) {
            return it->second;
        }

        return nullptr;
    }

    virtual const std::vector<std::shared_ptr<ReaderSpi>> searchAvailableReaders() override
    {
        std::lock_guard<std::mutex> lock(mMutex);

        std::vector<std::shared_ptr<ReaderSpi>> readers;
        for (const auto& reader : mStubReaders) {
            readers.push_back(reader.second);
        }

        return readers;
    }

    void addReaderName(const std::vector<std::string>& names)
    {
        std::lock_guard<std::mutex> lock(mMutex);

        for (const auto& readerName : names) {
            auto readerSpi = std::make_shared<ReaderSpiMock>(readerName);
            EXPECT_CALL(*readerSpi.get(), onUnregister).WillRepeatedly(Return());
            mStubReaders.insert({readerName, readerSpi});
        }
    }

    void removeReaderName(const std::vector<std::string>& names)
    {
        std::lock_guard<std::mutex> lock(mMutex);
        
        for (const auto& readerName : names) {
            mStubReaders.erase(readerName);
        }
    }

private:
    const std::string mName;
    const int mMonitoringCycleDuration = 0;
    std::map<std::string, std::shared_ptr<ReaderSpi>> mStubReaders;
    const std::shared_ptr<PluginIOException> mPluginError;
    std::mutex mMutex;
};
