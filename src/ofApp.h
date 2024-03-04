#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include <complex>
#include "flucoma-core/clients/common/FluidContext.hpp"
#include "flucoma-core/clients/common/BufferedProcess.hpp"
#include "flucoma-core/algorithms/public/STFT.hpp"
#include "flucoma-core/algorithms/public/MelBands.hpp"
#include "flucoma-core/algorithms/public/HPSS.hpp"
#include "flucoma-core/data/FluidMemory.hpp"

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define divider "\n---------\n"
#define PRINTSIZE(X) std::cout << "rows: " << X.rows() << "\ncols: " << X.cols() << "\nsize: " << X.size() << divider;

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();
	void exit();

	void audioIn(float* input, int bufferSize, int nChannels);
	void audioOut(float* buffer, int bufferSize, int nChannels);

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	//gui objects
	ofxSlider<float> outputVolume;
	float volume;

	// audio
	ofSoundStreamSettings mAudioSettings;
	ofSoundStream mSoundStream;

	vector<float> inputBuffer;
	vector<float> magBuffer;
	vector<float> melBuffer;

	fluid::client::BufferedProcess mBufferedProcess{ 1024,1024,1,2,128,fluid::FluidDefaultAllocator() }; //this makes a circular buffer manager
	fluid::client::FluidContext mContext; // this enables the fluid verse
	fluid::algorithm::STFT mSTFT{ 1024,1024,512 }; //this is a STFT processor
	fluid::algorithm::ISTFT mISTFT{ 1024,1024,512 }; //and its inverse processor
	fluid::algorithm::MelBands mMel{ 40,1024 }; //a melband processor
	fluid::algorithm::HPSS mHPSS{ 1024,31,fluid::FluidDefaultAllocator() }; //an hpss processor

	fluid::FluidTensor<std::complex<double>, 1> complexSpectre = fluid::FluidTensor<std::complex<double>, 1>(513);
	fluid::FluidTensor<double, 1> magSpectre = fluid::FluidTensor<double, 1>(513);
	fluid::FluidTensor<double, 1> mels = fluid::FluidTensor<double, 1>(40);
	fluid::FluidTensor<std::complex<double>, 2> complexHPSSout = fluid::FluidTensor<std::complex<double>, 2>(513, 3);//still have to define the output manually (like above)
};
