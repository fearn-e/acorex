#include "./AnalyserMenu.h"

using namespace Acorex;

AnalyserMenu::AnalyserMenu ( )
{
	Initialise ( false );
}

void AnalyserMenu::Initialise ( bool HiDpi )
{
	// DPI ----------------------------------------
	{
		if ( HiDpi ) { mLayout.enableHiDpi ( ); }
		else { mLayout.disableHiDpi ( ); }
	}

	// Clear --------------------------------------
	{
		RemoveListeners ( );

		mMainPanel.clear ( );
		mAnalysisPanel.clear ( );
		mAnalysisMetadataPanel.clear ( );
		mAnalysisConfirmPanel.clear ( );
		mReductionPanel.clear ( );
		mAnalysisInsertionPanel.clear ( );
	}

	// Variables ----------------------------------
	{
		mHasBeenReduced = false;
		inputPath = "";
		outputPath = "";
	}

	// States -------------------------------------
	{
		bDraw = false;
		bProcessing = false;

		bDrawMainPanel = false;
		bDrawAnalysisPanel = false;
		bDrawReductionPanel = false;
		bDrawAnalysisInsertionPanel = false;

		bInsertingIntoCorpus = false;

		bAnalysisDirectorySelected = false;
		bAnalysisOutputSelected = false;
		bReductionInputSelected = false;
		bReductionOutputSelected = false;

		bFlashingInvalidFileSelects = false;
		bFlashingInvalidAnalysisToggles = false;
		bFlashingInvalidReductionDimensions = false;

		flashColour = 255;
	}

	// Main Panel ---------------------------------
	{

		mMainPanel.setup ( "Menu" );

		mMainPanel.add ( mCreateCorpusButton.setup ( "Analyse Corpus" ) );
		mMainPanel.add ( mReduceCorpusButton.setup ( "Reduce Corpus" ) );

		mCreateCorpusButton.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mReduceCorpusButton.setBackgroundColor ( mColors.interfaceBackgroundColor );

		mMainPanel.setPosition ( mLayout.hiddenPanelPosition );
		mMainPanel.setWidthElements ( mLayout.analyseMainPanelWidth );
		mMainPanel.disableHeader ( );
	}

	// Analysis Panel -----------------------------
	{
		mAnalysisPanel.setup ( "Analysis" );

		mAnalysisPanel.add ( mAnalysisPickDirectoryButton.setup ( "Pick Audio Directory" ) );
		mAnalysisPanel.add ( mAnalysisDirectoryLabel.setup ( " ", "?" ) );
		mAnalysisPanel.add ( mAnalysisPickOutputFileButton.setup ( "Pick Output File" ) );
		mAnalysisPanel.add ( mAnalysisOutputLabel.setup ( "", "?" ) );

		mAnalysisPickDirectoryButton.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mAnalysisDirectoryLabel.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mAnalysisPickOutputFileButton.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mAnalysisOutputLabel.setBackgroundColor ( mColors.interfaceBackgroundColor );

		mAnalysisPanel.setPosition ( mLayout.hiddenPanelPosition );
		mAnalysisPanel.setWidthElements ( mLayout.analyseAnalysisPanelWidth );
		mAnalysisPanel.disableHeader ( );


		mAnalysisMetadataPanel.setup ( "Analysis Metadata" );

		mAnalysisMetadataPanel.add ( mTimeDimensionToggle.setup ( "Analyse Time", true ) );

		mAnalysisMetadataPanel.add ( mAnalysisPitchToggle.setup ( "Analyse Pitch", true ) );
		mAnalysisMetadataPanel.add ( mAnalysisLoudnessToggle.setup ( "Analyse Loudness", true ) );
		mAnalysisMetadataPanel.add ( mAnalysisShapeToggle.setup ( "Analyse Shape", false ) );
		mAnalysisMetadataPanel.add ( mAnalysisMFCCToggle.setup ( "Analyse MFCC", false ) );

		mAnalysisMetadataPanel.add ( mWindowFFTField.setup ( "Window Size: ", 1024, 512, 8192 ) );
		mAnalysisMetadataPanel.add ( mHopFractionField.setup ( "Hop Size (Fraction of Window):  1 / ", 2, 1, 16 ) );
		mAnalysisMetadataPanel.add ( mNBandsField.setup ( "Bands: ", 40, 1, 100 ) );
		mAnalysisMetadataPanel.add ( mNCoefsField.setup ( "Coefs: ", 13, 1, 20 ) );
		mAnalysisMetadataPanel.add ( mMinFreqField.setup ( "Min Freq: ", 20, 20, 2000 ) );
		mAnalysisMetadataPanel.add ( mMaxFreqField.setup ( "Max Freq: ", 5000, 20, 20000 ) );

		mTimeDimensionToggle.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mAnalysisPitchToggle.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mAnalysisLoudnessToggle.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mAnalysisShapeToggle.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mAnalysisMFCCToggle.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mWindowFFTField.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mHopFractionField.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mNBandsField.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mNCoefsField.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mMinFreqField.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mMaxFreqField.setBackgroundColor ( mColors.interfaceBackgroundColor );

		mAnalysisMetadataPanel.setPosition ( mLayout.hiddenPanelPosition );
		mAnalysisMetadataPanel.setWidthElements ( mLayout.analyseAnalysisPanelWidth );
		mAnalysisMetadataPanel.disableHeader ( );


		mAnalysisConfirmPanel.setup ( "Confirm Analysis" );

		mAnalysisConfirmPanel.add ( mConfirmAnalysisButton.setup ( "Confirm" ) );
		mAnalysisConfirmPanel.add ( mCancelAnalysisButton.setup ( "Cancel" ) );

		mConfirmAnalysisButton.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mCancelAnalysisButton.setBackgroundColor ( mColors.interfaceBackgroundColor );

		mAnalysisConfirmPanel.setPosition ( mLayout.hiddenPanelPosition );
		mAnalysisConfirmPanel.setWidthElements ( mLayout.analyseAnalysisPanelWidth );
		mAnalysisConfirmPanel.disableHeader ( );
	}

	// Insertion Duplicate Question Panel ---------
	{
		mAnalysisInsertionPanel.setup ( "Insertion Question" );

		mAnalysisInsertionPanel.add ( mAnalysisInsertionQuestionLabel.setup ( "For files already existing in the set, which version to use?", "" ) );
		mAnalysisInsertionPanel.add ( mAnalysisInsertionReplaceWithNewToggle.setup ( "Existing Files", false ) );

		mAnalysisInsertionQuestionLabel.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mAnalysisInsertionReplaceWithNewToggle.setBackgroundColor ( mColors.interfaceBackgroundColor );

		mAnalysisInsertionPanel.setPosition ( mLayout.hiddenPanelPosition );
		mAnalysisInsertionPanel.setWidthElements ( mLayout.analyseAnalysisPanelWidth );
		mAnalysisInsertionPanel.disableHeader ( );
	}

	// Reduction Panel ----------------------------
	{
		mReductionPanel.setup ( "Reduction" );

		mReductionPanel.add ( mReductionPickInputFileButton.setup ( "Pick Input File" ) );
		mReductionPanel.add ( mReductionInputLabel.setup ( " ", "?" ) );
		mReductionPanel.add ( mReductionPickOutputFileButton.setup ( "Pick Output File" ) );
		mReductionPanel.add ( mReductionOutputLabel.setup ( "", "?" ) );

		mReductionPanel.add ( mReducedDimensionsField.setup ( "Reduced Dimensions", 4, 2, 32 ) );
		mReductionPanel.add ( mMaxIterationsField.setup ( "Max Training Iterations", 200, 1, 1000 ) );

		mReductionPanel.add ( mConfirmReductionButton.setup ( "Confirm" ) );
		mReductionPanel.add ( mCancelReductionButton.setup ( "Cancel" ) );

		mReductionPickInputFileButton.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mReductionInputLabel.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mReductionPickOutputFileButton.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mReductionOutputLabel.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mReducedDimensionsField.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mMaxIterationsField.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mConfirmReductionButton.setBackgroundColor ( mColors.interfaceBackgroundColor );
		mCancelReductionButton.setBackgroundColor ( mColors.interfaceBackgroundColor );

		mReductionPanel.setPosition ( mLayout.hiddenPanelPosition );
		mReductionPanel.setWidthElements ( mLayout.analyseReductionPanelWidth );
		mReductionPanel.disableHeader ( );
	}

	// Listeners ----------------------------------
	{
		mCreateCorpusButton.addListener ( this, &AnalyserMenu::ShowAnalysisPanel );
		mReduceCorpusButton.addListener ( this, &AnalyserMenu::ShowReductionPanel );
		mCancelAnalysisButton.addListener ( this, &AnalyserMenu::ShowMainPanel );
		mCancelReductionButton.addListener ( this, &AnalyserMenu::ShowMainPanel );
		mAnalysisPickDirectoryButton.addListener ( this, &AnalyserMenu::SelectAnalysisDirectory );
		mAnalysisPickOutputFileButton.addListener ( this, &AnalyserMenu::SelectAnalysisOutputFile );
		mReductionPickInputFileButton.addListener ( this, &AnalyserMenu::SelectReductionInputFile );
		mReductionPickOutputFileButton.addListener ( this, &AnalyserMenu::SelectReductionOutputFile );
		mWindowFFTField.addListener ( this, &AnalyserMenu::QuantiseWindowSize );
		mHopFractionField.addListener ( this, &AnalyserMenu::QuantiseHopFraction );
		mConfirmAnalysisButton.addListener ( this, &AnalyserMenu::Analyse );
		mConfirmReductionButton.addListener ( this, &AnalyserMenu::Reduce );
		mAnalysisInsertionReplaceWithNewToggle.addListener ( this, &AnalyserMenu::AnalysisInsertionToggleChanged );
	}

	ToggleAnalysisUILockout ( false );
}

