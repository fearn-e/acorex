#include "ofApp.h"
#include <cmath>
#include <algorithm>

//--------------------------------------------------------------
void ofApp::setup() {
    // create the gui objects
    outputVolume.setup("Output", -60, -60, 12);
    outputVolume.setShape(19, 225, 514, 30);

    int bufferSize = 128;

    inputBuffer.assign(bufferSize, 0.0);

    // set the audio parameters
    mAudioSettings.sampleRate = 44100;
    mAudioSettings.bufferSize = bufferSize;
    mAudioSettings.numInputChannels = 1;
    mAudioSettings.numOutputChannels = 2;
    mAudioSettings.setInListener(ofGetAppPtr());
    mAudioSettings.setOutListener(ofGetAppPtr());

    inputBuffer.resize(bufferSize);
    magBuffer.resize(513);
    melBuffer.resize(40);

    // setup the circular buffer with host limits
    mBufferedProcess.hostSize(bufferSize);
    //  mBufferedProcess.maxSize(1024, 1024, mAudioSettings.numInputChannels, mAudioSettings.numOutputChannels);

      // setup the melbands filter
    mMel.init(40, 10000, 40, 513, mAudioSettings.sampleRate, 1024);

    // setup the hpss filter
    mHPSS.init(513, 31);

    // starts the audio engine
    mSoundStream.setup(mAudioSettings);
}

//--------------------------------------------------------------
void ofApp::update() {
    volume = std::pow(10, outputVolume / 20.);
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofNoFill();
    ofSetColor(225);
    outputVolume.draw();

    // draw the input channel:
    //title
    ofTranslate(20, 20, 0);
    ofDrawBitmapString("Input Wave", 4, 18);

    //frame
    ofSetLineWidth(1);
    ofDrawRectangle(0, 0, 512, 200);

    //wave
    ofSetColor(235, 58, 135);
    ofSetLineWidth(3);
    ofBeginShape();
    float ratio = 512 / mAudioSettings.bufferSize;
    for (unsigned int i = 0; i < mAudioSettings.bufferSize; i++) {
        ofVertex(i * ratio, constrain(100.0 - (inputBuffer[i] * 100.0), 0, 200));
    }
    ofEndShape(false);

    // draw the spectrogram:
    //title
    ofSetColor(225);
    ofTranslate(0, 240, 0);
    ofDrawBitmapString("Spectrogram", 4, 18);

    //frame
    ofSetLineWidth(1);
    ofDrawRectangle(0, 0, 512, 200);

    //wave
    ofSetColor(245, 58, 135);
    ofSetLineWidth(3);
    ofBeginShape();
    for (unsigned int i = 0; i < 512; i++) {
        ofVertex(i, constrain(200.0 - (magBuffer[i] * 4), 0, 200));
    }
    ofEndShape(false);

    // draw the mels:
    //title
    ofSetColor(225);
    ofTranslate(0, 210, 0);
    ofDrawBitmapString("40 MelBands", 4, 18);

    //frame
    ofSetLineWidth(1);
    ofDrawRectangle(0, 0, 512, 200);

    //wave
    ofSetColor(245, 58, 135);
    ofSetLineWidth(3);
    ofBeginShape();
    //  ofVertex(0, 0); //start bottom left
    for (unsigned int i = 0; i < 40; i++) {
        int val = constrain(200.0 - (melBuffer[i] * 3), 0, 200);
        ofVertex(i * 12.8, val);
        ofVertex((i + 1) * 12.8, val);
    };
    //  ofVertex(40*12, 0);//end bottom right
    ofEndShape(false);
}

//--------------------------------------------------------------
void ofApp::exit() {
    mSoundStream.close();
}
//--------------------------------------------------------------
void ofApp::audioIn(float* input, int bufferSize, int nChannels) {
    for (int i = 0; i < bufferSize; i++) {
        inputBuffer[i] = input[i];
    }

    //feeding the circular buffer
    //create a view of the left input
    fluid::FluidTensorView<float, 2> ofView{ input,0,bufferSize,nChannels };
    //  PRINTSIZE(ofView);
    mBufferedProcess.push(ofView.transpose()); //buffer input samples
    // process next buffer’s worth of data
    // need an overlap of 4 or there is intermodulation (see STFTBufferedProcess for how to compensate via normalise)
    mBufferedProcess.process(1024, 1024, 512, mContext, [this](fluid::FluidTensorView<double, 2> in, fluid::FluidTensorView<double, 2> out) {
        //    PRINTSIZE(in); PRINTSIZE(out);

        // stft the first input and get the magnitudes
        mSTFT.processFrame(in.row(0), complexSpectre); // mono in (array) to tensor of complex out
        mSTFT.magnitude(complexSpectre, magSpectre); // get the magnitudes

        // mel process
        mMel.processFrame(magSpectre, mels, false, false, true, fluid::FluidDefaultAllocator()); // process the melbands from the mags
        std::copy(mels.begin(), mels.end(), melBuffer.begin()); // export the mels

        // convert the spectrogram
        magSpectre.apply([](double& x) { x = 20 * std::log10(x); });// convert to dB in place
        std::copy(magSpectre.begin(), magSpectre.end(), magBuffer.begin()); //export the spectre

        // run the HPSS process
        mHPSS.processFrame(complexSpectre, complexHPSSout, 11, 11, 0, 0, 0, 0, 0, 0, 0, 0, 0); //mode 0 doesn't have thresh

        // inverse stft to output vector
        mISTFT.processFrame(complexHPSSout.col(1), out.row(0)); // harm (as COL!!! and 2nd out!!!) to left channel
        mISTFT.processFrame(complexHPSSout.col(0), out.row(1)); // perc (as COL!!! and 1st out!!!) to right channel
        //    PRINTSIZE(complexSpectre);PRINTSIZE(in); PRINTSIZE(out);

            // apply the volume to the output vector
        out.apply([this](double& x) { x = x * volume; });
        });
}

//--------------------------------------------------------------
void ofApp::audioOut(float* buffer, int bufferSize, int nChannels) {

    fluid::FluidTensorView<float, 2> ofView{ buffer,0,bufferSize,nChannels };
    mBufferedProcess.pull(ofView.transpose());
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
