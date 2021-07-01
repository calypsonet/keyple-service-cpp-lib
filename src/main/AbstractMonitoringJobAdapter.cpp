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

#include "AbstractMonitoringJobAdapter.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::service;

AbstractMonitoringJobAdapter::AbstractMonitoringJobAdapter(
  const std::shared_ptr<ObservableLocalReaderAdapter> reader)
: mReader(reader) {}

std::shared_ptr<ObservableLocalReaderAdapter> AbstractMonitoringJobAdapter::getReader()
{
    return mReader;
}

}
}
}
