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

#include <string>
#include <vector>

/* Keyple Core Service */
#include "PluginEvent.h"

namespace keyple {
namespace core {
namespace service {

/**
 * (package-private)<br>
 * This POJO contains all information about a change of reader state within an ObservablePlugin.
 *
 * <p>The getReaderNames() and getType() methods allow the event recipient to
 * retrieve the names of the readers involved and the type of the event.
 *
 * <p>Since the event provides a list of reader names, a single event can be used to notify a change
 * for one or more readers.
 *
 * <p>However, only one type of event is notified at a time.
 *
 * @since 2.0.0
 */
class PluginEventAdapter final : public PluginEvent {
public:
    /**
     * (package-private)<br>
     * Create a PluginEvent for a single reader from the plugin and reader names and the type of
     * event.
     *
     * @param pluginName A string containing the name of the plugin (should be not null).
     * @param readerName A string containing the name of the reader (should be not null).
     * @param type An event type Type::READER_CONNECTED or Type::READER_DISCONNECTED
     *        (should be not null).
     * @since 2.0.0
     */
    PluginEventAdapter(const std::string& pluginName,
                       const std::string& readerName,
                       const PluginEvent::Type type);

    /**
     * (package-private)<br>
     * Create a PluginEvent for multiple readers from the plugin name, multiple reader names and the
     * type of event.
     *
     * <p>Note: gathering several readers in the same event is always done for a same type of event
     * (e.g. simultaneous disconnection of 2 readers).
     *
     * @param pluginName A string containing the name of the plugin (must be not empty).
     * @param readerNames A set of string containing the readers names (must be not empty).
     * @param type An event type {@link Type#READER_CONNECTED} or {@link Type#READER_DISCONNECTED}
     *        (must be not null).
     * @since 2.0.0
     */
    PluginEventAdapter(const std::string& pluginName,
                       const std::vector<std::string>& readerNames,
                       const PluginEvent::Type type);

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual const std::string& getPluginName() const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual const std::vector<std::string> getReaderNames() const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual Type getType() const override;

private:
    /**
     *
     */
    const std::string mPluginName;

    /**
     *
     */
    const std::vector<std::string> mReaderNames;

    /**
     *
     */
    const PluginEvent::Type mType;
};

}
}
}
