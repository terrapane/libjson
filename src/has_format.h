/*
 *  has_format.h
 *
 *  Copyright (C) 2025, 2026
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

#include <version>

// Check to see if <format> is supported
#if defined(__cpp_lib_format)

    // XCode reports that std::format support exists in OS X 10.3 or later
    #ifdef  __APPLE__

        #if defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__) && \
            (__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__ >= 130300)

            #define TERRA_HAS_FORMAT 1 // NOLINT(cppcoreguidelines-macro-usage)

        #endif

    #else // Not __APPLE__

        #define TERRA_HAS_FORMAT 1 // NOLINT(cppcoreguidelines-macro-usage)

    #endif // defined(__APPLE__)

#endif // defined(__cpp_lib_format)
