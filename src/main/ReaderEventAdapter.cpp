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

#include "ReaderEventAdapter.h"

namespace keyple {
namespace core {
namespace service {

ReaderEventAdapter::ReaderEventAdapter(
  const std::string& pluginName,
  const std::string& readerName,
  const Type type,
  std::shared_ptr<ScheduledCardSelectionsResponse> scheduledCardSelectionsResponse)
: mPluginName(pluginName),
  mReaderName(readerName),
  mScheduledCardSelectionsResponse(scheduledCardSelectionsResponse),
  mType(type) {}

const std::string& ReaderEventAdapter::getPluginName() const
{
    return mPluginName;
}

const std::string& ReaderEventAdapter::getReaderName() const
{
    return mReaderName;
}

Type ReaderEventAdapter::getType() const
{
    return mType;
}

const std::shared_ptr<ScheduledCardSelectionsResponse>
    ReaderEventAdapter::getScheduledCardSelectionsResponse() const
{
    return mScheduledCardSelectionsResponse;
}

}
}
}
