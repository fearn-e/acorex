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

#include <ofxMidi.h>

/// queued
/// Manually handle messages using hasWaitingMessages()/getNextMessage().
/// Received messages are thread safe.

namespace Acorex {
namespace Utilities {

class MIDI {
public:
	void Initialise ( );
	void Update ( );
	void Draw ( );
	void Exit ( );

	void KeyEvent ( ofKeyEventArgs& args );

private:

	void AddListeners ( );
	void RemoveListeners ( );

	bool bListenersAdded = false;

	std::shared_ptr<ofxMidiIn> mMidiIn;

	std::vector<ofxMidiMessage> midiMessages; ///< received messages
	std::size_t maxMessages = 10; ///< max number of messages to keep track of
};

} // namespace Utilities
} // namespace Acorex