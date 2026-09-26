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

#include "Utilities/InterfaceDefs.h"

#include <ofxGui.h>
#include <ofxDropdown.h>

#include <string>

namespace Acorex {

struct AnalyserAction {
    enum Type : int {
        NONE = -1,
        ANALYSE = 0,
        REDUCE = 1,
        CANCEL = 2,
        ACTION_COUNT = 3
    };
};

struct ExplorerAction {
    enum Type : int {
        NONE = -1,
        OPEN_CORPUS = 0,
        CLOSE_CORPUS = 1,
        MIDI_TODO = 2,
        ACTION_COUNT = 3
    };
};

struct SettingsAction {
    enum Type : int {
        NONE = -1,
        AUDIO_SETTINGS = 0,
        DPI_TOGGLE = 1,
        ACTION_COUNT = 2
    };
};

struct ActionStrings {
    ActionStrings ( )
    {
        analyser.resize ( AnalyserAction::ACTION_COUNT );
        explorer.resize ( ExplorerAction::ACTION_COUNT );
        settings.resize ( SettingsAction::ACTION_COUNT );

        analyser[AnalyserAction::ANALYSE] = "Analyse";
        analyser[AnalyserAction::REDUCE] = "Reduce";
        analyser[AnalyserAction::CANCEL] = "Cancel";

        explorer[ExplorerAction::OPEN_CORPUS] = "Open Corpus";
        explorer[ExplorerAction::CLOSE_CORPUS] = "Close Corpus";
        explorer[ExplorerAction::MIDI_TODO] = "MIDI_HUB_ACTION_UNFINISHED";

        settings[SettingsAction::AUDIO_SETTINGS] = "Audio Settings";
        settings[SettingsAction::DPI_TOGGLE] = "DPI Toggle";

    }

    std::vector<std::string> analyser;
    std::vector<std::string> explorer;
    std::vector<std::string> settings;
};

class ActionsMenu {
public:
    ActionsMenu ( );
    ~ActionsMenu ( ) { }

    void Initialise ( );

    void Draw ( );
    void Update ( );

    void Exit ( );

    void RefreshUI ( );
    void WindowResized ( );

    void SetMenuLayout ( std::shared_ptr<Utilities::MenuLayout>& menuLayout ) { mLayout = menuLayout; }

private:
    bool bListenersAdded;
    void AddListeners ( );
    void RemoveListeners ( );

    void AnalyserAction ( string& dropdownName );
    void ExplorerAction ( string& dropdownName );
    void SettingsAction ( string& dropdownName );

    // States ----------------------------------------

    bool bDraw;
    bool bNeedDeselectActionDropdowns;

    // Menu Controls / Dropdowns ---------------------

    ofxPanel mAnalyserDropdownPanel;
    ofxPanel mExplorerDropdownPanel;
    ofxPanel mSettingsDropdownPanel;

    unique_ptr<ofxDropdown> mAnalyserActionsDropdown;
    unique_ptr<ofxDropdown> mExplorerActionsDropdown;
    unique_ptr<ofxDropdown> mSettingsActionsDropdown;

    // Acorex Objects --------------------------------

    Utilities::Colors mColors;
    std::shared_ptr<Utilities::MenuLayout> mLayout;
};

} // namespace Acorex