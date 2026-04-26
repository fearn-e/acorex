/*
The MIT License (MIT)

Copyright (c) 2026-2026 Elowyn Fearne

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include <string>

#define ACOREX_MAJOR_VERSION 2
#define ACOREX_MINOR_VERSION 0
#define ACOREX_PATCH_VERSION 0

#ifdef _ACOREX_DEVELOPMENT_BUILD
    #define ACOREX_VERSION_STATUS "development_build"
#elif _ACOREX_DEBUG
    #define ACOREX_VERSION_STATUS "debug_build"
#endif

namespace Acorex {

struct VersionString {
private:
    std::string versionString = "";

public:
    void ComputeVersionString ( );
    
    const std::string& GetVersionString ( ) const { return versionString; }
};

} // Acorex

// TODO - the date/time doesn't update with each compilation, because the Versioning.cpp file doesn't count as having changed - force recompile of that file somehow