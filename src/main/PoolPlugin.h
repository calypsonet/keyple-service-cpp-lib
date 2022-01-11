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
#include <string>
#include <vector>

/* Keyple Core Service */
#include "Plugin.h"

namespace keyple {
namespace core {
namespace service {

/**
 * Plugin able to handle the access to an undefined number of {@link Reader}.
 *
 * <p>It is typically used to define a plugin built on top of an HSM interface that can allocate a
 * large number of virtual reader slots.
 *
 * <p>A PoolPlugin can't be observable.
 *
 * @since 2.0.0
 */
class PoolPlugin : virtual public Plugin {
public:
    /**
     * Gets a list of group references that identify a group of readers.
     *
     * <p>A group reference can represent a family of readers that all have the same characteristics
     * (for example, containing a SAM with identical key sets).
     *
     * @return a list of String
     * @since 2.0.0
     */
    virtual const std::vector<std::string> getReaderGroupReferences() const = 0;

    /**
     * Gets a {@link Reader} and makes it exclusive to the caller until the {@link
     * #releaseReader(Reader)} method is invoked.
     *
     * <p>The allocated reader belongs to the group targeted with provided reference.
     *
     * @param readerGroupReference The reference of the group to which the reader belongs (may be
     *        null depending on the implementation made).
     * @return A not null reference.
     * @throws KeyplePluginException If the allocation failed due to lack of available reader.
     * @since 2.0.0
     */
    virtual std::shared_ptr<Reader> allocateReader(const std::string& readerGroupReference) = 0;

    /**
     * Releases a Reader previously allocated with allocateReader.
     *
     * <p>This method must be invoked as soon as the reader is no longer needed by the caller of
     * allocateReader in order to free the resource.
     *
     * @param reader The Reader to be released.
     * @since 2.0.0
     */
    virtual void releaseReader(const std::shared_ptr<Reader> reader) = 0;
};

}
}
}
