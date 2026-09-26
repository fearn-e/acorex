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

#include "ActionsMenu.h"

using namespace Acorex;

ActionsMenu::ActionsMenu ( ) : bListenersAdded ( false ), bDraw ( false )
{

}

void ActionsMenu::Initialise ( )
{
    bDraw = false;
    RemoveListeners ( );

    ActionStrings actionStrings;

    int actionWidths = 180, actionHeights = mLayout->getTopBarHeight ( );
    int actionsY = 0;
    int analyserX = 0, explorerX = 200, settingsX = 400;

    // Analyser Actions Dropdown
    mAnalyserActionsDropdown.reset ( );
    mAnalyserActionsDropdown = make_unique<ofxDropdown> ( static_cast<std::string>("Analyser"), Utilities::ofxDropdownScrollSpeed );
    for ( auto& action : actionStrings.analyser ) { mAnalyserActionsDropdown->add ( action ); }
    mAnalyserActionsDropdown->setPosition ( analyserX, actionsY );
    mAnalyserActionsDropdown->setSize ( actionWidths, actionHeights );
    mAnalyserActionsDropdown->disableMultipleSelection ( );
    mAnalyserActionsDropdown->enableCollapseOnSelection ( );
    mAnalyserActionsDropdown->setDropDownPosition ( ofxDropdown::DD_BELOW );
    mAnalyserActionsDropdown->setBackgroundColor ( mColors.interfaceBackgroundColor );
    mAnalyserActionsDropdown->deselect ( );

    // Explorer Actions Dropdown
    mExplorerActionsDropdown.reset ( );
    mExplorerActionsDropdown = make_unique<ofxDropdown> ( static_cast<std::string>("Explorer"), Utilities::ofxDropdownScrollSpeed );
    for ( auto& action : actionStrings.explorer ) { mExplorerActionsDropdown->add ( action ); }
    mExplorerActionsDropdown->setPosition ( explorerX, actionsY );
    mExplorerActionsDropdown->setSize ( actionWidths, actionHeights );
    mExplorerActionsDropdown->disableMultipleSelection ( );
    mExplorerActionsDropdown->enableCollapseOnSelection ( );
    mExplorerActionsDropdown->setDropDownPosition ( ofxDropdown::DD_BELOW );
    mExplorerActionsDropdown->setBackgroundColor ( mColors.interfaceBackgroundColor );
    mExplorerActionsDropdown->deselect ( );

    // Settings Actions Dropdown
    mSettingsActionsDropdown.reset ( );
    mSettingsActionsDropdown = make_unique<ofxDropdown> ( static_cast<std::string>("Settings"), Utilities::ofxDropdownScrollSpeed );
    for ( auto& action : actionStrings.settings ) { mSettingsActionsDropdown->add ( action ); }
    mSettingsActionsDropdown->setPosition ( settingsX, actionsY );
    mSettingsActionsDropdown->setSize ( actionWidths, actionHeights );
    mSettingsActionsDropdown->disableMultipleSelection ( );
    mSettingsActionsDropdown->enableCollapseOnSelection ( );
    mSettingsActionsDropdown->setDropDownPosition ( ofxDropdown::DD_BELOW );
    mSettingsActionsDropdown->setBackgroundColor ( mColors.interfaceBackgroundColor );
    mSettingsActionsDropdown->deselect ( );

    AddListeners ( );
    bDraw = true;
}


void ActionsMenu::Draw ( )
{
    if ( !bDraw )
    { return; }

    mAnalyserActionsDropdown->draw ( );
    mExplorerActionsDropdown->draw ( );
    mSettingsActionsDropdown->draw ( );
}
void ActionsMenu::Update ( )
{

}


void ActionsMenu::Exit ( )
{
    RemoveListeners ( );
}


void ActionsMenu::RefreshUI ( )
{

}
void ActionsMenu::WindowResized ( )
{

}


void ActionsMenu::AddListeners ( )
{
    if ( bListenersAdded )
    { return; }
    bListenersAdded = true;
    ofAddListener ( mAnalyserActionsDropdown->dropdownHidden_E, this, &ActionsMenu::AnalyserAction );
    ofAddListener ( mExplorerActionsDropdown->dropdownHidden_E, this, &ActionsMenu::ExplorerAction );
    ofAddListener ( mSettingsActionsDropdown->dropdownHidden_E, this, &ActionsMenu::SettingsAction );
}
void ActionsMenu::RemoveListeners ( )
{
    if ( !bListenersAdded )
    { return; }
    bListenersAdded = false;
    ofRemoveListener ( mAnalyserActionsDropdown->dropdownHidden_E, this, &ActionsMenu::AnalyserAction );
    ofRemoveListener ( mExplorerActionsDropdown->dropdownHidden_E, this, &ActionsMenu::ExplorerAction );
    ofRemoveListener ( mSettingsActionsDropdown->dropdownHidden_E, this, &ActionsMenu::SettingsAction );
}


void ActionsMenu::AnalyserAction ( string& dropdownName )
{
    int selectedAction = mAnalyserActionsDropdown->getSelectedOptionIndex ( );
    mAnalyserActionsDropdown->deselect ( );

}
void ActionsMenu::ExplorerAction ( string& dropdownName )
{
    int selectedAction = mExplorerActionsDropdown->getSelectedOptionIndex ( );
    mExplorerActionsDropdown->deselect ( );

}
void ActionsMenu::SettingsAction ( string& dropdownName )
{
    int selectedAction = mSettingsActionsDropdown->getSelectedOptionIndex ( );
    mSettingsActionsDropdown->deselect ( );

}