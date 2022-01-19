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

#include "LocalPoolPluginAdapter.h"

/* Keyple Core Plugin */
#include "KeyplePluginException.h"
#include "PluginIOException.h"

/* Keyple Core Util */
#include "KeypleAssert.h"

/* Keyple Core Service */
#include "LocalReaderAdapter.h"
#include "ObservableLocalReaderAdapter.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::plugin;
using namespace keyple::core::plugin::spi::reader::observable;
using namespace keyple::core::util;

LocalPoolPluginAdapter::LocalPoolPluginAdapter(std::shared_ptr<PoolPluginSpi> poolPluginSpi)
: AbstractPluginAdapter(poolPluginSpi->getName(),
                        std::dynamic_pointer_cast<KeyplePluginExtension>(poolPluginSpi)),
  mPoolPluginSpi(poolPluginSpi) {}

void LocalPoolPluginAdapter::doUnregister()
{
    try {
        mPoolPluginSpi->onUnregister();
    } catch (const Exception& e) {
        mLogger->error("Error during the unregistration of the extension of plugin '%'\n",
                       getName(),
                       e);
    }

    AbstractPluginAdapter::doUnregister();
}

const std::vector<std::string> LocalPoolPluginAdapter::getReaderGroupReferences() const
{
    checkStatus();

    try {
        return mPoolPluginSpi->getReaderGroupReferences();
    } catch (const PluginIOException& e) {
        throw KeyplePluginException("The pool plugin " +
                                    getName() +
                                    " is unable to get reader group references : " +
                                    e.getMessage(),
                                    std::make_shared<PluginIOException>(e));
    }
}

std::shared_ptr<Reader> LocalPoolPluginAdapter::allocateReader(
    const std::string& readerGroupReference)
{
    checkStatus();

    mLogger->debug("The pool plugin '%' is allocating a reader of the group reference '%'\n",
                   getName(),
                   readerGroupReference);

    Assert::getInstance().notEmpty(readerGroupReference, "readerGroupReference");

    std::shared_ptr<ReaderSpi> readerSpi = nullptr;

    try {
        readerSpi = mPoolPluginSpi->allocateReader(readerGroupReference);
    } catch (const PluginIOException& e) {
        throw KeyplePluginException("The pool plugin '" +
                                    getName() +
                                    "' is unable to allocate a reader of the reader group " +
                                    "reference " +
                                    readerGroupReference +
                                    " : " +
                                    e.getMessage(),
                                    std::make_shared<PluginIOException>(e));
    }

    std::shared_ptr<LocalReaderAdapter> localReaderAdapter = buildLocalReaderAdapter(readerSpi);
    getReadersMap().insert({localReaderAdapter->getName(), localReaderAdapter});
    localReaderAdapter->doRegister();

    return localReaderAdapter;
}

void LocalPoolPluginAdapter::releaseReader(std::shared_ptr<Reader> reader)
{
    checkStatus();

    mLogger->debug("The pool plugin '%' is releasing the reader '%'\n",
                   getName(),
                   reader != nullptr ? reader->getName() : "null");

    Assert::getInstance().notNull(reader, "reader");

    try {
        mPoolPluginSpi->releaseReader(
            std::dynamic_pointer_cast<LocalReaderAdapter>(reader)->getReaderSpi());

        /* Java 'finally' code moved here */
        getReadersMap().erase(reader->getName());
        std::dynamic_pointer_cast<LocalReaderAdapter>(reader)->doUnregister();
    } catch (const PluginIOException& e) {
        /* Java 'finally' code moved here */
        getReadersMap().erase(reader->getName());
        std::dynamic_pointer_cast<LocalReaderAdapter>(reader)->doUnregister();

        throw KeyplePluginException("The pool plugin '" +
                                    getName() +
                                    "' is unable to release the reader '" +
                                    reader->getName() +
                                    "' : " +
                                    e.getMessage(),
                                    std::make_shared<PluginIOException>(e));
    }
}

}
}
}
