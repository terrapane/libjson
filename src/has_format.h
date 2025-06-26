/*
 *  has_format.h
 *
 *  Copyright (C) 2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This header file will facilitate the selection of using std::format.
 *      While this should be trivial, support is inconsistent.  This file
 *      aims to address any platform-specific issues, while striving to honor
 *      advertised C++ features.
 *
 *  Portability Issues:
 *      None.
 */

#pragma once

// Check to see if the <format> header exists
#if defined(__has_include) && __has_include(<format>)

    #include <format>

    // XCode reports that std::format with std::to_char support exists in
    // OS X 10.3 or later; XCode 16.4 does not set the C++ feature definitions
    #if defined(__APPLE__)

        #if defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
            (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 130300)

            #define TERRA_HAS_FORMAT 1

        #endif

    #else // defined(__APPLE__)

        #if defined(__cpp_lib_format) && defined(__cpp_lib_to_chars)

            #define TERRA_HAS_FORMAT 1

        #endif

    #endif // defined(__APPLE__)

#endif // defined(__has_include) && __has_include(<format>)