void AnalyserMenu::Show ( )
{
	ShowMainPanel ( );
}

// fully resets all values and hides the menu
void AnalyserMenu::Hide ( )
{
	Initialise ( mLayout.HiDpi );
}

void AnalyserMenu::Draw ( )
{
	if ( !bDraw ) { return; }

	if ( bDrawMainPanel )
	{
		//draw background rectangle around main panel
		ofSetColor ( mColors.interfaceBackgroundColor );
		ofDrawRectangle ( 
			mMainPanel.getPosition ( ).x - mLayout.panelBackgroundMargin, 
			mMainPanel.getPosition ( ).y - mLayout.panelBackgroundMargin, 
			mMainPanel.getWidth ( ) + mLayout.panelBackgroundMargin * 2,
			mMainPanel.getHeight ( ) + mLayout.panelBackgroundMargin * 2 );
		mMainPanel.draw ( );
	}

	if ( bDrawAnalysisPanel )
	{
		int backgroundHeight = mAnalysisPanel.getHeight ( ) + mAnalysisMetadataPanel.getHeight ( ) + mAnalysisConfirmPanel.getHeight ( ) + mLayout.interPanelSpacing * 2;

		mAnalysisMetadataPanel.setPosition ( mAnalysisPanel.getPosition ( ).x, mAnalysisPanel.getPosition ( ).y + mAnalysisPanel.getHeight ( ) + mLayout.interPanelSpacing );
		mAnalysisConfirmPanel.setPosition ( mAnalysisMetadataPanel.getPosition ( ).x, mAnalysisMetadataPanel.getPosition ( ).y + mAnalysisMetadataPanel.getHeight ( ) + mLayout.interPanelSpacing );
		if ( bDrawAnalysisInsertionPanel )
		{
			backgroundHeight += mAnalysisInsertionPanel.getHeight ( ) + mLayout.interPanelSpacing;
			mAnalysisInsertionPanel.setPosition ( mAnalysisConfirmPanel.getPosition ( ).x, mAnalysisConfirmPanel.getPosition ( ).y + mAnalysisConfirmPanel.getHeight ( ) + mLayout.interPanelSpacing );
		}
		
		ofSetColor ( mColors.interfaceBackgroundColor );
		ofDrawRectangle (
			mAnalysisPanel.getPosition ( ).x - mLayout.panelBackgroundMargin, 
			mAnalysisPanel.getPosition ( ).y - mLayout.panelBackgroundMargin, 
			mAnalysisPanel.getWidth ( ) + mLayout.panelBackgroundMargin * 2,
			backgroundHeight + mLayout.panelBackgroundMargin * 2 );

		mAnalysisPanel.draw ( );
		mAnalysisMetadataPanel.draw ( );
		mAnalysisConfirmPanel.draw ( );
		if ( bDrawAnalysisInsertionPanel ) { mAnalysisInsertionPanel.draw ( ); }
	}

	if ( bDrawReductionPanel ) 
	{
		ofSetColor ( mColors.interfaceBackgroundColor );
		ofDrawRectangle (
			mReductionPanel.getPosition ( ).x - mLayout.panelBackgroundMargin,
			mReductionPanel.getPosition ( ).y - mLayout.panelBackgroundMargin,
			mReductionPanel.getWidth ( ) + mLayout.panelBackgroundMargin * 2,
			mReductionPanel.getHeight ( ) + mLayout.panelBackgroundMargin * 2 );

		mReductionPanel.draw ( );
	}

	if ( bFlashingInvalidFileSelects )
	{
		if ( bDrawAnalysisPanel && !bAnalysisDirectorySelected )
		{
			mAnalysisDirectoryLabel.setBackgroundColor ( ofColor ( flashColour, 0, 0 ) );
		}
		if ( bDrawAnalysisPanel && !bAnalysisOutputSelected )
		{
			mAnalysisOutputLabel.setBackgroundColor ( ofColor ( flashColour, 0, 0 ) );
		}
		if ( bDrawReductionPanel && !bReductionInputSelected )
		{
			mReductionInputLabel.setBackgroundColor ( ofColor ( flashColour, 0, 0 ) );
		}
		if ( bDrawReductionPanel && !bReductionOutputSelected )
		{
			mReductionOutputLabel.setBackgroundColor ( ofColor ( flashColour, 0, 0 ) );
		}

		if ( flashColour <= 0 )
		{
			flashColour = 255;
			bFlashingInvalidFileSelects = false;
			mAnalysisDirectoryLabel.setBackgroundColor ( mColors.interfaceBackgroundColor );
			mAnalysisOutputLabel.setBackgroundColor ( mColors.interfaceBackgroundColor );
			mReductionInputLabel.setBackgroundColor ( mColors.interfaceBackgroundColor );
			mReductionOutputLabel.setBackgroundColor ( mColors.interfaceBackgroundColor );
		}
	}

	if ( bFlashingInvalidAnalysisToggles )
	{
		mAnalysisLoudnessToggle.setBackgroundColor ( ofColor ( flashColour, 0, 0 ) );
		mAnalysisPitchToggle.setBackgroundColor ( ofColor ( flashColour, 0, 0 ) );
		mAnalysisShapeToggle.setBackgroundColor ( ofColor ( flashColour, 0, 0 ) );
		mAnalysisMFCCToggle.setBackgroundColor ( ofColor ( flashColour, 0, 0 ) );

		if ( flashColour <= 0 )
		{
			flashColour = 255;
			bFlashingInvalidAnalysisToggles = false;
			mAnalysisLoudnessToggle.setBackgroundColor ( mColors.interfaceBackgroundColor );
			mAnalysisPitchToggle.setBackgroundColor ( mColors.interfaceBackgroundColor );
			mAnalysisShapeToggle.setBackgroundColor ( mColors.interfaceBackgroundColor );
			mAnalysisMFCCToggle.setBackgroundColor ( mColors.interfaceBackgroundColor );
		}
	}

	if ( bFlashingInvalidReductionDimensions )
	{
		mReducedDimensionsField.setBackgroundColor ( ofColor ( flashColour, 0, 0 ) );

		if ( flashColour <= 0 )
		{
			flashColour = 255;
			bFlashingInvalidReductionDimensions = false;
			mReducedDimensionsField.setBackgroundColor ( mColors.interfaceBackgroundColor );
		}
	
	}

	if ( bFlashingInvalidAnalysisToggles || bFlashingInvalidFileSelects || bFlashingInvalidReductionDimensions )
	{
		flashColour -= 2;
		if ( flashColour < 0 ) { flashColour = 0; }
	}
}

