/*
 *  json_exception.h
 *
 *  Copyright (C) 2024, 2025
 *  Terrapane Corporation
 *  All Rights Reserved
 *
 *  Author:
 *      Paul E. Jones <paulej@packetizer.com>
 *
 *  Description:
 *      This file defines an exception type that will be used by various JSON
 *      type objects, JSONParser, and JSONFormatter objects.
 *
 *  Portability Issues:
 *      Requires C++20 or later.
 */

#include <stdexcept>

#pragma once

namespace Terra::JSON
{

// Define an exception to throw when an exception occurs in the JSON libary
class JSONException : public std::runtime_error
{
    using std::runtime_error::runtime_error;
};

} //namespace Terra::JSON
