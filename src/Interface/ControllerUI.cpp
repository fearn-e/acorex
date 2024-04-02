#pragma once

#include "Interface/ControllerUI.h"

void AcorexInterface::ControllerUI::setup ( )
{
	ShowMainPanel ( );
}

void AcorexInterface::ControllerUI::Reset ( )
{
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

		flashColour = 255;
	}

	// Main Panel ---------------------------------
	{

		mMainPanel.setup ( "Menu" );

		mMainPanel.add ( mCreateCorpusButton.setup ( "Analyse Corpus" ) );
		mMainPanel.add ( mReduceCorpusButton.setup ( "Reduce Corpus" ) );

		mMainPanel.setPosition ( mLayout.hiddenPanelPosition );
		mMainPanel.setWidthElements ( mLayout.mainPanelWidth );
		mMainPanel.disableHeader ( );
	}

	// Analysis Panel -----------------------------
	{
		mAnalysisPanel.setup ( "Analysis" );

		mAnalysisPanel.add ( mAnalysisPickDirectoryButton.setup ( "Pick Audio Directory" ) );
		mAnalysisPanel.add ( mAnalysisDirectoryLabel.setup ( "", "?" ) );
		mAnalysisPanel.add ( mAnalysisPickOutputFileButton.setup ( "Pick Output File" ) );
		mAnalysisPanel.add ( mAnalysisOutputLabel.setup ( " ", "?" ) );

		mAnalysisPanel.setPosition ( mLayout.hiddenPanelPosition );
		mAnalysisPanel.setWidthElements ( 315 );
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

		mAnalysisMetadataPanel.setPosition ( mLayout.hiddenPanelPosition );
		mAnalysisMetadataPanel.setWidthElements ( mLayout.analysisPanelWidth );
		mAnalysisMetadataPanel.disableHeader ( );


		mAnalysisConfirmPanel.setup ( "Confirm Analysis" );

		mAnalysisConfirmPanel.add ( mConfirmAnalysisButton.setup ( "Confirm" ) );
		mAnalysisConfirmPanel.add ( mCancelAnalysisButton.setup ( "Cancel" ) );

		mAnalysisConfirmPanel.setPosition ( mLayout.hiddenPanelPosition );
		mAnalysisConfirmPanel.setWidthElements ( mLayout.analysisPanelWidth );
		mAnalysisConfirmPanel.disableHeader ( );
	}

	// Insertion Duplicate Question Panel ---------
	{
		mAnalysisInsertionPanel.setup ( "Insertion Question" );

		mAnalysisInsertionPanel.add ( mAnalysisInsertionQuestionLabel.setup ( "For files already existing in the set, which version to use?", "" ) );
		mAnalysisInsertionPanel.add ( mAnalysisInsertionReplaceWithNewToggle.setup ( "Existing Files", false ) );

		mAnalysisInsertionPanel.setPosition ( mLayout.hiddenPanelPosition );
		mAnalysisInsertionPanel.setWidthElements ( mLayout.analysisPanelWidth );
		mAnalysisInsertionPanel.disableHeader ( );
	}

	// Reduction Panel ----------------------------
	{
		mReductionPanel.setup ( "Reduction" );

		mReductionPanel.add ( mReductionPickInputFileButton.setup ( "Pick Input File" ) );
		mReductionPanel.add ( mReductionInputLabel.setup ( "", "?" ) );
		mReductionPanel.add ( mReductionPickOutputFileButton.setup ( "Pick Output File" ) );
		mReductionPanel.add ( mReductionOutputLabel.setup ( " ", "?" ) );

		mReductionPanel.add ( mReducedDimensionsField.setup ( "Reduced Dimensions", 3, 1, 10 ) );
		mReductionPanel.add ( mMaxIterationsField.setup ( "Max Training Iterations", 200, 1, 1000 ) );

		mReductionPanel.add ( mConfirmReductionButton.setup ( "Confirm" ) );
		mReductionPanel.add ( mCancelReductionButton.setup ( "Cancel" ) );

		mReductionPanel.setPosition ( mLayout.hiddenPanelPosition );
		mReductionPanel.setWidthElements ( mLayout.reductionPanelWidth );
		mReductionPanel.disableHeader ( );
	}

	// Listeners ----------------------------------
	{
		mCreateCorpusButton.addListener ( this, &ControllerUI::ShowAnalysisPanel );
		mReduceCorpusButton.addListener ( this, &ControllerUI::ShowReductionPanel );
		mCancelAnalysisButton.addListener ( this, &ControllerUI::ShowMainPanel );
		mCancelReductionButton.addListener ( this, &ControllerUI::ShowMainPanel );
		mAnalysisPickDirectoryButton.addListener ( this, &ControllerUI::SelectAnalysisDirectory );
		mAnalysisPickOutputFileButton.addListener ( this, &ControllerUI::SelectAnalysisOutputFile );
		mReductionPickInputFileButton.addListener ( this, &ControllerUI::SelectReductionInputFile );
		mReductionPickOutputFileButton.addListener ( this, &ControllerUI::SelectReductionOutputFile );
		mWindowFFTField.addListener ( this, &ControllerUI::QuantiseWindowSize );
		mHopFractionField.addListener ( this, &ControllerUI::QuantiseHopFraction );
		mConfirmAnalysisButton.addListener ( this, &ControllerUI::Analyse );
		mConfirmReductionButton.addListener ( this, &ControllerUI::Reduce );
		mAnalysisInsertionReplaceWithNewToggle.addListener ( this, &ControllerUI::AnalysisInsertionToggleChanged );
	}

	ToggleAnalysisUILockout ( false );
}

