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

#include <cstdint>
#include <memory>
#include <ostream>
#include <vector>

/* Calypsonet Terminal Card */
#include "ApduResponseApi.h"

namespace keyple {
namespace core {
namespace service {

using namespace calypsonet::terminal::card;

/**
 * (package-private)<br>
 * This POJO contains a set of data related to an ISO-7816 APDU response.
 *
 * @since 2.0.0
 */
class ApduResponseAdapter final : public ApduResponseApi {
public:
    /**
     * (package-private)<br>
     * Builds an APDU response from an array of bytes from the card, computes the status word.
     *
     * @param apdu A array of at least 2 bytes.
     * @since 2.0.0
     */
    ApduResponseAdapter(const std::vector<uint8_t>& apdu);

    /**
     * 
     */
    virtual ~ApduResponseAdapter() = default;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual const std::vector<uint8_t>& getApdu() const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual const std::vector<uint8_t> getDataOut() const override;

    /**
     * {@inheritDoc}
     *
     * @since 2.0.0
     */
    virtual int getStatusWord() const override;

    /**
     *
     */
    friend std::ostream& operator<<(std::ostream& os, const ApduResponseAdapter& ara);

    /**
     *
     */
    friend std::ostream& operator<<(std::ostream& os, const std::shared_ptr<ApduResponseAdapter> ara);

private:
    /**
     *
     */
    const std::vector<uint8_t> mApdu;

    /**
     *
     */
    const int mStatusWord;
};

}
}
}
