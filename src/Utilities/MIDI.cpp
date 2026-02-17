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

Utilities::MIDIHub::MIDIHub ( ) : bListenersAdded ( false )
{
}

// TODO - retrigger this on a hotkey / periodically retrigger to check if parent instance died?
void Utilities::MIDIHub::Initialise ( )
{
	if ( mMidiIn == NULL )
	{
		mMidiIn = std::shared_ptr<ofxMidiIn> ( new ofxMidiIn ( "ofxMidi Client" ) );
	}

    mOscSender0.setup ( "localhost", ACOREX_OSC_PORT + 0 );
    mOscSender1.setup ( "localhost", ACOREX_OSC_PORT + 1 );
    mOscSender2.setup ( "localhost", ACOREX_OSC_PORT + 2 );
    mOscSender3.setup ( "localhost", ACOREX_OSC_PORT + 3 );

	mMidiIn->openPort ( 0 );
	//midiIn.openPort("IAC Pure Data In");	// by name
	//midiIn.openVirtualPort("ofxMidiIn Input"); // open a virtual port

	// don't ignore sysex, timing, & active sense messages,
	// these are ignored by default
	//midiIn.ignoreTypes(false, false, false);

	// print received messages to the console
	// mMidiIn->setVerbose ( true );

	AddListeners ( );
}

