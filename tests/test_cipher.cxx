/**
 * Copyright (C) 2015 Virgil Security Inc.
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

/**
 * @file test_cipher.cxx
 * @brief Covers class VirgilCipher
 */

#include "catch.hpp"

#include <virgil/crypto/VirgilByteArray.h>
#include <virgil/crypto/VirgilCipher.h>
#include <virgil/crypto/VirgilKeyPair.h>

using virgil::crypto::str2bytes;
using virgil::crypto::bytes2hex;
using virgil::crypto::bytes2str;
using virgil::crypto::VirgilByteArray;
using virgil::crypto::VirgilCipher;
using virgil::crypto::VirgilKeyPair;

TEST_CASE("encrypt and decrypt with generated keys", "[cipher]") {
    VirgilByteArray password = str2bytes("password");
    VirgilByteArray testData = str2bytes("this string will be encrypted");
    VirgilByteArray recipientId = str2bytes("2e8176ba-34db-4c65-b977-c5eac687c4ac");
    VirgilKeyPair keyPair(password);

    VirgilCipher cipher;
    cipher.addKeyRecipient(recipientId, keyPair.publicKey());

    SECTION("and embedded content info") {
        VirgilByteArray encryptedData = cipher.encrypt(testData, true);
        VirgilByteArray decryptedData;
        REQUIRE_THROWS(
                decryptedData = cipher.decryptWithKey(encryptedData, recipientId, keyPair.privateKey())
        );
        REQUIRE_NOTHROW(
                decryptedData = cipher.decryptWithKey(encryptedData, recipientId, keyPair.privateKey(), password)
        );
        REQUIRE(testData == decryptedData);
    }

    SECTION("and embedded content info with custom parameters") {
        VirgilByteArray intParamKey = str2bytes("int_parameter_key");
        const int intParamValue = 35777;
        VirgilByteArray strParamKey = str2bytes("string_parameter_key");
        VirgilByteArray strParamValue = str2bytes("string parameter");
        VirgilByteArray hexParamKey = str2bytes("data_param_value");
        VirgilByteArray hexParamValue = str2bytes("will be stored as octet string");

        cipher.customParams().setInteger(intParamKey, intParamValue);
        cipher.customParams().setString(strParamKey, strParamValue);
        cipher.customParams().setData(hexParamKey, hexParamValue);

        VirgilByteArray encryptedData = cipher.encrypt(testData, true);
        cipher.removeAllRecipients();
        cipher.customParams().clear();

        VirgilByteArray decryptedData;
        REQUIRE_NOTHROW(
                decryptedData = cipher.decryptWithKey(encryptedData, recipientId, keyPair.privateKey(), password)
        );
        REQUIRE(testData == decryptedData);

        REQUIRE(cipher.customParams().getInteger(intParamKey) == intParamValue);
        REQUIRE(cipher.customParams().getString(strParamKey) == strParamValue);
        REQUIRE(cipher.customParams().getData(hexParamKey) == hexParamValue);
    }

    SECTION("and separated content info") {
        VirgilByteArray encryptedData = cipher.encrypt(testData, false);
        VirgilByteArray contentInfo = cipher.getContentInfo();
        REQUIRE(contentInfo.size() > 0);

        REQUIRE_NOTHROW(
                cipher.setContentInfo(contentInfo)
        );

        VirgilByteArray decryptedData;
        REQUIRE_THROWS(
                decryptedData = cipher.decryptWithKey(encryptedData, recipientId, keyPair.privateKey())
        );
        REQUIRE_NOTHROW(
                decryptedData = cipher.decryptWithKey(encryptedData, recipientId, keyPair.privateKey(), password)
        );
        REQUIRE(testData == decryptedData);
    }

    SECTION("and separated content info with custom parameters") {
        VirgilByteArray intParamKey = str2bytes("int_parameter_key");
        const int intParamValue = 35777;
        VirgilByteArray strParamKey = str2bytes("string_parameter_key");
        VirgilByteArray strParamValue = str2bytes("string parameter");
        VirgilByteArray hexParamKey = str2bytes("data_param_value");
        VirgilByteArray hexParamValue = str2bytes("will be stored as octet string");

        cipher.customParams().setInteger(intParamKey, intParamValue);
        cipher.customParams().setString(strParamKey, strParamValue);
        cipher.customParams().setData(hexParamKey, hexParamValue);

        VirgilByteArray encryptedData = cipher.encrypt(testData, false);
        VirgilByteArray contentInfo = cipher.getContentInfo();
        REQUIRE(contentInfo.size() > 0);

        REQUIRE_NOTHROW(
                cipher.setContentInfo(contentInfo)
        );

        VirgilByteArray decryptedData;
        REQUIRE_NOTHROW(
                decryptedData = cipher.decryptWithKey(encryptedData, recipientId, keyPair.privateKey(), password)
        );
        REQUIRE(testData == decryptedData);

        REQUIRE(cipher.customParams().getInteger(intParamKey) == intParamValue);
        REQUIRE(cipher.customParams().getString(strParamKey) == strParamValue);
        REQUIRE(cipher.customParams().getData(hexParamKey) == hexParamValue);
    }
}

