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

#include "ApduResponseAdapter.h"

/* Keyple Core Util */
#include "Arrays.h"
#include "KeypleStd.h"

namespace keyple {
namespace core {
namespace service {

using namespace keyple::core::util::cpp;

ApduResponseAdapter::ApduResponseAdapter(const std::vector<uint8_t>& apdu)
: mApdu(apdu),
  mStatusWord(((apdu[apdu.size() - 2] & 0x000000FF) << 8) + (apdu[apdu.size() - 1] & 0x000000FF)) {}

const std::vector<uint8_t>& ApduResponseAdapter::getApdu() const
{
	return mApdu;
}

const std::vector<uint8_t> ApduResponseAdapter::getDataOut() const
{
	return Arrays::copyOfRange(mApdu, 0, static_cast<int>(mApdu.size()) - 2);
}

int ApduResponseAdapter::getStatusWord() const
{
    return mStatusWord;
}

std::ostream& operator<<(std::ostream& os, const ApduResponseAdapter& ara)
{
	os << "APDU_RESPONSE_ADAPTER: {"
       << "APDU = " << ara.mApdu << ", "
	   << "STATUS_WORD = " << ara.mStatusWord
	   << "}";

	return os;
}

std::ostream& operator<<(std::ostream& os, const std::shared_ptr<ApduResponseAdapter> ara)
{
	os << *ara.get();

	return os;
}

}
}
}
