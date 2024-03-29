#pragma once

#include "Interface/ControllerUI.h"

void AcorexInterface::ControllerUI::setup ( )
{
	Reset ( );
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
		mAnalysisInsertionPanel.add ( mAnalysisInsertionToggle.setup ( "Existing", false ) );

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
		mAnalysisInsertionToggle.addListener ( this, &ControllerUI::AnalysisInsertionToggleChanged );
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
	mAnalysisInsertionToggle.removeListener ( this, &ControllerUI::AnalysisInsertionToggleChanged );
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

	AcorexCorpus::MetaSetStruct metaset = PackSettingsFromUser ( );
	bool success = false;
	if ( !bInsertingIntoCorpus )
	{
		PackDimensionNamesIntoSet ( metaset, false );
		success = mController.CreateCorpus ( inputPath, outputPath, metaset ); // TODO - CHANGE, METASET CAN'T BE PASSED AS CONST IF FILELIST NEEDS TO BE WRITTEN TO, EITHER SEARCHDIRECTORY HERE OR PASS AS NON-CONST
	}
	else
	{
		success = mController.InsertIntoCorpus ( inputPath, outputPath, metaset ); // TODO - SAME AS ABOVE
	}

	if ( !success )
	{
		ShowMainPanel ( );
		ofLogError ( "ControllerUI" ) << "Failed to create corpus";
		return;
	}

	success = mJSON.WriteMeta ( outputPath, metaset );
	if ( !success )
	{
		ofLogError ( "ControllerUI" ) << "Failed to write metadata";
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
		AcorexCorpus::MetaSetStruct metaset;
		bool success = mJSON.ReadMeta ( outputFile.getPath ( ), metaset, false );
		if ( !success )
		{
			ofLogError ( "ControllerUI" ) << "Failed to read metadata";
			return;
		}

		success = UnpackSettingsFromFile ( metaset, true );
		if ( !success )
		{
			ofLogError ( "ControllerUI" ) << "Failed to unpack metadata";
			return;
		}
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

	AcorexCorpus::MetaSetStruct metaset;
	bool success = mJSON.ReadMeta ( inputFile.getPath ( ), metaset, false );

	if ( !success )
	{
		ofLogError ( "ControllerUI" ) << "Failed to read metadata";
		return;
	}
	if ( metaset.currentDimensionCount <= 1 )
	{
		ofLogError ( "ControllerUI" ) << "Analysis already contains only one dimension";
		return;
	}

	UnpackSettingsFromFile ( metaset, false );
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

bool AcorexInterface::ControllerUI::UnpackSettingsFromFile ( AcorexCorpus::MetaSetStruct& metaset, bool cancelIfAlreadyReduced )
{
	if ( cancelIfAlreadyReduced && metaset.isReduction )
	{
		ofLogError ( "ControllerUI" ) << "Analysis has already been reduced";
		return false;
	}

	mTimeDimensionToggle = metaset.isTimeAnalysis;
	mAnalysisPitchToggle = metaset.analysisPitch;
	mAnalysisLoudnessToggle = metaset.analysisLoudness;
	mAnalysisShapeToggle = metaset.analysisShape;
	mAnalysisMFCCToggle = metaset.analysisMFCC;
	mWindowFFTField = metaset.windowFFTSize;
	mHopFractionField = metaset.hopFraction;
	mNBandsField = metaset.nBands;
	mNCoefsField = metaset.nCoefs;
	mMinFreqField = metaset.minFreq;
	mMaxFreqField = metaset.maxFreq;
	mCurrentDimensionCount = metaset.currentDimensionCount;
	mReducedDimensionsField = metaset.dimensionReductionTarget;
	mMaxIterationsField = metaset.maxIterations;

#ifndef META_SET_SIZE_18
#error "incorrect number of metadata entries"
#endif // !META_SET_SIZE_18

	return true;
}

AcorexCorpus::MetaSetStruct AcorexInterface::ControllerUI::PackSettingsFromUser ( )
{
	AcorexCorpus::MetaSetStruct metaset;

	metaset.isReduction = mHasBeenReduced;
	metaset.insertionReplacesDuplicates = mAnalysisInsertionToggle;
	metaset.isTimeAnalysis = mTimeDimensionToggle;
	metaset.analysisPitch = mAnalysisPitchToggle;
	metaset.analysisLoudness = mAnalysisLoudnessToggle;
	metaset.analysisShape = mAnalysisShapeToggle;
	metaset.analysisMFCC = mAnalysisMFCCToggle;
	metaset.windowFFTSize = mWindowFFTField;
	metaset.hopFraction = mHopFractionField;
	metaset.nBands = mNBandsField;
	metaset.nCoefs = mNCoefsField;
	metaset.minFreq = mMinFreqField;
	metaset.maxFreq = mMaxFreqField;
	metaset.dimensionReductionTarget = mReducedDimensionsField;
	metaset.maxIterations = mMaxIterationsField;

#ifndef META_SET_SIZE_18
#error "incorrect number of metadata entries"
#endif // !META_SET_SIZE_18

	return metaset;
}

void AcorexInterface::ControllerUI::PackDimensionNamesIntoSet ( AcorexCorpus::MetaSetStruct& metaset, bool reducing )
{
	metaset.dimensionNames.clear ( );

	if ( reducing )
	{
		if ( mTimeDimensionToggle )
		{
			metaset.dimensionNames.push_back ( "Samples" );
			metaset.dimensionNames.push_back ( "Time" );
		}

		for ( int i = 0; i < mReducedDimensionsField; i++ )
		{
			metaset.dimensionNames.push_back ( "Dimension " + ofToString ( i ) );
		}
	}

	if ( mTimeDimensionToggle )
	{ 
		metaset.dimensionNames.push_back ( "Samples" );
		metaset.dimensionNames.push_back ( "Time" );

		if ( mAnalysisPitchToggle )
		{
			metaset.dimensionNames.push_back ( "Pitch" );
			metaset.dimensionNames.push_back ( "Pitch Confidence" );
		}

		if ( mAnalysisLoudnessToggle )
		{
			metaset.dimensionNames.push_back ( "Loudness" );
			metaset.dimensionNames.push_back ( "True Peak" );
		}

		if ( mAnalysisShapeToggle )
		{
			metaset.dimensionNames.push_back ( "Spectral Centroid" );
			metaset.dimensionNames.push_back ( "Spectral Spread" );
			metaset.dimensionNames.push_back ( "Spectral Skewness" );
			metaset.dimensionNames.push_back ( "Spectral Kurtosis" );
			metaset.dimensionNames.push_back ( "Spectral Rolloff" );
			metaset.dimensionNames.push_back ( "Spectral Flatness" );
			metaset.dimensionNames.push_back ( "Spectral Crest" );
		}

		if ( mAnalysisMFCCToggle )
		{
			for ( int i = 0; i < mNCoefsField; i++ )
			{
				metaset.dimensionNames.push_back ( "MFCC " + ofToString ( i ) );
			}
		}
		
	}
	else
	{
		if ( mAnalysisPitchToggle )
		{
			Push7Stats ( "Pitch", metaset.dimensionNames );
			Push7Stats ( "Pitch Confidence", metaset.dimensionNames );
		}

		if ( mAnalysisLoudnessToggle )
		{
			Push7Stats ( "Loudness", metaset.dimensionNames );
			Push7Stats ( "True Peak", metaset.dimensionNames );
		}

		if ( mAnalysisShapeToggle )
		{
			Push7Stats ( "Spectral Centroid", metaset.dimensionNames );
			Push7Stats ( "Spectral Spread", metaset.dimensionNames );
			Push7Stats ( "Spectral Skewness", metaset.dimensionNames );
			Push7Stats ( "Spectral Kurtosis", metaset.dimensionNames );
			Push7Stats ( "Spectral Rolloff", metaset.dimensionNames );
			Push7Stats ( "Spectral Flatness", metaset.dimensionNames );
			Push7Stats ( "Spectral Crest", metaset.dimensionNames );
		}

		if ( mAnalysisMFCCToggle )
		{
			for ( int i = 0; i < mNCoefsField; i++ )
			{
				Push7Stats ( "MFCC " + ofToString ( i ), metaset.dimensionNames );
			}
		}
	}
}

void AcorexInterface::ControllerUI::Push7Stats ( std::string masterDimension, std::vector<std::string>& dimensionNames )
{
	dimensionNames.push_back ( masterDimension + " (Mean)" );
	dimensionNames.push_back ( masterDimension + " (Standard Deviation)" );
	dimensionNames.push_back ( masterDimension + " (Skewness)" );
	dimensionNames.push_back ( masterDimension + " (Kurtosis)" );
	dimensionNames.push_back ( masterDimension + " (Low %)" );
	dimensionNames.push_back ( masterDimension + " (Middle %)" );
	dimensionNames.push_back ( masterDimension + " (High %)" );
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
	if ( value ) { mAnalysisInsertionToggle.setName ( "New" ); }
	else { mAnalysisInsertionToggle.setName ( "Existing" ); }
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