void AcorexInterface::ControllerUI::draw ( )
{
	if ( bDrawMainPanel )
	{
		//draw background rectangle around main panel
		ofSetColor ( mColors.panelBackgroundColor );
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
		
		ofSetColor ( mColors.panelBackgroundColor );
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
		ofSetColor ( mColors.panelBackgroundColor );
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

		//ofDrawBitmapString ( "Invalid field values", 40, 40 );

		flashColour = flashColour - 2;
		if ( flashColour < 0 )
		{
			flashColour = 255;
			bFlashingInvalidFileSelects = false;
			mAnalysisDirectoryLabel.setBackgroundColor ( ofColor ( 0 ) );
			mAnalysisOutputLabel.setBackgroundColor ( ofColor ( 0 ) );
			mReductionInputLabel.setBackgroundColor ( ofColor ( 0 ) );
			mReductionOutputLabel.setBackgroundColor ( ofColor ( 0 ) );
		}
	}

	if ( bFlashingInvalidAnalysisToggles )
	{
		mAnalysisLoudnessToggle.setBackgroundColor ( ofColor ( flashColour, 0, 0 ) );
		mAnalysisPitchToggle.setBackgroundColor ( ofColor ( flashColour, 0, 0 ) );
		mAnalysisShapeToggle.setBackgroundColor ( ofColor ( flashColour, 0, 0 ) );
		mAnalysisMFCCToggle.setBackgroundColor ( ofColor ( flashColour, 0, 0 ) );

		flashColour = flashColour - 2;
		if ( flashColour < 0 )
		{
			flashColour = 255;
			bFlashingInvalidAnalysisToggles = false;
			mAnalysisLoudnessToggle.setBackgroundColor ( ofColor ( 0 ) );
			mAnalysisPitchToggle.setBackgroundColor ( ofColor ( 0 ) );
			mAnalysisShapeToggle.setBackgroundColor ( ofColor ( 0 ) );
			mAnalysisMFCCToggle.setBackgroundColor ( ofColor ( 0 ) );
		}
	}
}

void AcorexInterface::ControllerUI::exit ( )
{
	RemoveListeners ( );
}

void AcorexInterface::ControllerUI::RemoveListeners ( )
{
	mCreateCorpusButton.removeListener ( this, &ControllerUI::ShowAnalysisPanel );
	mReduceCorpusButton.removeListener ( this, &ControllerUI::ShowReductionPanel );
	mCancelAnalysisButton.removeListener ( this, &ControllerUI::ShowMainPanel );
	mCancelReductionButton.removeListener ( this, &ControllerUI::ShowMainPanel );
	mAnalysisPickDirectoryButton.removeListener ( this, &ControllerUI::SelectAnalysisDirectory );
	mAnalysisPickOutputFileButton.removeListener ( this, &ControllerUI::SelectAnalysisOutputFile );
	mReductionPickInputFileButton.removeListener ( this, &ControllerUI::SelectReductionInputFile );
	mReductionPickOutputFileButton.removeListener ( this, &ControllerUI::SelectReductionOutputFile );
	mWindowFFTField.removeListener ( this, &ControllerUI::QuantiseWindowSize );
	mHopFractionField.removeListener ( this, &ControllerUI::QuantiseHopFraction );
	mConfirmAnalysisButton.removeListener ( this, &ControllerUI::Analyse );
	mConfirmReductionButton.removeListener ( this, &ControllerUI::Reduce );
	mAnalysisInsertionReplaceWithNewToggle.removeListener ( this, &ControllerUI::AnalysisInsertionToggleChanged );
}

// Analyse and Reduce ---------------------------

