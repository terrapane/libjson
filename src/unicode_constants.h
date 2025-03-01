/*
 *  unicode_constants.h
 *
 *  Copyright (C) 2024-2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file defines various Unicode constants used in parsing JSON
 *      or producing JSON text.
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

#include <cstdint>

namespace Terra::JSON::Unicode
{

// Largest ASCII value
constexpr std::uint32_t Maximum_ASCII_Value = 0x7f;

// Largest valid Unicode character
constexpr std::uint32_t Maximum_Character_Value = 0x10ffff;

// Maximum unicode character in the BMP
constexpr std::uint32_t Maximum_BMP_Value = 0xffff;

// Surrogates are within the following range
// (See: https://en.wikipedia.org/wiki/UTF-16#U+D800_to_U+DFFF_(surrogates))
constexpr std::uint32_t Surrogate_High_Min =  0xd800;
constexpr std::uint32_t Surrogate_High_Max =  0xdbff;
constexpr std::uint32_t Surrogate_Low_Min =  0xdc00;
constexpr std::uint32_t Surrogate_Low_Max =  0xdfff;

// Values used in parsing or creating surrogate pairs
// (See: https://www.Unicode.org/faq/utf_bom.html#utf16-3)
constexpr std::uint32_t Lead_Offset = 0xd800 - (0x10000 >> 10);
constexpr std::uint32_t Surrogate_Offset = 0x10000 - (0xd800 << 10) - 0xdc00;

} // namespace Terra::JSON::Unicode
