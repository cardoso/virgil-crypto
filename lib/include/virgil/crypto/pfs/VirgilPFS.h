/**
 * Copyright (C) 2015-2017 Virgil Security Inc.
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

#ifndef VIRGIL_CRYPTO_PFS_VIRGIL_PFS_H
#define VIRGIL_CRYPTO_PFS_VIRGIL_PFS_H

#include <virgil/crypto/VirgilByteArray.h>

#include <virgil/crypto/pfs/VirgilPFSSession.h>
#include <virgil/crypto/pfs/VirgilPFSEncryptedMessage.h>
#include <virgil/crypto/pfs/VirgilPFSInitiatorPublicInfo.h>
#include <virgil/crypto/pfs/VirgilPFSInitiatorPrivateInfo.h>
#include <virgil/crypto/pfs/VirgilPFSResponderPublicInfo.h>
#include <virgil/crypto/pfs/VirgilPFSResponderPrivateInfo.h>

#include <virgil/crypto/primitive/VirgilOperationRandom.h>
#include <virgil/crypto/primitive/VirgilOperationHash.h>
#include <virgil/crypto/primitive/VirgilOperationDH.h>
#include <virgil/crypto/primitive/VirgilOperationKDF.h>
#include <virgil/crypto/primitive/VirgilOperationCipher.h>

namespace virgil { namespace crypto { namespace pfs {

/**
 * @brief This is the main entry for the all Perfect Forward Secrecy (PFS) Modules.
 *
 * @ingroup pfs
 */
class VirgilPFS {
public:
    /**
     * @brief Configures PFS module with default underlying algorithms.
     *
     * Each underlying algorithm can be changed with correspond setter.
     */
    VirgilPFS();

    /**
     * @brief Start session from the Initiator side.
     * @param initiatorPrivateInfo - initiator private keys and related information.
     * @param responderPublicInfo - responder public keys and related information.
     * @return Created session (can be ignored).
     * @note Function has side effect: created session is stored in the object state.
     */
    VirgilPFSSession startInitiatorSession(
            const VirgilPFSInitiatorPrivateInfo& initiatorPrivateInfo,
            const VirgilPFSResponderPublicInfo& responderPublicInfo);

    /**
     * @brief Start session from the Responder side.
     * @param responderPrivateInfo - responder private keys and related information.
     * @param initiatorPublicInfo - initiator public keys and related information.
     * @return Created session (can be ignored).
     * @note Function has side effect: created session is stored in the object state.
     */
    VirgilPFSSession startResponderSession(
            const VirgilPFSResponderPrivateInfo& responderPrivateInfo,
            const VirgilPFSInitiatorPublicInfo& initiatorPublicInfo);

    /**
     * @brief Encrypt given data.
     * @param data - data to be encrypted.
     * @return Encrypted message.
     * @note Stored session is used for encryption.
     * @note Function has side effect: random_ internal state is changed.
     */
    VirgilPFSEncryptedMessage encrypt(const VirgilByteArray& data);

    /**
     * @brief Decrypt given message.
     * @param encryptedMessage - message to be decrypted.
     * @return Plain text.
     * @note Stored session is used for decryption.
     */
    VirgilByteArray decrypt(const VirgilPFSEncryptedMessage& encryptedMessage) const;

    /**
     * @brief Set custom implementation for algorithm: random.
     * @param random - new algorithm implementation.
     */
    void setRandom(VirgilOperationRandom random);

    /**
     * @brief Set custom implementation for algorithm: hash.
     * @param hash - new algorithm implementation.
     */
    void setHash(VirgilOperationHash hash);

    /**
     * @brief Set custom implementation for algorithm: Diffie–Hellman.
     * @param dh - new algorithm implementation.
     */
    void setDH(VirgilOperationDH dh);

    /**
     * @brief Set custom implementation for algorithm: Key Dervation Function.
     * @param kdf - new algorithm implementation.
     */
    void setKDF(VirgilOperationKDF kdf);

    /**
     * @brief Set custom implementation for algorithm: Symmetric Cipher.
     * @param cipher - new algorithm implementation.
     */
    void setCipher(VirgilOperationCipher cipher);

    /**
     * @brief Return current session.
     * @return Session.
     */
    VirgilPFSSession getSession() const;

    /**
     * @brief Set new session.
     * @param session - new session.
     */
    void setSession(VirgilPFSSession session);

private:
    VirgilByteArray calculateSharedKey(
            const VirgilPFSInitiatorPrivateInfo& initiatorPrivateInfo,
            const VirgilPFSResponderPublicInfo& responderPublicInfo) const;

    VirgilByteArray calculateSharedKey(
            const VirgilPFSResponderPrivateInfo& responderPrivateInfo,
            const VirgilPFSInitiatorPublicInfo& initiatorPublicInfo) const;

    VirgilByteArray calculateSecretKey(const VirgilByteArray& keyMaterial, size_t size);

    VirgilByteArray calculateAdditionalData(
            const std::string& initiatorIdentifier,
            const std::string& responderIdentifier) const;

    VirgilByteArray calculateSessionIdentifier(
            const VirgilByteArray& secretKey, const VirgilByteArray& additionalData) const;

private:
    VirgilOperationRandom random_;
    VirgilOperationHash hash_;
    VirgilOperationDH dh_;
    VirgilOperationKDF kdf_;
    VirgilOperationCipher cipher_;
    VirgilPFSSession session_;
};

}}}

#endif //VIRGIL_CRYPTO_PFS_VIRGIL_PFS_H