void AcorexInterface::ControllerUI::Analyse ( )
{
	if ( !bAnalysisDirectorySelected || !bAnalysisOutputSelected )
	{
		flashColour = 255;
		bFlashingInvalidFileSelects = true;
		ofLogError ( "ControllerUI" ) << "Analysis directory or output file not selected";
		return;
	}

	if ( !mAnalysisLoudnessToggle && !mAnalysisPitchToggle && !mAnalysisShapeToggle && !mAnalysisMFCCToggle )
	{
		flashColour = 255;
		bFlashingInvalidAnalysisToggles = true;
		ofLogError ( "ControllerUI" ) << "No analysis types selected";
		return;
	}

	bool success = false;
	if ( !bInsertingIntoCorpus )
	{
		AcorexCorpus::AnalysisSettings settings;
		PackSettingsFromUser ( settings );
		success = mController.CreateCorpus ( inputPath, outputPath, settings );
	}
	else
	{
		success = mController.InsertIntoCorpus ( inputPath, outputPath, mAnalysisInsertionReplaceWithNewToggle );
	}

	if ( !success )
	{
		ShowMainPanel ( );
		ofLogError ( "ControllerUI" ) << "Failed to create corpus";
		return;
	}

	// TODO - ask if user wants to reduce the data or view it in the corpus viewer
	// make a new panel for this with two choices
	ShowMainPanel ( );
	ofLogNotice ( "ControllerUI" ) << "Corpus created";
	//------------------------------------------------ TEMPORARY
}

void AcorexInterface::ControllerUI::Reduce ( )
{
	if ( !bReductionInputSelected || !bReductionOutputSelected )
	{
		flashColour = 255;
		bFlashingInvalidFileSelects = true;
		ofLogError ( "ControllerUI" ) << "Reduction input or output file not selected";
		return;
	}

	if ( mReducedDimensionsField >= mCurrentDimensionCount )
	{
		flashColour = 255;
		bFlashingInvalidAnalysisToggles = true;
		ofLogError ( "ControllerUI" ) << "Can't reduce to more dimensions than currently exist";
		return;
	}

	// TODO - reduce

	// TODO - open in corpus viewer
}

// File Dialog Button Callbacks -----------------

