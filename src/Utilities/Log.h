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

#pragma once

#include "Utilities/InterfaceDefs.h"

#include <ofLog.h>
#include <ofColor.h>
#include <vector>
#include <queue>
#include <mutex>
#include <string>

// TODO - needs to be visible while stuff is loading/processing and the window is frozen... will need multithreading

namespace Acorex {
namespace Utilities {

struct LogEntry {
    //add timestamp
    LogEntry ( ofLogLevel newLevel, std::string newContext, std::string newMessage, std::chrono::system_clock::time_point newTimestamp ) : level ( newLevel ), context ( newContext ), message ( newMessage ), timestamp ( newTimestamp ) { }
    ofLogLevel level = OF_LOG_NOTICE;
    std::string context = "";
    std::string message = "";
    std::chrono::system_clock::time_point timestamp;
};

class LogDisplay {
public:
    LogDisplay ( );
    ~LogDisplay ( ) { }

    void Initialise ( );
    void Update ( );
    void Draw ( );
    void Exit ( );

    void AddLog ( ofLogLevel level, const std::string& context, const std::string& message );

    void SetMenuLayout ( std::shared_ptr<MenuLayout> newLayout ) { mLayout = newLayout; }

    void KeyEvent ( ofKeyEventArgs& args );

    //void Clear ( );
private:
    bool bListenersAdded;
    void AddListeners ( );
    void RemoveListeners ( );

    ofColor getLevelColor ( ofLogLevel level );

    std::shared_ptr<MenuLayout> mLayout;

    std::vector<LogEntry> logs;

    std::mutex newLogMutex;
    std::queue<LogEntry> newLogs;
};

class AcorexLoggerChannel : public ofBaseLoggerChannel {
private:
    std::shared_ptr<LogDisplay> mLogDisplay;

    std::shared_ptr<ofBaseLoggerChannel> mOriginalChannel;
    bool bSendToOriginalChannel;

public:
    AcorexLoggerChannel ( );

    void SetLogDisplay ( std::shared_ptr<LogDisplay> newLogDisplay ) { mLogDisplay = newLogDisplay; }
    void ToggleSendToOriginalChannel ( ) { bSendToOriginalChannel = !bSendToOriginalChannel; }

    void log ( ofLogLevel level, const std::string& module, const std::string& message ); //override
};

} // namespace Utilities
} // namespace Acorex