void AnalyserMenu::Exit ( )
{
	RemoveListeners ( );
}

void AnalyserMenu::RemoveListeners ( )
{
	mCreateCorpusButton.removeListener ( this, &AnalyserMenu::ShowAnalysisPanel );
	mReduceCorpusButton.removeListener ( this, &AnalyserMenu::ShowReductionPanel );
	mCancelAnalysisButton.removeListener ( this, &AnalyserMenu::ShowMainPanel );
	mCancelReductionButton.removeListener ( this, &AnalyserMenu::ShowMainPanel );
	mAnalysisPickDirectoryButton.removeListener ( this, &AnalyserMenu::SelectAnalysisDirectory );
	mAnalysisPickOutputFileButton.removeListener ( this, &AnalyserMenu::SelectAnalysisOutputFile );
	mReductionPickInputFileButton.removeListener ( this, &AnalyserMenu::SelectReductionInputFile );
	mReductionPickOutputFileButton.removeListener ( this, &AnalyserMenu::SelectReductionOutputFile );
	mWindowFFTField.removeListener ( this, &AnalyserMenu::QuantiseWindowSize );
	mHopFractionField.removeListener ( this, &AnalyserMenu::QuantiseHopFraction );
	mConfirmAnalysisButton.removeListener ( this, &AnalyserMenu::Analyse );
	mConfirmReductionButton.removeListener ( this, &AnalyserMenu::Reduce );
	mAnalysisInsertionReplaceWithNewToggle.removeListener ( this, &AnalyserMenu::AnalysisInsertionToggleChanged );
}