void AcorexInterface::ControllerUI::SelectAnalysisDirectory ( )
{
	ofFileDialogResult audioDirectory = ofSystemLoadDialog ( "Select folder containing audio files...", true, ofFilePath::getCurrentWorkingDirectory ( ) );
	if ( !audioDirectory.bSuccess )
	{
		ofLogError ( "ControllerUI" ) << "No folder selected";
		return;
	}
	if ( !ofDirectory::doesDirectoryExist ( audioDirectory.getPath ( ) ) )
	{
		ofLogError ( "ControllerUI" ) << "Invalid directory";
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
void AcorexInterface::ControllerUI::SelectAnalysisOutputFile ( )
{
	ofFileDialogResult outputFile = ofSystemSaveDialog ( "acorex_corpus.json", "Save analysed corpus as..." );
	if ( !outputFile.bSuccess )
	{
		ofLogError ( "ControllerUI" ) << "Invalid save query";
		return;
	}
	if ( outputFile.getName ( ).find ( ".json" ) == std::string::npos )
	{
		ofLogError ( "ControllerUI" ) << "Invalid file extension";
		return;
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
		AcorexCorpus::AnalysisSettings settings;
		bool success = mJSON.Read ( outputFile.getPath ( ), settings );
		if ( !success )
		{
			ofLogError ( "ControllerUI" ) << "Failed to read metadata";
			return;
		}

		if ( settings.hasBeenReduced )
		{
			ofLogError ( "ControllerUI" ) << "Can't insert into an already reduced dataset";
			return;
		}

		UnpackSettingsFromFile ( settings );
	}

	ToggleAnalysisUILockout ( bInsertingIntoCorpus );
	outputPath = outputFile.getPath ( );
	mAnalysisOutputLabel = outputFile.getName ( );
	bAnalysisOutputSelected = true;
}

void AcorexInterface::ControllerUI::SelectReductionInputFile ( )
{
	ofFileDialogResult inputFile = ofSystemLoadDialog ( "Select a corpus file...", false, ofFilePath::getCurrentWorkingDirectory ( ) );
	if ( !inputFile.bSuccess )
	{
		ofLogError ( "ControllerUI" ) << "No file selected";
		return;
	}
	if ( !ofFile::doesFileExist ( inputFile.getPath ( ) ) )
	{
		ofLogError ( "ControllerUI" ) << "Invalid file";
		return;
	}
	if ( inputFile.getName ( ).find ( ".json" ) == std::string::npos )
	{
		ofLogError ( "ControllerUI" ) << "Invalid file extension";
		return;
	}

	AcorexCorpus::AnalysisSettings settings;
	bool success = mJSON.Read ( inputFile.getPath ( ), settings );

	if ( !success )
	{
		ofLogError ( "ControllerUI" ) << "Failed to read metadata";
		return;
	}
	if ( settings.currentDimensionCount <= 1 )
	{
		ofLogError ( "ControllerUI" ) << "Analysis already contains only one dimension";
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
void AcorexInterface::ControllerUI::SelectReductionOutputFile ( )
{
	ofFileDialogResult outputFile = ofSystemSaveDialog ( "acorex_corpus_reduced.json", "Save reduced corpus as..." );
	if ( !outputFile.bSuccess )
	{
		ofLogError ( "ControllerUI" ) << "Invalid save query";
		return;
	}
	if ( outputFile.getName ( ).find ( ".json" ) == std::string::npos )
	{
		ofLogError ( "ControllerUI" ) << "Invalid file extension";
		return;
	}

	outputPath = outputFile.getPath ( );
	mReductionOutputLabel = outputFile.getName ( );
	bReductionOutputSelected = true;
}

// Load and Save Settings -----------------------

void AcorexInterface::ControllerUI::UnpackSettingsFromFile ( const AcorexCorpus::AnalysisSettings& settings )
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

#ifndef DATA_CHANGE_CHECK_8
#error "check if this implementation is still valid for the data struct"
#endif // !DATA_CHANGE_CHECK_8
}

void AcorexInterface::ControllerUI::PackSettingsFromUser ( AcorexCorpus::AnalysisSettings& settings )
{
	settings.insertionReplacesDuplicates = mAnalysisInsertionReplaceWithNewToggle;
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


#ifndef DATA_CHANGE_CHECK_8
#error "check if this implementation is still valid for the data struct"
#endif // !DATA_CHANGE_CHECK_8
}

void AcorexInterface::ControllerUI::PackSettingsFromUser ( AcorexCorpus::ReductionSettings& settings )
{
	settings.dimensionReductionTarget = mReducedDimensionsField;
	settings.maxIterations = mMaxIterationsField;

#ifndef DATA_CHANGE_CHECK_8
#error "check if this implementation is still valid for the data struct"
#endif // !DATA_CHANGE_CHECK_8
}


// UI Value Management -------------------------------

void AcorexInterface::ControllerUI::QuantiseWindowSize ( int& value )
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

void AcorexInterface::ControllerUI::QuantiseHopFraction ( int& value )
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

void AcorexInterface::ControllerUI::AnalysisInsertionToggleChanged ( bool& value )
{
	if ( value ) { mAnalysisInsertionReplaceWithNewToggle.setName ( "New Files" ); }
	else { mAnalysisInsertionReplaceWithNewToggle.setName ( "Existing Files" ); }
}

// Panel Management ------------------------------

void AcorexInterface::ControllerUI::ToggleAnalysisUILockout ( bool lock )
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

void AcorexInterface::ControllerUI::ShowMainPanel ( )
{
	Reset ( );
	bDrawMainPanel = true;
	mMainPanel.setPosition ( mLayout.defaultPanelPosition );
}

void AcorexInterface::ControllerUI::ShowAnalysisPanel ( )
{
	Reset ( );
	bDrawAnalysisPanel = true;
	mAnalysisPanel.setPosition ( mLayout.defaultPanelPosition );
	mAnalysisMetadataPanel.setPosition ( mAnalysisPanel.getPosition ( ).x, mAnalysisPanel.getPosition ( ).y + mAnalysisPanel.getHeight ( ) + mLayout.interPanelSpacing );
	mAnalysisConfirmPanel.setPosition ( mAnalysisMetadataPanel.getPosition ( ).x, mAnalysisMetadataPanel.getPosition ( ).y + mAnalysisMetadataPanel.getHeight ( ) + mLayout.interPanelSpacing );
}

void AcorexInterface::ControllerUI::ShowAnalysisInsertionPanel ( )
{
	bDrawAnalysisInsertionPanel = true;
	mAnalysisInsertionPanel.setPosition ( mAnalysisConfirmPanel.getPosition ( ).x, mAnalysisConfirmPanel.getPosition ( ).y + mAnalysisConfirmPanel.getHeight ( ) + mLayout.interPanelSpacing );
}

void AcorexInterface::ControllerUI::HideAnalysisInsertionPanel ( )
{
	bDrawAnalysisInsertionPanel = false;
	mAnalysisInsertionPanel.setPosition ( mLayout.hiddenPanelPosition );
}

void AcorexInterface::ControllerUI::ShowReductionPanel ( )
{
	Reset ( );
	bDrawReductionPanel = true;
	mReductionPanel.setPosition ( mLayout.defaultPanelPosition );
}