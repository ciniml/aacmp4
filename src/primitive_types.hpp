// SPDX-License-Identifier: BSL-1.0
// Copyright Kenta Ida 2023.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstdint>

namespace AACMP4 {
    typedef std::uint8_t u8;
    struct __attribute__((packed)) u16 {
        std::uint8_t octets[2];

        constexpr u16() : octets {0, 0} {}
        constexpr u16(std::uint16_t value) : octets {std::uint8_t(value >> 8), std::uint8_t(value)} {}
        constexpr operator std::uint16_t() const {
            return (static_cast<std::uint32_t>(this->octets[0]) <<  8)
                 | (static_cast<std::uint32_t>(this->octets[1]) <<  0)
                 ;
        }
    };
    struct __attribute__((packed)) u24 {
        std::uint8_t octets[3];
        constexpr u24() : octets {0, 0, 0}  {}
        constexpr u24(std::uint32_t value) : octets { std::uint8_t(value >> 16), std::uint8_t(value >> 8), std::uint8_t(value) } {}
        constexpr operator std::uint32_t() const {
            return (static_cast<std::uint32_t>(this->octets[0]) << 16)
                 | (static_cast<std::uint32_t>(this->octets[1]) <<  8)
                 | (static_cast<std::uint32_t>(this->octets[2]) <<  0)
                 ;
        }
    };
    struct __attribute__((packed)) u32 {
        std::uint8_t octets[4];
        constexpr u32() : octets {0, 0, 0, 0}  {}
        constexpr u32(std::uint32_t value) : octets{std::uint8_t(value >> 24), std::uint8_t(value >> 16), std::uint8_t(value >> 8), std::uint8_t(value)} {}
        constexpr operator std::uint32_t() const {
            return (static_cast<std::uint32_t>(this->octets[0]) << 24)
                 | (static_cast<std::uint32_t>(this->octets[1]) << 16)
                 | (static_cast<std::uint32_t>(this->octets[2]) <<  8)
                 | (static_cast<std::uint32_t>(this->octets[3]) <<  0)
                 ;
        }
    };
    struct __attribute__((packed)) u64 {
        std::uint8_t octets[8];
        constexpr u64() : octets {0, 0, 0, 0, 0, 0, 0, 0}  {}
        constexpr u64(std::uint64_t value) : octets{
            std::uint8_t(value >> 56), 
            std::uint8_t(value >> 48), 
            std::uint8_t(value >> 40), 
            std::uint8_t(value >> 32), 
            std::uint8_t(value >> 24), 
            std::uint8_t(value >> 16), 
            std::uint8_t(value >> 8), 
            std::uint8_t(value),
        } {}
        constexpr operator std::uint32_t() const {
            return (static_cast<std::uint32_t>(this->octets[0]) << 24)
                 | (static_cast<std::uint32_t>(this->octets[1]) << 16)
                 | (static_cast<std::uint32_t>(this->octets[2]) <<  8)
                 | (static_cast<std::uint32_t>(this->octets[3]) <<  0)
                 ;
        }
    };
} // namespace AACMP4
