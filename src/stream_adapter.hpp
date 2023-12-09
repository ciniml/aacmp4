// SPDX-License-Identifier: BSL-1.0
// Copyright Kenta Ida 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstdint>
#include "primitive_types.hpp"

namespace AACMP4 {
    template<typename T>
    struct StreamAdapter {
        T& stream;
        StreamAdapter(T& stream) : stream(stream) {}

        void write(const u8* data, std::size_t size) {
            this->stream.write(reinterpret_cast<const char*>(data), size);
        }
        std::size_t position(void) {
            return this->stream.tellp();
        }
    };
} // namespace AACMP4