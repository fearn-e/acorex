/*
The MIT License (MIT)

Copyright (c) 2024-2026 Elowyn Fearne

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Utilities/Log.h"

#include "Utilities/TemporaryDefaults.h"

using namespace Acorex;

// LogDisplay --------------------------------------------------------------------------

void Utilities::LogDisplay::Initialise ( )
{
    logs.clear ( );

    {
        std::lock_guard<std::mutex> lock ( newLogMutex );
        while ( !newLogs.empty ( ) ) { newLogs.pop ( ); }
    }

}

void Utilities::LogDisplay::Update ( )
{
    {
        std::lock_guard<std::mutex> lock ( newLogMutex );
        while ( !newLogs.empty ( ) )
        {
            logs.push_back ( newLogs.front ( ) );
            newLogs.pop ( );
        }
    }

    size_t logsToDelete = (logs.size ( ) > ACOREX_MAX_LOG_ENTRIES_STORED) ? logs.size ( ) - ACOREX_MAX_LOG_ENTRIES_STORED : 0;
    if ( logsToDelete > 0 )
    {
        logs.erase ( logs.begin ( ), logs.begin ( ) + logsToDelete );
    }
}

void Utilities::LogDisplay::Draw ( )
{
    ofEnableAlphaBlending ( );

    if ( !mLayout ) { return; }
    
    size_t logsToDraw = std::min ( logs.size ( ), ACOREX_MAX_LOG_ENTRIES_DISPLAYED );
    int logIndex = logs.size ( ) - 1; // start with newest log (size - 1), and go backwards (index--)

    int secondsToFadeStart = ACOREX_LOG_DISPLAY_SECONDS_TO_FADE_START;
    int secondsWhileFading = ACOREX_LOG_DISPLAY_SECONDS_WHILE_FADING;
    int secondsToDisplay = secondsToFadeStart + secondsWhileFading;

    for ( size_t i = 0; i < logsToDraw; i++ )
    {
        const LogEntry& log = logs[logIndex];

        if ( log.timestamp + std::chrono::seconds ( secondsToDisplay ) < std::chrono::system_clock::now ( ) )
        {
            logIndex--;
            continue; // skip logs older than 10 seconds
        }

        ofColor logLineColor = getLevelColor ( log.level );
        logLineColor.a = ofMap ( i, 0, ACOREX_MAX_LOG_ENTRIES_DISPLAYED, 255, 50 ); // newer logs are more opaque, older logs are more transparent

        if ( log.timestamp + std::chrono::seconds ( secondsToFadeStart ) < std::chrono::system_clock::now ( ) )
        {
            auto fadeStart = log.timestamp + std::chrono::seconds ( secondsToFadeStart );
            auto fadeEnd = fadeStart + std::chrono::seconds ( secondsWhileFading );
            float fadeProgress = std::chrono::duration<float> ( std::chrono::system_clock::now ( ) - fadeStart ).count ( ) / std::chrono::duration<float> ( fadeEnd - fadeStart ).count ( );
            logLineColor.a = ofMap ( fadeProgress, 0.0, 1.0, logLineColor.a, 0.0 );
        }

        ofSetColor ( logLineColor );
        ofDrawBitmapString ( log.context + ": " + log.message, 10, ofGetHeight ( ) - 10 - (i * mLayout->getPanelRowHeight ( )) );
        logIndex--;
    }
    
    ofDisableAlphaBlending ( );
}

void Utilities::LogDisplay::Exit ( )
{
}

void Utilities::LogDisplay::AddLog ( ofLogLevel level, const std::string& context, const std::string& message )
{
    std::lock_guard<std::mutex> lock ( newLogMutex );
    auto timestamp = std::chrono::system_clock::now ( );
    newLogs.push ( { level, context, message, timestamp } );
}

ofColor Utilities::LogDisplay::getLevelColor ( ofLogLevel level )
{
    switch ( level ) // TODO - move into InterfaceDefs.h
    {
    case OF_LOG_VERBOSE:        return ofColor ( 130, 130, 160 ); // Blueish dark grey
    case OF_LOG_NOTICE:         return ofColor ( 190, 190, 210 ); // Blueish grey
    case OF_LOG_WARNING:        return ofColor ( 240, 240, 10  ); // Yellow
    case OF_LOG_ERROR:          return ofColor ( 240, 10,  10  ); // Red
    case OF_LOG_FATAL_ERROR:    return ofColor ( 240, 10,  160 ); // Magenta
    default:                    return ofColor ( 130, 130, 130 ); // Dark grey
    }
}

// AcorexLoggerChannel ------------------------------------------------------------------

// TODO - option to change original channel sending at runtime
Utilities::AcorexLoggerChannel::AcorexLoggerChannel ( ) : bSendToOriginalChannel ( true )
{
    mOriginalChannel = ofGetLoggerChannel ( );
}

void Utilities::AcorexLoggerChannel::log ( ofLogLevel level, const std::string& module, const std::string& message )
{
    if ( mLogDisplay )
    {
        mLogDisplay->AddLog ( level, module, message );
    }

    if ( mOriginalChannel && bSendToOriginalChannel )
    {
        mOriginalChannel->log ( level, module, message );
    }
}