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

#include <Versioning.h>

using namespace Acorex;

void VersionString::ComputeVersionString ( )
{
    versionString = "v" + std::to_string ( ACOREX_MAJOR_VERSION ) + "." + std::to_string ( ACOREX_MINOR_VERSION ) + "." + std::to_string ( ACOREX_PATCH_VERSION );
#ifdef _ACOREX_STABLE_BUILD
    //
#else
    std::string compileYear = std::string ( __DATE__ + 7, 4 );
    std::string compileDay = std::string ( __DATE__ + 4, 2 );
    if ( compileDay[0] == ' ' ) { compileDay[0] = '0'; }
    std::string compileMonth = "";
    switch ( __DATE__[0] )
    {
    case 'J':
        if ( __DATE__[1] == 'a' && __DATE__[2] == 'n' ) compileMonth = "01";
        else if ( __DATE__[1] == 'u' && __DATE__[2] == 'n' ) compileMonth = "06";
        else if ( __DATE__[1] == 'u' && __DATE__[2] == 'l' ) compileMonth = "07";
        break;
    case 'F': compileMonth = "02"; break;
    case 'M':
        if ( __DATE__[1] == 'a' && __DATE__[2] == 'r' ) compileMonth = "03";
        else if ( __DATE__[1] == 'a' && __DATE__[2] == 'y' ) compileMonth = "05";
        break;
    case 'A':
        if ( __DATE__[1] == 'p' && __DATE__[2] == 'r' ) compileMonth = "04";
        else if ( __DATE__[1] == 'u' && __DATE__[2] == 'g' ) compileMonth = "08";
        break;
    case 'S': compileMonth = "09"; break;
    case 'O': compileMonth = "10"; break;
    case 'N': compileMonth = "11"; break;
    case 'D': compileMonth = "12"; break;
    }
    std::string compileTime = __TIME__;

    std::string compileTimestamp = compileDay + "." + compileMonth + "." + compileYear + "_" + compileTime;

    versionString += "_" + compileTimestamp + "_" + ACOREX_VERSION_STATUS;
#endif
}