TEST_CASE("generated keys", "[cipher]") {
    VirgilByteArray testData = str2bytes("this string will be encrypted");
    VirgilByteArray bobId = str2bytes("2e8176ba-34db-4c65-b977-c5eac687c4ac");
    VirgilByteArray johnId = str2bytes("968dc52d-2045-4abe-ab51-0b04737cac76");
    VirgilKeyPair bobKeyPair;
    VirgilKeyPair johnKeyPair;
    VirgilByteArray alicePassword = str2bytes("alice secret");

    SECTION("encrypt for multiple recipients") {
        VirgilByteArray encryptedData;

        VirgilCipher cipher;
        cipher.addKeyRecipient(bobId, bobKeyPair.publicKey());
        cipher.addKeyRecipient(johnId, johnKeyPair.publicKey());
        cipher.addPasswordRecipient(alicePassword);
        encryptedData = cipher.encrypt(testData, true);

        SECTION("decrypt for Bob") {
            VirgilCipher cipher;
            VirgilByteArray decryptedData = cipher.decryptWithKey(encryptedData, bobId, bobKeyPair.privateKey());
            REQUIRE(testData == decryptedData);
        }

        SECTION("decrypt for John") {
            VirgilCipher cipher;
            VirgilByteArray decryptedData = cipher.decryptWithKey(encryptedData, johnId, johnKeyPair.privateKey());
            REQUIRE(testData == decryptedData);
        }

        SECTION("decrypt for Alice") {
            VirgilCipher cipher;
            VirgilByteArray decryptedData = cipher.decryptWithPassword(encryptedData, alicePassword);
            REQUIRE(testData == decryptedData);
        }
    }
}

TEST_CASE("encrypt and decrypt with password", "[cipher]") {
    VirgilByteArray password = str2bytes("password");
    VirgilByteArray wrongPassword = str2bytes("wrong password");
    VirgilByteArray testData = str2bytes("this string will be encrypted");

    VirgilCipher cipher;
    cipher.addPasswordRecipient(password);

    SECTION("and embedded content info") {
        VirgilByteArray encryptedData = cipher.encrypt(testData, true);
        VirgilByteArray decryptedData;
        REQUIRE_THROWS(
                decryptedData = cipher.decryptWithPassword(encryptedData, wrongPassword)
        );
        REQUIRE_NOTHROW(
                decryptedData = cipher.decryptWithPassword(encryptedData, password)
        );
        REQUIRE(testData == decryptedData);
    }
}

