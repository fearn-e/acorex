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

ActionsMenu::ActionsMenu ( ) : bListenersAdded ( false ), bDraw ( false ), bNeedDeselectActionDropdowns ( false )
{

}

#define TEMP_ACTION_WIDTHS 180
#define TEMP_ACTIONS_Y 50
#define TEMP_ANALYSER_X 0
#define TEMP_EXPLORER_X 200
#define TEMP_SETTINGS_X 400

void ActionsMenu::Initialise ( )
{
    bDraw = false;
    RemoveListeners ( );

    mAnalyserDropdownPanel.clear ( );
    mAnalyserDropdownPanel.setup ( );

    mExplorerDropdownPanel.clear ( );
    mExplorerDropdownPanel.setup ( );
    
    mSettingsDropdownPanel.clear ( );
    mSettingsDropdownPanel.setup ( );

    ActionStrings actionStrings;

    //TODO.TEMP - add these values to interfacedefs
    int actionHeights = mLayout->getTopBarHeight ( );

    // Analyser Actions Dropdown
    mAnalyserActionsDropdown.reset ( );
    mAnalyserActionsDropdown = make_unique<ofxDropdown> ( static_cast<std::string>("Analyser"), 0 );
    for ( auto& action : actionStrings.analyser ) { mAnalyserActionsDropdown->add ( action ); }
    mAnalyserDropdownPanel.add ( mAnalyserActionsDropdown.get ( ) );
    //TODO.TEMP - try changing these
    //mAnalyserActionsDropdown->setSize ( actionWidths, actionHeights );
    mAnalyserActionsDropdown->disableMultipleSelection ( );
    mAnalyserActionsDropdown->enableCollapseOnSelection ( );
    mAnalyserActionsDropdown->setDropDownPosition ( ofxDropdown::DD_BELOW );
    mAnalyserActionsDropdown->setBackgroundColor ( mColors.interfaceBackgroundColor );
    mAnalyserActionsDropdown->deselect ( );

    // Explorer Actions Dropdown
    mExplorerActionsDropdown.reset ( );
    mExplorerActionsDropdown = make_unique<ofxDropdown> ( static_cast<std::string>("Explorer"), 0 );
    for ( auto& action : actionStrings.explorer ) { mExplorerActionsDropdown->add ( action ); }
    mExplorerDropdownPanel.add ( mExplorerActionsDropdown.get ( ) );
    //TODO.TEMP - try changing these
    //mExplorerActionsDropdown->setSize ( actionWidths, actionHeights );
    mExplorerActionsDropdown->disableMultipleSelection ( );
    mExplorerActionsDropdown->enableCollapseOnSelection ( );
    mExplorerActionsDropdown->setDropDownPosition ( ofxDropdown::DD_BELOW );
    mExplorerActionsDropdown->setBackgroundColor ( mColors.interfaceBackgroundColor );
    mExplorerActionsDropdown->deselect ( );

    // Settings Actions Dropdown
    mSettingsActionsDropdown.reset ( );
    mSettingsActionsDropdown = make_unique<ofxDropdown> ( static_cast<std::string>("Settings"), 0 );
    for ( auto& action : actionStrings.settings ) { mSettingsActionsDropdown->add ( action ); }
    mSettingsDropdownPanel.add ( mSettingsActionsDropdown.get ( ) );
    //TODO.TEMP - try changing these
    //mSettingsActionsDropdown->setSize ( actionWidths, actionHeights );
    mSettingsActionsDropdown->disableMultipleSelection ( );
    mSettingsActionsDropdown->enableCollapseOnSelection ( );
    mSettingsActionsDropdown->setDropDownPosition ( ofxDropdown::DD_BELOW );
    mSettingsActionsDropdown->setBackgroundColor ( mColors.interfaceBackgroundColor );
    mSettingsActionsDropdown->deselect ( );

    mAnalyserDropdownPanel.setPosition ( TEMP_ANALYSER_X, TEMP_ACTIONS_Y );
    mAnalyserDropdownPanel.setWidthElements ( TEMP_ACTION_WIDTHS );
    mAnalyserDropdownPanel.disableHeader ( );

    mExplorerDropdownPanel.setPosition ( TEMP_EXPLORER_X, TEMP_ACTIONS_Y );
    mExplorerDropdownPanel.setWidthElements ( TEMP_ACTION_WIDTHS );
    mExplorerDropdownPanel.disableHeader ( );

    mSettingsDropdownPanel.setPosition ( TEMP_SETTINGS_X, TEMP_ACTIONS_Y );
    mSettingsDropdownPanel.setWidthElements ( TEMP_ACTION_WIDTHS );
    mSettingsDropdownPanel.disableHeader ( );

    AddListeners ( );
    bDraw = true;
}