void Utilities::MIDIHub::Update ( )
{
	if ( mMidiIn->hasWaitingMessages ( ) )
	{
		ofxMidiMessage message;

		while ( mMidiIn->getNextMessage ( message ) )
		{
			midiMessages.push_back ( message );
		}

		while ( midiMessages.size ( ) > maxMessages )
		{
			midiMessages.erase ( midiMessages.begin ( ) );
		}
	}

	while ( midiMessages.size ( ) > 0 )
	{
		// PROCESS MIDI HERE AND SEND OSC MESSAGES

		// TEMPORARY HARDCODED VALUES FOR TESTING - TODO - learn midi at runtime function?
		// control 0, 2, 4, 6 - volumes - (faders) (XoXoXoXo)
        // control 1, 3, 5, 7 - jump chance - (faders) (oXoXoXoX)
        // control 16, 18, 20, 22 - pan width - (knobs) (XoXoXoXo)
        // control 17, 19, 21, 23 - crossfade sample length - (knobs) (oXoXoXoX)
		// control 32, 34, 36, 38 - create playhead at picker point - (S button)
		// control 48, 50, 52, 54 - pick random point - (M button)
		// control 64, 66, 68, 70 - create playhead at random point - (R button)

		bool volumeMessage = false;
        bool jumpChanceMessage = false;
        bool panWidthMessage = false;
        bool crossfadeSampleLengthMessage = false;
        bool createPickerPlayheadMessage = false;
        bool pickRandomPointMessage = false;
        bool createRandomPlayheadMessage = false;

        // pointer to the osc sender to send to:
        ofxOscSender* sender = nullptr;

        int receiver1Controls[ ] = { 0, 1, 16, 17, 32, 48, 64 };
        int receiver2Controls[ ] = { 2, 3, 18, 19, 34, 50, 66 };
        int receiver3Controls[ ] = { 4, 5, 20, 21, 36, 52, 68 };
        int receiver4Controls[ ] = { 6, 7, 22, 23, 38, 54, 70 };

		int volumeControls[ ] = { 0, 2, 4, 6 };
		int jumpChanceControls[ ] = { 1, 3, 5, 7 };
		int panWidthControls[ ] = { 16, 18, 20, 22 };
        int crossfadeSampleLengthControls[ ] = { 17, 19, 21, 23 };
        int createPickerPlayheadControls[ ] = { 32, 34, 36, 38 };
        int pickRandomPointControls[ ] = { 48, 50, 52, 54 };
        int createRandomPlayheadControls[ ] = { 64, 66, 68, 70 };

		if ( std::find ( std::begin ( receiver1Controls ), std::end ( receiver1Controls ), midiMessages[0].control ) != std::end ( receiver1Controls ) )
		{
            sender = &mOscSender0;
		}
		else if ( std::find ( std::begin ( receiver2Controls ), std::end ( receiver2Controls ), midiMessages[0].control ) != std::end ( receiver2Controls ) )
		{
            sender = &mOscSender1;
		}
		else if ( std::find ( std::begin ( receiver3Controls ), std::end ( receiver3Controls ), midiMessages[0].control ) != std::end ( receiver3Controls ) )
		{
            sender = &mOscSender2;
		}
		else if ( std::find ( std::begin ( receiver4Controls ), std::end ( receiver4Controls ), midiMessages[0].control ) != std::end ( receiver4Controls ) )
		{
            sender = &mOscSender3;
        }

		if ( midiMessages[0].status == MIDI_CONTROL_CHANGE )
		{
			if ( std::find ( std::begin ( volumeControls ), std::end ( volumeControls ), midiMessages[0].control ) != std::end ( volumeControls ) )
			{
				volumeMessage = true;
			}
			else if ( std::find ( std::begin ( jumpChanceControls ), std::end ( jumpChanceControls ), midiMessages[0].control ) != std::end ( jumpChanceControls ) )
			{
				jumpChanceMessage = true;
			}
			else if ( std::find ( std::begin ( panWidthControls ), std::end ( panWidthControls ), midiMessages[0].control ) != std::end ( panWidthControls ) )
			{
				panWidthMessage = true;
			}
			else if ( std::find ( std::begin ( crossfadeSampleLengthControls ), std::end ( crossfadeSampleLengthControls ), midiMessages[0].control ) != std::end ( crossfadeSampleLengthControls ) )
			{
				crossfadeSampleLengthMessage = true;
			}
			else if ( std::find ( std::begin ( createPickerPlayheadControls ), std::end ( createPickerPlayheadControls ), midiMessages[0].control ) != std::end ( createPickerPlayheadControls ) )
			{
				createPickerPlayheadMessage = true;
			}
			else if ( std::find ( std::begin ( pickRandomPointControls ), std::end ( pickRandomPointControls ), midiMessages[0].control ) != std::end ( pickRandomPointControls ) )
			{
				pickRandomPointMessage = true;
			}
			else if ( std::find ( std::begin ( createRandomPlayheadControls ), std::end ( createRandomPlayheadControls ), midiMessages[0].control ) != std::end ( createRandomPlayheadControls ) )
			{
				createRandomPlayheadMessage = true;
            }
		}

		//volume0
        if ( volumeMessage )
		{
			int volume = ofMap ( midiMessages[0].value, 0, 127, 0, 1000, true );
			sender->send ( "/acorex/control/volume", volume );
            ofLogVerbose ( "MIDI-PARENT" ) << "Sent OSC message: /acorex/control/volume " << volume;
        }
		//jump chance
		else if ( jumpChanceMessage )
		{
			int jumpChance = ofMap ( midiMessages[0].value, 0, 127, 0, 1000, true );
			sender->send ( "/acorex/control/jump_chance", jumpChance );
            ofLogVerbose ( "MIDI-PARENT" ) << "Sent OSC message: /acorex/control/jump_chance " << jumpChance;
		}
		//pan width
		else if ( panWidthMessage )
		{
			int panWidth = ofMap ( midiMessages[0].value, 0, 127, 0, 1000, true );
			sender->send ( "/acorex/control/pan_width", panWidth );
			ofLogVerbose ( "MIDI-PARENT" ) << "Sent OSC message: /acorex/control/pan_width " << panWidth;
        }
        //crossfade sample length
		else if ( crossfadeSampleLengthMessage )
		{
			int crossfadeSampleLength = ofMap ( midiMessages[0].value, 0, 127, 0, 1000, true );
			sender->send ( "/acorex/control/crossfade_sample_length", crossfadeSampleLength );
			ofLogVerbose ( "MIDI-PARENT" ) << "Sent OSC message: /acorex/control/crossfade_sample_length " << crossfadeSampleLength;
        }
		//create playhead at picker point
        else if ( createPickerPlayheadMessage )
		{
			if ( midiMessages[0].value < 64 ) // only trigger on button release
			{
				sender->send ( "/acorex/control/create_picker_playhead", 1 );
				ofLogVerbose ( "MIDI-PARENT" ) << "Sent OSC message: /acorex/control/create_picker_playhead";
			}
        }
		//pick random point
		else if ( pickRandomPointMessage )
		{
			if ( midiMessages[0].value < 64 ) // only trigger on button release
			{
				sender->send ( "/acorex/control/pick_random_point", 1 );
				ofLogVerbose ( "MIDI-PARENT" ) << "Sent OSC message: /acorex/control/pick_random_point";
			}
        }
		//create playhead at random point
        else if ( createRandomPlayheadMessage ) 
		{
			if ( midiMessages[0].value < 64 ) // only trigger on button release
			{
				sender->send ( "/acorex/control/create_random_playhead", 1 );
				ofLogVerbose ( "MIDI-PARENT" ) << "Sent OSC message: /acorex/control/create_random_playhead";
			}
        }
		else
		{
            ofLogVerbose ( "MIDI-PARENT" ) << "Received unhandled MIDI message: " << midiMessages[0].toString ( );
            ofLogVerbose ( "STATUS" ) << ofxMidiMessage::getStatusString ( midiMessages[0].status );
            ofLogVerbose ( "CHANNEL" ) << midiMessages[0].channel;
            ofLogVerbose ( "CONTROL" ) << midiMessages[0].control;
            ofLogVerbose ( "VALUE" ) << midiMessages[0].value;
		}

		midiMessages.erase ( midiMessages.begin ( ) );
	}
}

void Utilities::MIDIHub::Exit ( )
{
	RemoveListeners ( );

	mMidiIn->closePort ( );

	mOscSender0.clear ( );
	mOscSender1.clear ( );
	mOscSender2.clear ( );
    mOscSender3.clear ( );
}

void Utilities::MIDIHub::AddListeners ( )
{
	if ( bListenersAdded ) { return; }

	ofAddListener ( ofEvents ( ).keyReleased, this, &Utilities::MIDIHub::KeyEvent );

	bListenersAdded = true;
}

void Utilities::MIDIHub::RemoveListeners ( )
{
	if ( !bListenersAdded ) { return; }

	ofRemoveListener ( ofEvents ( ).keyReleased, this, &Utilities::MIDIHub::KeyEvent );

	bListenersAdded = false;
}

void Utilities::MIDIHub::KeyEvent ( ofKeyEventArgs& args )
{
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