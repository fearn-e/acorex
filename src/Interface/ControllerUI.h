#pragma once

#include "Corpus/Controller.h"
#include "Corpus/Metadata.h"
#include "Corpus/JSON.h"
#include "Interface/InterfaceDefs.h"
#include <ofxGui.h>
#include <ofSystemUtils.h>

namespace acorex {
namespace interface {

class ControllerUI {
public:
	ControllerUI ( ) { };
	~ControllerUI ( ) { };

	void setup ( );
	void draw ( );
	void exit ( );

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

	bool SetSettingsFromFile ( std::vector<corpus::Metadata>& metaset, bool cancelIfAlreadyReduced );
	std::vector<corpus::Metadata> PackSettingsIntoSet ( );

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

	bool hasBeenReduced;
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
	ofxToggle mAnalysisInsertionToggle;


	ofxPanel mReductionPanel;
	ofxButton mReductionPickInputFileButton;
	ofxLabel mReductionInputLabel;
	ofxButton mReductionPickOutputFileButton;
	ofxLabel mReductionOutputLabel;
	ofxButton mConfirmReductionButton;
	ofxButton mCancelReductionButton;




	corpus::Controller mController;
	corpus::JSON mJSON;
	interface::Colors mColors;
	corpus::MetaStrings mMetaStrings;
};

} // namespace interface
} // namespace acorex