void ActionsMenu::Draw ( )
{
    if ( !bDraw )
    { return; }

    mAnalyserDropdownPanel.draw ( );
    mExplorerDropdownPanel.draw ( );
    mSettingsDropdownPanel.draw ( );
}
void ActionsMenu::Update ( )
{
    if ( bNeedDeselectActionDropdowns )
    {
        ofLogFatalError ( "ActionsMenu" ) << "Deselecting dropdowns.";
        bNeedDeselectActionDropdowns = false;
        mAnalyserActionsDropdown->deselect ( );
        mExplorerActionsDropdown->deselect ( );
        mSettingsActionsDropdown->deselect ( );
    }
}


void ActionsMenu::Exit ( )
{
    RemoveListeners ( );
}


void ActionsMenu::RefreshUI ( )
{
    //TODO.TEMP - add these values to interfacedefs
    int actionHeights = mLayout->getTopBarHeight ( );

    mAnalyserDropdownPanel.setPosition ( TEMP_ANALYSER_X, TEMP_ACTIONS_Y );
    mAnalyserActionsDropdown->setSize ( TEMP_ACTION_WIDTHS, actionHeights );
    mAnalyserDropdownPanel.setWidthElements ( TEMP_ACTION_WIDTHS );
    mAnalyserDropdownPanel.sizeChangedCB ( );

    mExplorerDropdownPanel.setPosition ( TEMP_EXPLORER_X, TEMP_ACTIONS_Y );
    mExplorerActionsDropdown->setSize ( TEMP_ACTION_WIDTHS, actionHeights );
    mExplorerDropdownPanel.setWidthElements ( TEMP_ACTION_WIDTHS );
    mExplorerDropdownPanel.sizeChangedCB ( );

    mSettingsDropdownPanel.setPosition ( TEMP_SETTINGS_X, TEMP_ACTIONS_Y );
    mSettingsActionsDropdown->setSize ( TEMP_ACTION_WIDTHS, actionHeights );
    mSettingsDropdownPanel.setWidthElements ( TEMP_ACTION_WIDTHS );
    mSettingsDropdownPanel.sizeChangedCB ( );
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
    bNeedDeselectActionDropdowns = true;
    switch ( selectedAction )
    {
    case AnalyserAction::NONE:
        //nothing selected, skip
        break;
    case AnalyserAction::ANALYSE:

        ofLogWarning ( "ActionsMenu" ) << "This action is as of yet not implemented.";
        break;
    case AnalyserAction::REDUCE:

        ofLogWarning ( "ActionsMenu" ) << "This action is as of yet not implemented.";
        break;
    case AnalyserAction::CANCEL:
        //TODO.67
        ofLogWarning ( "ActionsMenu" ) << "This action is as of yet not implemented.";
        break;
    default:
        ofLogError ( "ActionsMenu" ) << "Undefined analyser action: " << selectedAction << ", \"" << mAnalyserActionsDropdown->getAllSelected ( )[0] << "\"";
        break;
    }

    return;
}
void ActionsMenu::ExplorerAction ( string& dropdownName )
{
    int selectedAction = mExplorerActionsDropdown->getSelectedOptionIndex ( );
    bNeedDeselectActionDropdowns = true;
    switch ( selectedAction )
    {
    case ExplorerAction::NONE:
        //nothing selected, skip
        break;
    case ExplorerAction::OPEN_CORPUS:

        ofLogWarning ( "ActionsMenu" ) << "This action is as of yet not implemented.";
        break;
    case ExplorerAction::CLOSE_CORPUS:
        //TODO.TEMP - close corpus without having to open a new one
        ofLogWarning ( "ActionsMenu" ) << "This action is as of yet not implemented.";
        break;
    case ExplorerAction::MIDI_TODO:
        //TODO.66
        ofLogWarning ( "ActionsMenu" ) << "This action is as of yet not implemented.";
        break;
    default:
        ofLogError ( "ActionsMenu" ) << "Undefined explorer action: " << selectedAction << ", \"" << mExplorerActionsDropdown->getAllSelected ( )[0] << "\"";
        break;
    }

    return;
}
void ActionsMenu::SettingsAction ( string& dropdownName )
{
    int selectedAction = mSettingsActionsDropdown->getSelectedOptionIndex ( );
    bNeedDeselectActionDropdowns = true;
    switch ( selectedAction )
    {
    case SettingsAction::NONE:
        //nothing selected, skip
        break;
    case SettingsAction::AUDIO_SETTINGS:
        //TODO.TEMP - open audio settings menu? popout a second window? draw a window within the acorex window?
        ofLogWarning ( "ActionsMenu" ) << "This action is as of yet not implemented.";
        break;
    case SettingsAction::DPI_TOGGLE:
        //TODO.TEMP - toggle DPI here instead of ofApp
        ofLogWarning ( "ActionsMenu" ) << "This action is as of yet not implemented.";
        break;
    default:
        ofLogError ( "ActionsMenu" ) << "Undefined settings action: " << selectedAction << ", \"" << mSettingsActionsDropdown->getAllSelected ( )[0] << "\"";
        break;
    }

    return;
}