// Analyse and Reduce ---------------------------

void AnalyserMenu::Analyse ( )
{
	if ( !bAnalysisDirectorySelected || !bAnalysisOutputSelected )
	{
		flashColour = 255;
		bFlashingInvalidFileSelects = true;
		ofLogError ( "AnalyserMenu" ) << "Analysis directory or output file not selected";
		return;
	}

	if ( !mAnalysisLoudnessToggle && !mAnalysisPitchToggle && !mAnalysisShapeToggle && !mAnalysisMFCCToggle )
	{
		flashColour = 255;
		bFlashingInvalidAnalysisToggles = true;
		ofLogError ( "AnalyserMenu" ) << "No analysis types selected";
		return;
	}

	bProcessing = true;

	bool success = false;
	if ( !bInsertingIntoCorpus )
	{
		Utils::AnalysisSettings settings;
		PackSettingsFromUser ( settings );
		success = mController.CreateCorpus ( inputPath, outputPath, settings );
	}
	else
	{
		success = mController.InsertIntoCorpus ( inputPath, outputPath, mAnalysisInsertionReplaceWithNewToggle );
	}

	bProcessing = false;

	if ( !success )
	{
		ShowMainPanel ( );
		return;
	}

	// TODO - ask if user wants to reduce the data or view it in the corpus viewer
	ShowMainPanel ( );
	ofLogNotice ( "AnalyserMenu" ) << "Corpus created";
	//------------------------------------------------ TEMPORARY
}

