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
	void AnalyseInitial ( );
	void AnalyseInsertReplace ( );
	void AnalyseInsertKeep ( );
	void Analyse ( );

	void Reduce ( );

	void SelectAnalysisDirectory ( );
	void SelectAnalysisOutputFile ( );
	void SelectReductionInputFile ( );
	void SelectReductionOutputFile ( );

	void ResetDefaultValues ( );

	bool SetSettingsFromFile ( std::vector<corpus::Metadata>& metaset, bool cancelIfAlreadyReduced );
	std::vector<corpus::Metadata> PackSettingsIntoSet ( );

	void QuantiseWindowSize ( int& value );
	void QuantiseHopFraction ( int& value );

	void ToggleAnalysisUILockout ( bool lock );

	void ShowPanel ( ofxPanel& visiblePanel, bool& visibleDrawFlag, bool hideOthers );
	void HideAllPanels ( );

	void ShowMainPanel ( );
	void ShowAnalysisPanel ( );
	void ShowReductionPanel ( );

	corpus::Controller mController;
	corpus::JSON mJSON;
	interface::Colors mColors;
	corpus::MetaStrings mMetaStrings;

	// State --------------------------------------

	bool bDrawMainPanel = true;
	bool bDrawAnalysisPanel = false;
	bool bDrawReductionPanel = false;
	bool bDrawInsertionDuplicateQuestionPanel = false;
	
	bool bInsertingIntoCorpus = false;

	bool bAnalysisDirectorySelected = false;
	bool bAnalysisOutputSelected = false;
	bool bReductionInputSelected = false;
	bool bReductionOutputSelected = false;
	
	bool bFlashingInvalidFileSelects = false;
	bool bFlashingInvalidAnalysisToggles = false;
	int flashColour = 0;

	// Metadata -----------------------------------

	//stats - mean, standard deviation, skewness, kurtosis, low percentile, middle (median default), high percentile
	//int maxSamplingRate = 22050;

	bool hasBeenReduced = false;
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

	bool insertionReplacesDuplicates = false;

	// File Paths ---------------------------------

	std::string inputPath = "";
	std::string outputPath = "";

	// Panels -------------------------------------

	ofxPanel mMainPanel;
	ofxButton mCreateCorpusButton;
	ofxButton mReduceCorpusButton;


	ofxPanel mAnalysisPanel;
	ofxButton mAnalysisPickDirectoryButton;
	ofxLabel mAnalysisDirectoryLabel;
	ofxButton mAnalysisPickOutputFileButton;
	ofxLabel mAnalysisOutputLabel;
	ofxButton mConfirmAnalysisButton;
	ofxButton mCancelAnalysisButton;


	ofxPanel mReductionPanel;
	ofxButton mReductionPickInputFileButton;
	ofxLabel mReductionInputLabel;
	ofxButton mReductionPickOutputFileButton;
	ofxLabel mReductionOutputLabel;
	ofxButton mConfirmReductionButton;
	ofxButton mCancelReductionButton;

	ofxPanel mInsertionDuplicateQuestionPanel;
	ofxLabel mInsertionDuplicateQuestionLabel;
	ofxButton mInsertionDuplicateYesButton;
	ofxButton mInsertionDuplicateNoButton;
};

} // namespace interface
} // namespace acorex