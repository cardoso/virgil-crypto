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

#ifndef VIRGIL_CRYPTO_VIRGIL_RANDOM_H
#define VIRGIL_CRYPTO_VIRGIL_RANDOM_H

#include <virgil/crypto/VirgilByteArray.h>

#include <memory>

namespace virgil { namespace crypto { inline namespace primitive {

class VirgilOperationRandom {
private:
    template<class Random>
    struct Model;

public:
    template<class Random>
    VirgilOperationRandom(Random random) : self_(new Model<Random>(std::move(random))) {}

    VirgilByteArray randomize(size_t bytesNum) {
        return self_->doRandomize(bytesNum);
    }

    VirgilOperationRandom(const VirgilOperationRandom& other) : self_(other.self_->doCopy()) {}

    VirgilOperationRandom(VirgilOperationRandom&& other) noexcept = default;

    VirgilOperationRandom& operator=(const VirgilOperationRandom& other) {
        VirgilOperationRandom tmp(other);
        *this = std::move(tmp);
        return *this;
    }

    VirgilOperationRandom& operator=(VirgilOperationRandom&& other) noexcept = default;

    ~VirgilOperationRandom() noexcept = default;

    static VirgilOperationRandom getDefault();

private:
    struct Concept {
        virtual VirgilByteArray doRandomize(size_t bytesNum) = 0;

        virtual Concept* doCopy() const = 0;
    };

    template<class Random>
    struct Model : Concept {
        explicit Model(Random randomImpl) : impl_(std::move(randomImpl)) {}

        VirgilByteArray doRandomize(size_t bytesNum) override {
            return impl_.randomize(bytesNum);
        }

        Concept* doCopy() const override {
            return new Model(*this);
        }

    private:
        Random impl_;
    };

private:
    std::unique_ptr<Concept> self_;
};

}}}

#endif //VIRGIL_CRYPTO_VIRGIL_RANDOM_H