void AnalyserMenu::Reduce ( )
{
	if ( !bReductionInputSelected || !bReductionOutputSelected )
	{
		flashColour = 255;
		bFlashingInvalidFileSelects = true;
		ofLogError ( "AnalyserMenu" ) << "Reduction input or output file not selected";
		return;
	}

	if ( mReducedDimensionsField >= mCurrentDimensionCount )
	{
		flashColour = 255;
		bFlashingInvalidReductionDimensions = true;
		ofLogError ( "AnalyserMenu" ) << "Can't reduce to more dimensions than currently exist";
		return;
	}

	bProcessing = true;

	bool success = false;
	Utils::ReductionSettings settings;
	PackSettingsFromUser ( settings );
	success = mController.ReduceCorpus ( inputPath, outputPath, settings );

	bProcessing = false;

	if ( !success )
	{
		ShowMainPanel ( );
		return;
	}

	// TODO - ask if user wants to open the reduced data in the corpus viewer
	ShowMainPanel ( );
	ofLogNotice ( "AnalyserMenu" ) << "Corpus reduced";
	//------------------------------------------------ TEMPORARY
}

// File Dialog Button Callbacks -----------------

void AnalyserMenu::SelectAnalysisDirectory ( )
{
	ofFileDialogResult audioDirectory = ofSystemLoadDialog ( "Select folder containing audio files...", true, ofFilePath::getCurrentWorkingDirectory ( ) );
	if ( !audioDirectory.bSuccess )
	{
		ofLogError ( "AnalyserMenu" ) << "No folder selected";
		return;
	}
	if ( !ofDirectory::doesDirectoryExist ( audioDirectory.getPath ( ) ) )
	{
		ofLogError ( "AnalyserMenu" ) << "Invalid directory";
		return;
	}

	inputPath = audioDirectory.getPath ( );
	mAnalysisDirectoryLabel = audioDirectory.getName ( );
	bAnalysisDirectorySelected = true;
}

