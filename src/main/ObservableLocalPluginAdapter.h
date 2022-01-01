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

#include <memory>
#include <typeinfo>

/* Keyple Core Util */
#include "LoggerFactory.h"
#include "Thread.h"

/* Keyple Core Service */
#include "AbstractObservableLocalPluginAdapter.h"
#include "PluginEvent.h"
#include "Reader.h"

/* Keyple Core Plugin */
#include "ObservablePluginSpi.h"


namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::plugin::spi;
using namespace keyple::core::util::cpp;

/**
 * (package-private)<br>
 * Implementation of a local ObservablePlugin.
 *
 * @since 2.0.0
 */
class ObservableLocalPluginAdapter final : public AbstractObservableLocalPluginAdapter {
public:
    /**
     * (package-private)<br>
     * Constructor.
     *
     * @param observablePluginSpi The associated plugin SPI.
     * @since 2.0.0
     */
    ObservableLocalPluginAdapter(std::shared_ptr<ObservablePluginSpi> observablePluginSpi);

    /**
     *
     */
    ~ObservableLocalPluginAdapter();

    /**
     * (package-private)<br>
     * Checks whether the background job is monitoring for new readers.
     *
     * @return True, if the background job is monitoring, false in all other cases.
     * @since 2.0.0
     */
    bool isMonitoring() const;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual void addObserver(std::shared_ptr<PluginObserverSpi> observer) override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual void removeObserver(const std::shared_ptr<PluginObserverSpi> observer) override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual void clearObservers() override;

private:
    /**
     *
     */
    class UncaughtExceptionHandler : public Thread::UncaughtExceptionHandler {
    public:
        /**
         * 
         */
        UncaughtExceptionHandler(ObservableLocalPluginAdapter* parent);

        /**
         * 
         */
        void uncaughtException(std::shared_ptr<Thread> t, std::shared_ptr<Exception> e);

    private:
        /**
         * 
         */
        ObservableLocalPluginAdapter *mParent;
    };

    /**
     * Thread in charge of reporting live events
     */
    class EventThread : public Thread {
    public:
        friend ObservableLocalPluginAdapter;

        /**
         *
         */
        EventThread(const std::string& pluginName, ObservableLocalPluginAdapter* parent);

        /**
         * (private)<br>
         * Indicate whether the thread is running or not
         */
        bool isMonitoring() const;

        /**
         * Marks the thread as one that should end when the last threadWaitTimeout occurs
         */
        void end();

    private:
        /**
         *
         */
        const std::string mPluginName;

        /**
         *
         */
        const long mMonitoringCycleDuration;

        /**
         *
         */
        bool mRunning;

        /**
         * 
         */
        bool mStarted;

        /**
         *
         */
        bool mTerminated;

        /**
         *
         */
        ObservableLocalPluginAdapter* mParent;

        /**
         * (private)<br>
         * Adds a reader to the list of known readers (by the plugin)
         *
         * @param readerName The name of the reader.
         * @throw PluginIOException if an error occurs while searching the reader.
         */
        void addReader(const std::string& readerName);

        /**
         * (private)<br>
         * Removes a reader from the list of known readers (by the plugin)
         */
        void removeReader(const std::shared_ptr<Reader> reader);

        /**
         * (private)<br>
         * Notifies observers of changes in the list of readers
         */
        void notifyChanges(const PluginEvent::Type type,
                           const std::vector<std::string>& changedReaderNames);

        /**
         * (private)<br>
         * Compares the list of current readers to the list provided by the system and adds or removes
         * readers accordingly.<br>
         * Observers are notified of changes.
         *
         * @param actualNativeReaderNames the list of readers currently known by the system
         * @throw PluginIOException if an error occurs while searching readers.
         */
        void processChanges(const std::vector<std::string>& actualNativeReaderNames);

        /**
         * Reader monitoring loop<br>
         * Checks reader insertions and removals<br>
         * Notifies observers of any changes
         * 
         * C++: this replaces run() override
         */
        void execute() override;
    };

    /**
     *
     */
    const std::unique_ptr<Logger> mLogger =
        LoggerFactory::getLogger(typeid(ObservableLocalPluginAdapter));

    /**
     *
     */
    std::shared_ptr<ObservablePluginSpi> mObservablePluginSpi;


    /**
     * Local thread to monitoring readers presence
     */
    std::shared_ptr<EventThread> mThread;
};

}
}
}
