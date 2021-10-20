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

#include <ostream>
#include <string>
#include <vector>

namespace keyple {
namespace core {
namespace service {

/**
 * (package-private)<br>
 * All information about a change of reader state within an ObservablePlugin.
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
class PluginEvent {
public:
    /**
     * The two types of reader event
     *
     * @since 2.0.0
     */
    enum class Type {
        /**
         * A reader has been connected.
         *
         * @since 2.0.0
         */
        READER_CONNECTED,

        /**
         * A reader has been disconnected.
         *
         * @since 2.0.0
         */
        READER_DISCONNECTED,

        /**
         * This plugin has become unavailable.
         *
         * @since 2.0.0
         */
        UNAVAILABLE
    };

    /**
     * Gets the name of the plugin to which the reader that generated the event belongs.
     *
     * @return A not empty string.
     * @since 2.0.0
     */
    virtual const std::string& getPluginName() const = 0;

    /**
     * Gets the names of the readers related to the event in the form of a sorted set.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    virtual const std::vector<std::string> getReaderNames() const = 0;

    /**
     * Gets the plugin event type.
     *
     * @return A not null reference.
     * @since 2.0.0
     */
    virtual Type getType() const = 0;

    /**
     *
     */
    friend std::ostream& operator<<(std::ostream& os, const Type t)
    {
        switch (t) {
        case Type::READER_CONNECTED:
            os << "TYPE = READER_CONNECTED";
            break;
        case Type::READER_DISCONNECTED:
            os << "TYPE = READER_DISCONNECTED";
            break;
        case Type::UNAVAILABLE:
            os << "TYPE = UNAVAILABLE";
            break;
        }

        return os;
    }
};

}
}
}