//TODO - fix windows save dialog defaults
	//#ifdef _WIN32
	//auto resultOut = pfd::save_file::save_file("Saving reduced analysis as...", "reduced_corpus.json", { "JSON Files", "*.json" });
	//std::string resultOutPath = resultOut.result();
	//#endif
void AnalyserMenu::SelectAnalysisOutputFile ( )
{
	ofFileDialogResult outputFile = ofSystemSaveDialog ( "acorex_corpus.json", "Save analysed corpus as..." );
	if ( !outputFile.bSuccess )
	{
		ofLogError ( "AnalyserMenu" ) << "Invalid save query";
		return;
	}
	if ( outputFile.getName ( ).find ( ".json" ) == std::string::npos )
	{
		ofLogNotice ( "AnalyserMenu" ) << "Added missing .json extension";
		outputFile.filePath += ".json";
		outputFile.fileName += ".json";
	}

	if ( ofFile::doesFileExist ( outputFile.getPath ( ) ) )
	{
		bInsertingIntoCorpus = true;
		ShowAnalysisInsertionPanel ( );
	}
	else
	{
		bInsertingIntoCorpus = false;
		HideAnalysisInsertionPanel ( );
	}

	if ( bInsertingIntoCorpus )
	{
		Utils::AnalysisSettings settings;
		bool success = mJSON.Read ( outputFile.getPath ( ), settings );
		if ( !success ) { return; }

		if ( settings.hasBeenReduced )
		{
			ofLogError ( "AnalyserMenu" ) << "Can't insert into an already reduced dataset";
			return;
		}

		UnpackSettingsFromFile ( settings );
	}

	ToggleAnalysisUILockout ( bInsertingIntoCorpus );
	outputPath = outputFile.getPath ( );
	mAnalysisOutputLabel = outputFile.getName ( );
	bAnalysisOutputSelected = true;
}

void AnalyserMenu::SelectReductionInputFile ( )
{
	ofFileDialogResult inputFile = ofSystemLoadDialog ( "Select a corpus file...", false, ofFilePath::getCurrentWorkingDirectory ( ) );
	if ( !inputFile.bSuccess )
	{
		ofLogError ( "AnalyserMenu" ) << "No file selected";
		return;
	}
	if ( !ofFile::doesFileExist ( inputFile.getPath ( ) ) )
	{
		ofLogError ( "AnalyserMenu" ) << "Invalid file";
		return;
	}
	if ( inputFile.getName ( ).find ( ".json" ) == std::string::npos )
	{
		ofLogError ( "AnalyserMenu" ) << "Invalid file extension";
		return;
	}

	Utils::AnalysisSettings settings;
	bool success = mJSON.Read ( inputFile.getPath ( ), settings );
	if ( !success ) { return; }
	if ( settings.currentDimensionCount <= 2 )
	{
		ofLogError ( "AnalyserMenu" ) << "Analysis already contains the minimum number of dimensions";
		return;
	}

	UnpackSettingsFromFile ( settings );
	inputPath = inputFile.getPath ( );
	mReductionInputLabel = inputFile.getName ( );
	bReductionInputSelected = true;
}

//TODO - fix windows save dialog defaults
	//#ifdef _WIN32
	//auto resultOut = pfd::save_file::save_file("Saving reduced analysis as...", "reduced_corpus.json", { "JSON Files", "*.json" });
	//std::string resultOutPath = resultOut.result();
	//#endif
