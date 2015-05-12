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

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <string>
#include <stdexcept>

#include <virgil/VirgilByteArray.h>
using virgil::VirgilByteArray;

#include <virgil/VirgilException.h>
using virgil::VirgilException;

#include <virgil/service/data/VirgilCertificate.h>
using virgil::service::data::VirgilCertificate;

#include <virgil/service/VirgilStreamCipher.h>
using virgil::service::VirgilStreamCipher;

#include <virgil/stream/VirgilStreamDataSource.h>
using virgil::stream::VirgilStreamDataSource;

#include <virgil/stream/VirgilStreamDataSink.h>
using virgil::stream::VirgilStreamDataSink;

#include <virgil/stream/utils.h>

#include <tclap/CmdLine.h>

#include "version.h"

#ifdef SPLIT_CLI
    #define MAIN main
#else
    #define MAIN decrypt_main
#endif

int MAIN(int argc, char **argv) {
    try {
        // Parse arguments.
        TCLAP::CmdLine cmd("Decrypt data", ' ', cli_version());

        TCLAP::ValueArg<std::string> inArg("i", "in", "Data to be decrypted. If omitted stdin is used.",
                false, "", "file");

        TCLAP::ValueArg<std::string> outArg("o", "out", "Decrypted data. If omitted stdout is used.",
                false, "", "file");

        TCLAP::ValueArg<std::string> contentInfoArg("c", "content-info",
                "Content info. Use this option if content info is not embedded in the encrypted data.",
                false, "", "file");

        TCLAP::ValueArg<std::string> keyArg("k", "key", "Recipient's private key.",
                false, "", "file");

        TCLAP::ValueArg<std::string> pwdArg("p", "pwd", "Private key password.",
                false, "", "arg");

        TCLAP::ValueArg<std::string> recipientArg("r", "recipient",
                "If option -key is defined this value is used as recipient's certificate, "
                "otherwise this value is used as recipient's password.",
                true, "", "arg");


        cmd.add(recipientArg);
        cmd.add(pwdArg);
        cmd.add(keyArg);
        cmd.add(contentInfoArg);
        cmd.add(outArg);
        cmd.add(inArg);

        cmd.parse(argc, argv);

        // Prepare input.
        std::istream *inStream = &std::cin;
        std::ifstream inFile(inArg.getValue().c_str(), std::ios::in | std::ios::binary);
        if (inFile.good()) {
            inStream = &inFile;
        } else if (!inArg.getValue().empty()) {
            throw std::invalid_argument(std::string("can not read file: " + inArg.getValue()));
        }
        VirgilStreamDataSource dataSource(*inStream);

        // Prepare output.
        std::ostream *outStream = &std::cout;
        std::ofstream outFile(outArg.getValue().c_str(), std::ios::out | std::ios::binary);
        if (outFile.good()) {
            outStream = &outFile;
        } else if (!outArg.getValue().empty()) {
            throw std::invalid_argument(std::string("can not write file: " + outArg.getValue()));
        }
        VirgilStreamDataSink dataSink(*outStream);

        // Create cipher.
        VirgilStreamCipher cipher;

        // Set content info.
        std::ifstream contentInfoFile(contentInfoArg.getValue().c_str(), std::ios::in | std::ios::binary);
        if (contentInfoFile.good()) {
            VirgilByteArray contentInfo;
            std::copy(std::istreambuf_iterator<char>(contentInfoFile), std::istreambuf_iterator<char>(),
                    std::back_inserter(contentInfo));
            cipher.setContentInfo(contentInfo);
        } else if (!contentInfoArg.getValue().empty()) {
            throw std::invalid_argument(std::string("can not read file: " + contentInfoArg.getValue()));
        }

        if (!keyArg.getValue().empty()) {
            // Read certificate
            VirgilCertificate certificate = virgil::stream::read_certificate(recipientArg.getValue());
            // Read private key
            std::ifstream keyFile(keyArg.getValue().c_str(), std::ios::in | std::ios::binary);
            if (!keyFile.good() && !keyArg.getValue().empty()) {
                throw std::invalid_argument(std::string("can not read file: " + keyArg.getValue()));
            }
            VirgilByteArray privateKey;
            std::copy(std::istreambuf_iterator<char>(keyFile), std::istreambuf_iterator<char>(),
                    std::back_inserter(privateKey));
            VirgilByteArray privateKeyPassword = virgil::str2bytes(pwdArg.getValue());
            // Decrypt
            cipher.decryptWithKey(dataSource, dataSink, certificate.id().certificateId(),
                    privateKey, privateKeyPassword);
        } else if (!recipientArg.getValue().empty()) {
            // Decrypt
            cipher.decryptWithPassword(dataSource, dataSink, virgil::str2bytes(recipientArg.getValue()));
        } else {
            throw std::invalid_argument(std::string("no recipients are defined"));
        }
    } catch (TCLAP::ArgException& exception) {
        std::cerr << "Error: " << exception.error() << " for arg " << exception.argId() << std::endl;
        return EXIT_FAILURE;
    } catch (std::exception& exception) {
        std::cerr << "Error: " << exception.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
