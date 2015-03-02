/**
 * Copyright (C) 2014 Virgil Security Inc.
 *
 * Lead Maintainer: Virgil Security Inc. <support@virgilsecurity.com>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     (1) Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *     (2) Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *
 *     (3) Neither the name of the copyright holder nor the names of its
 *     contributors may be used to endorse or promote products derived from
 *     this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef VIRGIL_DATA_VIRGIL_CERTIFICATE_ID_H
#define VIRGIL_DATA_VIRGIL_CERTIFICATE_ID_H

#include <virgil/service/data/VirgilAccountId.h>
using virgil::service::data::VirgilAccountId;

#include <virgil/VirgilByteArray.h>
using virgil::VirgilByteArray;

namespace virgil { namespace service { namespace data {

/**
 * @brief This class handle unique identifier of Virgil Service certificate.
 *
 * This class thru inheritance contains certificate identifier as well as account identifier.
 */
class VirgilCertificateId : public VirgilAccountId {
public:
    /**
     * @name certificateId accessors
     * @brief Provides getter and setter for Virgil Service certificate identifier.
     */
    ///@{
    VirgilByteArray certificateId() const;
    void setCertificateId(const VirgilByteArray& certificateId);
    ///@}
    /**
     * @name ID management
     */
    ///@{
    /**
     * @brief Returns true if certificate id is not specified or one of the parent ids are not specified.
     */
    virtual bool isEmpty() const;
    /**
     * @brief Clears certificate id and all parent ids.
     */
    virtual void clear();
    ///@}
    /**
     * @name VirgilAsn1Compatible implementation
     *
     * Marshalling format:
     *     certfificateId UTF8String
     */
    ///@{
    virtual size_t asn1Write(VirgilAsn1Writer& asn1Writer, size_t childWrittenBytes = 0) const;
    virtual void asn1Read(VirgilAsn1Reader& asn1Reader);
    ///@}
    /**
     * @name VirgilJsonCompatible implementation
     *
     * Marshalling format:
     *     {
     *         "certificateId" : "UTF8String"
     *     }
     */
    ///@{
    virtual Json::Value jsonWrite(Json::Value& childObject) const;
    virtual Json::Value jsonRead(const Json::Value& parentValue);
    ///@}
    /**
     * @brief Polymorphic destructor.
     */
    virtual ~VirgilCertificateId() throw() {};
private:
    VirgilByteArray certificateId_;
};

}}}

#endif /* VIRGIL_DATA_VIRGIL_CERTIFICATE_ID_H */