void AnalyserMenu::SelectReductionOutputFile ( )
{
	ofFileDialogResult outputFile = ofSystemSaveDialog ( "acorex_corpus_reduced.json", "Save reduced corpus as..." );
	if ( !outputFile.bSuccess )
	{
		ofLogError ( "AnalyserMenu" ) << "Invalid save query";
		return;
	}
	if ( outputFile.getName ( ).find ( ".json" ) == std::string::npos )
	{
		ofLogNotice ( "AnalyserMenu" ) << "Added missing .json extension";
		outputFile.filePath += ".json";
		outputFile.fileName += ".json";
	}

	outputPath = outputFile.getPath ( );
	mReductionOutputLabel = outputFile.getName ( );
	bReductionOutputSelected = true;
}

// Load and Save Settings -----------------------

void AnalyserMenu::UnpackSettingsFromFile ( const Utils::AnalysisSettings& settings )
{
	mTimeDimensionToggle = settings.bTime;
	mAnalysisPitchToggle = settings.bPitch;
	mAnalysisLoudnessToggle = settings.bLoudness;
	mAnalysisShapeToggle = settings.bShape;
	mAnalysisMFCCToggle = settings.bMFCC;
	mWindowFFTField = settings.windowFFTSize;
	mHopFractionField = settings.hopFraction;
	mNBandsField = settings.nBands;
	mNCoefsField = settings.nCoefs;
	mMinFreqField = settings.minFreq;
	mMaxFreqField = settings.maxFreq;
	mCurrentDimensionCount = settings.currentDimensionCount;

#ifndef DATA_CHANGE_CHECK_1
#error "check if this implementation is still valid for the data struct"
#endif // !DATA_CHANGE_CHECK_1
}

void AnalyserMenu::PackSettingsFromUser ( Utils::AnalysisSettings& settings )
{
	settings.bTime = mTimeDimensionToggle;
	settings.bPitch = mAnalysisPitchToggle;
	settings.bLoudness = mAnalysisLoudnessToggle;
	settings.bShape = mAnalysisShapeToggle;
	settings.bMFCC = mAnalysisMFCCToggle;
	settings.windowFFTSize = mWindowFFTField;
	settings.hopFraction = mHopFractionField;
	settings.nBands = mNBandsField;
	settings.nCoefs = mNCoefsField;
	settings.minFreq = mMinFreqField;
	settings.maxFreq = mMaxFreqField;


#ifndef DATA_CHANGE_CHECK_1
#error "check if this implementation is still valid for the data struct"
#endif // !DATA_CHANGE_CHECK_1
}

void AnalyserMenu::PackSettingsFromUser ( Utils::ReductionSettings& settings )
{
	settings.dimensionReductionTarget = mReducedDimensionsField;
	settings.maxIterations = mMaxIterationsField;

#ifndef DATA_CHANGE_CHECK_1
#error "check if this implementation is still valid for the data struct"
#endif // !DATA_CHANGE_CHECK_1
}


// UI Value Management -------------------------------

void AnalyserMenu::QuantiseWindowSize ( int& value )
{
	//find closest power of 2 between 512 and 8192
	int closest = 512;
	int diff = abs ( value - 512 );
	for ( int i = 1024; i <= 8192; i *= 2 )
	{
		int newdiff = abs ( value - i );
		if ( newdiff < diff )
		{
			closest = i;
			diff = newdiff;
		}
	}
	mWindowFFTField.setup ( mWindowFFTField.getName(), closest, mWindowFFTField.getMin(), mWindowFFTField.getMax() );
	mAnalysisMetadataPanel.setWidthElements ( mAnalysisMetadataPanel.getWidth ( ) );
	mAnalysisMetadataPanel.setPosition ( mAnalysisMetadataPanel.getPosition ( ) );
}

void AnalyserMenu::QuantiseHopFraction ( int& value )
{
	//find closest power of 2 between 1 and 16
	int closest = 1;
	int diff = abs ( value - 1 );
	for ( int i = 2; i <= 16; i *= 2 )
	{
		int newdiff = abs ( value - i );
		if ( newdiff < diff )
		{
			closest = i;
			diff = newdiff;
		}
	}
	mHopFractionField.setup ( mHopFractionField.getName ( ), closest, mHopFractionField.getMin ( ), mHopFractionField.getMax ( ) );
	mAnalysisMetadataPanel.setWidthElements ( mAnalysisMetadataPanel.getWidth ( ) );
	mAnalysisMetadataPanel.setPosition ( mAnalysisMetadataPanel.getPosition ( ) );
}

