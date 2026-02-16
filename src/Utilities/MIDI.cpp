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

#include "Utilities/MIDI.h"

#include "Utilities/TemporaryKeybinds.h"

#include <ofLog.h>

using namespace Acorex;

//--------------------------------------------------------------
void Utilities::MIDI::Initialise() {
	if ( mMidiIn == NULL )
	{
		mMidiIn = std::shared_ptr<ofxMidiIn> ( new ofxMidiIn ( "ofxMidi Client" ) );
	}

	mMidiIn->openPort ( 0 );
	//midiIn.openPort("IAC Pure Data In");	// by name
	//midiIn.openVirtualPort("ofxMidiIn Input"); // open a virtual port

	// don't ignore sysex, timing, & active sense messages,
	// these are ignored by default
	//midiIn.ignoreTypes(false, false, false);

	// print received messages to the console
	mMidiIn->setVerbose ( true );

	AddListeners ( );
}

//--------------------------------------------------------------
void Utilities::MIDI::Update() {
	/// queued message handling
	if ( mMidiIn->hasWaitingMessages ( ) )
	{
		ofxMidiMessage message;

		// add the latest message to the message queue
		while ( mMidiIn->getNextMessage ( message ) )
		{
			midiMessages.push_back ( message );
		}

		// remove any old messages if we have too many
		while ( midiMessages.size ( ) > maxMessages )
		{
			midiMessages.erase ( midiMessages.begin ( ) );
		}
	}

	while ( midiMessages.size ( ) > 0 )
	{
        ofLogNotice ( "MIDI" ) << "Received MIDI message: " << midiMessages[0].toString ( );
        midiMessages.erase ( midiMessages.begin ( ) );
    }
}

//--------------------------------------------------------------
void Utilities::MIDI::Exit() {
	RemoveListeners ( );

	mMidiIn->closePort();
}

void Utilities::MIDI::AddListeners ( )
{
    if ( bListenersAdded ) { return; }
	
	ofAddListener ( ofEvents ( ).keyReleased, this, &Utilities::MIDI::KeyEvent );

	bListenersAdded = true;
}

void Utilities::MIDI::RemoveListeners ( )
{
    if ( !bListenersAdded ) { return; }

    ofRemoveListener ( ofEvents ( ).keyReleased, this, &Utilities::MIDI::KeyEvent );

    bListenersAdded = false;
}

//--------------------------------------------------------------
void Utilities::MIDI::KeyEvent ( ofKeyEventArgs& args ) {
	if ( args.type == ofKeyEventArgs::Type::Released )
	{
		if ( args.key == ACOREX_KEYBIND_MIDI_LIST_PORTS ) { mMidiIn->listInPorts ( ); }
		else if ( args.key == ACOREX_KEYBIND_MIDI_NEXT_PORT )
		{
			int port = 0;
			if ( mMidiIn->isOpen ( ) ) { port = mMidiIn->getPort ( ); }

			int numPorts = mMidiIn->getNumInPorts ( );
			if ( numPorts > 0 && mMidiIn->isOpen ( ) )
			{
				port = (port + 1) % numPorts;
				mMidiIn->openPort ( port );
				ofLogNotice ( "MIDI" ) << "Opened MIDI port: " << mMidiIn->getInPortName ( port );
			}
			else if ( numPorts > 0 )
			{
				mMidiIn->openPort ( port );
				ofLogNotice ( "MIDI" ) << "Opened MIDI port: " << mMidiIn->getInPortName ( port );

			}
			else
			{
				ofLogWarning ( "MIDI" ) << "No MIDI input ports available.";
			}
		}
	}
}