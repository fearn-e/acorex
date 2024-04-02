#pragma once

#include "Corpus/Data.h"
#include "Corpus/JSON.h"
#include "Corpus/Controller.h"
#include "Interface/InterfaceDefs.h"
#include <ofxGui.h>
#include <ofSystemUtils.h>

namespace AcorexInterface {

class ControllerUI {
public:
	ControllerUI ( ) { };
	~ControllerUI ( ) { };

	void setup ( );
	void draw ( );
	void exit ( );

private:
	// Setup --------------------------------------
	void Reset ( );
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

	bool UnpackSettingsFromFile ( AcorexCorpus::MetaSetStruct& metaset, bool cancelIfAlreadyReduced );
	AcorexCorpus::MetaSetStruct PackSettingsFromUser ( );

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
	int flashColour;

	// Metadata -----------------------------------

	//stats - mean, standard deviation, skewness, kurtosis, low percentile, middle (median default), high percentile
	//int maxSamplingRate = 22050;

	bool mHasBeenReduced;
	ofxToggle mTimeDimensionToggle;
	ofxToggle mAnalysisPitchToggle;
	ofxToggle mAnalysisLoudnessToggle;
	ofxToggle mAnalysisShapeToggle;
	ofxToggle mAnalysisMFCCToggle;
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
	ofxLabel mAnalysisDirectoryLabel;
	ofxButton mAnalysisPickOutputFileButton;
	ofxLabel mAnalysisOutputLabel;
	ofxButton mConfirmAnalysisButton;
	ofxButton mCancelAnalysisButton;

	ofxPanel mAnalysisInsertionPanel;
	ofxLabel mAnalysisInsertionQuestionLabel;
	ofxToggle mAnalysisInsertionReplaceWithNewToggle;


	ofxPanel mReductionPanel;
	ofxButton mReductionPickInputFileButton;
	ofxLabel mReductionInputLabel;
	ofxButton mReductionPickOutputFileButton;
	ofxLabel mReductionOutputLabel;
	ofxButton mConfirmReductionButton;
	ofxButton mCancelReductionButton;




	AcorexCorpus::Controller mController;
	AcorexCorpus::JSON mJSON;
	AcorexInterface::Colors mColors;
	AcorexInterface::ControllerUILayout mLayout;
};

} // namespace interface