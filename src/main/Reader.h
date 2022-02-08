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
#include <typeinfo>
#include <vector>

/* Calypsonet Terminal Reader */
#include "CardReader.h"

/* Keyple Core Common */
#include "KeypleReaderExtension.h"

namespace keyple {
namespace core {
namespace service {

using namespace calypsonet::terminal::reader;
using namespace keyple::core::common;

/**
 * Drives the underlying hardware to configure the search and check for the presence of cards.
 *
 * @since 2.0.0
 */
class Reader : virtual public CardReader {
public:
    /**
     *
     */
    virtual ~Reader() = default;

    /**
     * Returns the KeypleReaderExtension that is reader-specific.
     *
     * <p>Note: the provided argument is used at compile time to check the type consistency.
     *
     * @param readerExtensionClass The specific class of the reader.
     * @param <T> The type of the reader extension.
     * @return A {@link KeypleReaderExtension}.
     * @throws IllegalStateException If reader is no longer registered.
     * @since 2.0.0
     */
    virtual std::shared_ptr<KeypleReaderExtension> getExtension(
        const std::type_info& readerExtensionClass) const = 0;
};

}
}
}