void AnalyserMenu::AnalysisInsertionToggleChanged ( bool& value )
{
	if ( value ) { mAnalysisInsertionReplaceWithNewToggle.setName ( "New Files" ); }
	else { mAnalysisInsertionReplaceWithNewToggle.setName ( "Existing Files" ); }
}

// Panel Management ------------------------------

void AnalyserMenu::ToggleAnalysisUILockout ( bool lock )
{
	mTimeDimensionToggle.setTextColor ( lock ? mColors.lockedTextColor : mColors.normalTextColor );
	mAnalysisPitchToggle.setTextColor ( lock ? mColors.lockedTextColor : mColors.normalTextColor );
	mAnalysisLoudnessToggle.setTextColor ( lock ? mColors.lockedTextColor : mColors.normalTextColor );
	mAnalysisShapeToggle.setTextColor ( lock ? mColors.lockedTextColor : mColors.normalTextColor );
	mAnalysisMFCCToggle.setTextColor ( lock ? mColors.lockedTextColor : mColors.normalTextColor );
	mWindowFFTField.setTextColor ( lock ? mColors.lockedTextColor : mColors.normalTextColor );
	mHopFractionField.setTextColor ( lock ? mColors.lockedTextColor : mColors.normalTextColor );
	mNBandsField.setTextColor ( lock ? mColors.lockedTextColor : mColors.normalTextColor );
	mNCoefsField.setTextColor ( lock ? mColors.lockedTextColor : mColors.normalTextColor );
	mMinFreqField.setTextColor ( lock ? mColors.lockedTextColor : mColors.normalTextColor );
	mMaxFreqField.setTextColor ( lock ? mColors.lockedTextColor : mColors.normalTextColor );

	if ( lock )
	{
		mAnalysisMetadataPanel.unregisterMouseEvents ( );
		mWindowFFTField.unregisterMouseEvents ( );
		mHopFractionField.unregisterMouseEvents ( );
	}
	else
	{
		mAnalysisMetadataPanel.registerMouseEvents ( );
		mWindowFFTField.registerMouseEvents ( );
		mHopFractionField.registerMouseEvents ( );
	}
}

void AnalyserMenu::ShowMainPanel ( )
{
	Initialise ( mLayout.HiDpi );
	bDraw = true;
	bDrawMainPanel = true;
	mMainPanel.setPosition ( mLayout.analysePanelOriginX, mLayout.analysePanelOriginY );
}

void AnalyserMenu::ShowAnalysisPanel ( )
{
	Initialise ( mLayout.HiDpi );
	bDraw = true;
	bDrawAnalysisPanel = true;
	mAnalysisPanel.setPosition ( mLayout.analysePanelOriginX, mLayout.analysePanelOriginY );
	mAnalysisMetadataPanel.setPosition ( mAnalysisPanel.getPosition ( ).x, mAnalysisPanel.getPosition ( ).y + mAnalysisPanel.getHeight ( ) + mLayout.interPanelSpacing );
	mAnalysisConfirmPanel.setPosition ( mAnalysisMetadataPanel.getPosition ( ).x, mAnalysisMetadataPanel.getPosition ( ).y + mAnalysisMetadataPanel.getHeight ( ) + mLayout.interPanelSpacing );
}

void AnalyserMenu::ShowAnalysisInsertionPanel ( )
{
	bDrawAnalysisInsertionPanel = true;
	mAnalysisInsertionPanel.setPosition ( mAnalysisConfirmPanel.getPosition ( ).x, mAnalysisConfirmPanel.getPosition ( ).y + mAnalysisConfirmPanel.getHeight ( ) + mLayout.interPanelSpacing );
}

void AnalyserMenu::HideAnalysisInsertionPanel ( )
{
	bDrawAnalysisInsertionPanel = false;
	mAnalysisInsertionPanel.setPosition ( mLayout.hiddenPanelPosition );
}

void AnalyserMenu::ShowReductionPanel ( )
{
	Initialise ( mLayout.HiDpi );
	bDraw = true;
	bDrawReductionPanel = true;
	mReductionPanel.setPosition ( mLayout.analysePanelOriginX, mLayout.analysePanelOriginY );
}