TEST_CASE("encrypt and decrypt RSA-3072", "[cipher-rsa]") {
    VirgilByteArray password = str2bytes("password");
    VirgilByteArray testData = str2bytes("this string will be encrypted");
    VirgilByteArray recipientId = str2bytes("2e8176ba-34db-4c65-b977-c5eac687c4ac");
    VirgilKeyPair keyPair = VirgilKeyPair::generate(VirgilKeyPair::Type_RSA_3072, password);

    VirgilCipher cipher;
    cipher.addKeyRecipient(recipientId, keyPair.publicKey());

    SECTION("and embedded content info") {
        VirgilByteArray encryptedData = cipher.encrypt(testData, true);
        VirgilByteArray decryptedData;
        REQUIRE_THROWS(
                decryptedData = cipher.decryptWithKey(encryptedData, recipientId, keyPair.privateKey())
        );
        REQUIRE_NOTHROW(
                decryptedData = cipher.decryptWithKey(encryptedData, recipientId, keyPair.privateKey(), password)
        );
        REQUIRE(testData == decryptedData);
    }
}

TEST_CASE("encrypt and decrypt RSA-8192", "[cipher-rsa]") {
    VirgilByteArray password = str2bytes("password");
    VirgilByteArray testData = str2bytes("this string will be encrypted");
    VirgilByteArray recipientId = str2bytes("2e8176ba-34db-4c65-b977-c5eac687c4ac");
    VirgilKeyPair keyPair = VirgilKeyPair::generate(VirgilKeyPair::Type_RSA_8192, password);

    VirgilCipher cipher;
    cipher.addKeyRecipient(recipientId, keyPair.publicKey());

    SECTION("and embedded content info") {
        VirgilByteArray encryptedData = cipher.encrypt(testData, true);
        VirgilByteArray decryptedData;
        decryptedData = cipher.decryptWithKey(encryptedData, recipientId, keyPair.privateKey(), password);
        REQUIRE(testData == decryptedData);
    }
}

TEST_CASE("encrypt and decrypt curve25519", "[cipher]") {
    VirgilByteArray password = str2bytes("password");
    VirgilByteArray testData = str2bytes("this string will be encrypted");
    VirgilByteArray recipientId = str2bytes("2e8176ba-34db-4c65-b977-c5eac687c4ac");
    VirgilKeyPair keyPair = VirgilKeyPair::generate(VirgilKeyPair::Type_EC_M255);

    VirgilCipher cipher;
    cipher.addKeyRecipient(recipientId, keyPair.publicKey());

    SECTION("and embedded content info") {
        VirgilByteArray encryptedData = cipher.encrypt(testData, true);
        VirgilByteArray decryptedData;
        decryptedData = cipher.decryptWithKey(encryptedData, recipientId, keyPair.privateKey(), password);
        REQUIRE(testData == decryptedData);
    }
}

TEST_CASE("check recipient existance", "[cipher]") {
    VirgilByteArray bobId = str2bytes("2e8176ba-34db-4c65-b977-c5eac687c4ac");
    VirgilByteArray johnId = str2bytes("968dc52d-2045-4abe-ab51-0b04737cac76");
    VirgilByteArray aliceId = str2bytes("99e435e7-2527-4a5a-89bb-37927bdb337b");
    VirgilKeyPair bobKeyPair;
    VirgilKeyPair johnKeyPair;

    VirgilCipher cipher;
    cipher.addKeyRecipient(bobId, bobKeyPair.publicKey());
    cipher.addKeyRecipient(johnId, johnKeyPair.publicKey());

    SECTION("within local context") {
        REQUIRE(cipher.keyRecipientExists(bobId));
        REQUIRE(cipher.keyRecipientExists(johnId));
        REQUIRE_FALSE(cipher.keyRecipientExists(aliceId));
    }

    SECTION("ContentInfo context") {
        (void) cipher.encrypt(VirgilByteArray());

        VirgilCipher restoredCipher;
        restoredCipher.setContentInfo(cipher.getContentInfo());

        REQUIRE(restoredCipher.keyRecipientExists(bobId));
        REQUIRE(restoredCipher.keyRecipientExists(johnId));
        REQUIRE_FALSE(restoredCipher.keyRecipientExists(aliceId));
    }
}