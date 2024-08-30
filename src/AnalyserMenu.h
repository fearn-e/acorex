/*
The MIT License (MIT)

Copyright (c) 2024 Elowyn Fearne

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "Utils/Data.h"
#include "Utils/JSON.h"
#include "Analyser/Controller.h"
#include "Utils/InterfaceDefs.h"
#include <ofxGui.h>
#include <ofSystemUtils.h>

namespace Acorex {

class AnalyserMenu {
public:
	AnalyserMenu ( );
	~AnalyserMenu ( ) { };

	void Initialise ( bool HiDpi );
	void Show ( );
	void Hide ( );
	void Draw ( );
	void Exit ( );

private:
	void RemoveListeners ( );

	// Analyse and Reduce --------------------------

	void Analyse ( );
	void Reduce ( );

	// File Dialog Button Callbacks ----------------

	void SelectAnalysisDirectory ( );
	void SelectAnalysisOutputFile ( );
	void SelectReductionInputFile ( );
	void SelectReductionOutputFile ( );

	// Load and Save Settings ----------------------

	void UnpackSettingsFromFile ( const Utils::AnalysisSettings& settings );
	void PackSettingsFromUser ( Utils::AnalysisSettings& settings);
	void PackSettingsFromUser ( Utils::ReductionSettings& settings );

	// UI Value Management -------------------------

	void QuantiseWindowSize ( int& value );
	void QuantiseHopFraction ( int& value );
	void AnalysisInsertionToggleChanged ( bool& value );

	// Panel Management ----------------------------

	void ToggleAnalysisUILockout ( bool lock );
	void ShowMainPanel ( );
	void ShowAnalysisPanel ( );
	void ShowAnalysisInsertionPanel ( );
	void HideAnalysisInsertionPanel ( );
	void ShowReductionPanel ( );

	// State --------------------------------------

	bool bDraw;
	bool bProcessing;

	bool bDrawMainPanel;
	bool bDrawAnalysisPanel;
	bool bDrawAnalysisInsertionPanel;
	bool bDrawReductionPanel;
	
	bool bInsertingIntoCorpus;

	bool bAnalysisDirectorySelected;
	bool bAnalysisOutputSelected;
	bool bReductionInputSelected;
	bool bReductionOutputSelected;
	
	bool bFlashingInvalidFileSelects;
	bool bFlashingInvalidAnalysisToggles;
	bool bFlashingInvalidReductionDimensions;
	int flashColour;

	// Metadata -----------------------------------

	//stats - mean, standard deviation, skewness, kurtosis, low percentile, middle (median default), high percentile
	//int maxSamplingRate = 22050;

	bool mHasBeenReduced;
	ofxToggle mAnalysisPitchToggle;
	ofxToggle mAnalysisLoudnessToggle;
	ofxToggle mAnalysisShapeToggle;
	ofxToggle mAnalysisMFCCToggle;
	ofxIntField mSampleRateField;
	ofxIntField mWindowFFTField;
	ofxIntField mHopFractionField;
	ofxIntField mNBandsField;
	ofxIntField mNCoefsField;
	ofxIntField mMinFreqField;
	ofxIntField mMaxFreqField;

	int mCurrentDimensionCount;

	ofxIntField mReducedDimensionsField;
	ofxIntField mMaxIterationsField;

	// File Paths ---------------------------------

	std::string inputPath;
	std::string outputPath;

	// Panels -------------------------------------

	ofxPanel mMainPanel;
	ofxButton mCreateCorpusButton;
	ofxButton mReduceCorpusButton;

	ofxPanel mAnalysisPanel;
	ofxPanel mAnalysisMetadataPanel;
	ofxPanel mAnalysisConfirmPanel;
	ofxButton mAnalysisPickDirectoryButton;
	ofxTextField mAnalysisDirectoryLabel;
	ofxButton mAnalysisPickOutputFileButton;
	ofxTextField mAnalysisOutputLabel;
	ofxButton mConfirmAnalysisButton;
	ofxButton mCancelAnalysisButton;

	ofxPanel mAnalysisInsertionPanel;
	ofxLabel mAnalysisInsertionQuestionLabel;
	ofxToggle mAnalysisInsertionReplaceWithNewToggle;


	ofxPanel mReductionPanel;
	ofxButton mReductionPickInputFileButton;
	ofxTextField mReductionInputLabel;
	ofxButton mReductionPickOutputFileButton;
	ofxTextField mReductionOutputLabel;
	ofxButton mConfirmReductionButton;
	ofxButton mCancelReductionButton;

	// Acorex Objects ------------------------------

	Analyser::Controller mController;
	Utils::JSON mJSON;
	Utils::Colors mColors;
	Utils::MenuLayout mLayout;
};

} // namespace Acorex