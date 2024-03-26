#pragma once

#include "Interface/ControllerUI.h"

void acorex::interface::ControllerUI::setup ( )
{
	SetupPanels ( );
}

void acorex::interface::ControllerUI::SetupPanels ( )
{
	exit ( );
	mMainPanel.clear ( );
	mAnalysisPanel.clear ( );
	mAnalysisMetadataPanel.clear ( );
	mAnalysisConfirmPanel.clear ( );
	mReductionPanel.clear ( );
	mInsertionDuplicateQuestionPanel.clear ( );

	// Main Panel ---------------------------------
	{

		mMainPanel.setup ( "Menu" );

		mMainPanel.add ( mCreateCorpusButton.setup ( "Analyse Corpus" ) );
		mMainPanel.add ( mReduceCorpusButton.setup ( "Reduce Corpus" ) );

		mMainPanel.setPosition ( 40, 40 );
		mMainPanel.setWidthElements ( 200 );
		mMainPanel.disableHeader ( );
	}

	// Analysis Panel ------------------------------
	{
		mAnalysisPanel.setup ( "Analysis" );

		mAnalysisPanel.add ( mAnalysisPickDirectoryButton.setup ( "Pick Audio Directory" ) );
		mAnalysisPanel.add ( mAnalysisDirectoryLabel.setup ( "", "?" ) );
		mAnalysisPanel.add ( mAnalysisPickOutputFileButton.setup ( "Pick Output File" ) );
		mAnalysisPanel.add ( mAnalysisOutputLabel.setup ( "", "?" ) );

		mAnalysisPanel.setPosition ( -1000, -1000 );
		mAnalysisPanel.setWidthElements ( 315 );
		mAnalysisPanel.disableHeader ( );


		mAnalysisMetadataPanel.setup ( "Analysis Metadata" );

		mAnalysisMetadataPanel.add ( mTimeDimensionToggle.setup ( "Time Dimension", mTimeDimensionToggle ) );

		mAnalysisMetadataPanel.add ( mAnalysisPitchToggle.setup ( "Analyse Pitch", mAnalysisPitchToggle ) );
		mAnalysisMetadataPanel.add ( mAnalysisLoudnessToggle.setup ( "Analyse Loudness", mAnalysisLoudnessToggle ) );
		mAnalysisMetadataPanel.add ( mAnalysisShapeToggle.setup ( "Analyse Shape", mAnalysisShapeToggle ) );
		mAnalysisMetadataPanel.add ( mAnalysisMFCCToggle.setup ( "Analyse MFCC", mAnalysisMFCCToggle ) );

		mAnalysisMetadataPanel.add ( mWindowFFTField.setup ( "Window Size: ", 1024, 512, 8192 ) );
		mAnalysisMetadataPanel.add ( mHopFractionField.setup ( "Hop Size (Fraction of Window):  1 / ", 2, 1, 16 ) );
		mAnalysisMetadataPanel.add ( mNBandsField.setup ( "Bands: ", 40, 1, 100 ) );
		mAnalysisMetadataPanel.add ( mNCoefsField.setup ( "Coefs: ", 13, 1, 20 ) );
		mAnalysisMetadataPanel.add ( mMinFreqField.setup ( "Min Freq: ", 20, 20, 2000 ) );
		mAnalysisMetadataPanel.add ( mMaxFreqField.setup ( "Max Freq: ", 5000, 20, 20000 ) );

		mAnalysisMetadataPanel.setPosition ( -1000, -1000 );
		mAnalysisMetadataPanel.setWidthElements ( 315 );
		mAnalysisMetadataPanel.disableHeader ( );

		
		mAnalysisConfirmPanel.setup ( "Confirm Analysis" );

		mAnalysisConfirmPanel.add ( mConfirmAnalysisButton.setup ( "Confirm" ) );
		mAnalysisConfirmPanel.add ( mCancelAnalysisButton.setup ( "Cancel" ) );

		mAnalysisConfirmPanel.setPosition ( -1000, -1000 );
		mAnalysisConfirmPanel.setWidthElements ( 315 );
		mAnalysisConfirmPanel.disableHeader ( );
	}

	// Reduction Panel -----------------------------
	{
		mReductionPanel.setup ( "Reduction" );

		mReductionPanel.add ( mReductionPickInputFileButton.setup ( "Pick Input File" ) );
		mReductionPanel.add ( mReductionInputLabel.setup ( "", "?" ) );
		mReductionPanel.add ( mReductionPickOutputFileButton.setup ( "Pick Output File" ) );
		mReductionPanel.add ( mReductionOutputLabel.setup ( "", "?" ) );

		mReductionPanel.add ( mReducedDimensionsField.setup ( "Reduced Dimensions", 3, 1, 10 ) );
		mReductionPanel.add ( mMaxIterationsField.setup ( "Max Training Iterations", 200, 1, 1000 ) );

		mReductionPanel.add ( mConfirmReductionButton.setup ( "Confirm" ) );
		mReductionPanel.add ( mCancelReductionButton.setup ( "Cancel" ) );

		mReductionPanel.setPosition ( -1000, -1000 );
		mReductionPanel.setWidthElements ( 300 );
		mReductionPanel.disableHeader ( );
	}

	// Insertion Duplicate Question Panel ----------
	{
		mInsertionDuplicateQuestionPanel.setup ( "Insertion Question" );

		mInsertionDuplicateQuestionPanel.add ( mInsertionDuplicateQuestionLabel.setup ( "For files already existing in the set, which version to use?", "" ) );
		mInsertionDuplicateQuestionPanel.add ( mInsertionDuplicateYesButton.setup ( "New" ) );
		mInsertionDuplicateQuestionPanel.add ( mInsertionDuplicateNoButton.setup ( "Existing" ) );
		
		mInsertionDuplicateQuestionPanel.setPosition ( -1000, -1000 );
		mInsertionDuplicateQuestionPanel.setWidthElements ( 350 );
		mInsertionDuplicateQuestionPanel.disableHeader ( );
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
		mConfirmAnalysisButton.addListener ( this, &ControllerUI::AnalyseInitial );
		mConfirmReductionButton.addListener ( this, &ControllerUI::Reduce );
		mInsertionDuplicateYesButton.addListener ( this, &ControllerUI::AnalyseInsertReplace );
		mInsertionDuplicateNoButton.addListener ( this, &ControllerUI::AnalyseInsertKeep );
	}
}

void acorex::interface::ControllerUI::draw ( )
{
	if ( bDrawMainPanel ) { mMainPanel.draw ( ); }
	if ( bDrawAnalysisPanel ) 
	{	
		mAnalysisMetadataPanel.setPosition ( mAnalysisPanel.getPosition ( ).x, mAnalysisPanel.getPosition ( ).y + mAnalysisPanel.getHeight ( ) + 10 );
		mAnalysisConfirmPanel.setPosition ( mAnalysisMetadataPanel.getPosition ( ).x, mAnalysisMetadataPanel.getPosition ( ).y + mAnalysisMetadataPanel.getHeight ( ) + 10 );
		mAnalysisPanel.draw ( ); 
		mAnalysisMetadataPanel.draw ( );
		mAnalysisConfirmPanel.draw ( );
	}
	if ( bDrawReductionPanel ) { mReductionPanel.draw ( ); }

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

void acorex::interface::ControllerUI::exit ( )
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
	mConfirmAnalysisButton.removeListener ( this, &ControllerUI::AnalyseInitial );
	mConfirmReductionButton.removeListener ( this, &ControllerUI::Reduce );
	mInsertionDuplicateYesButton.removeListener ( this, &ControllerUI::AnalyseInsertReplace );
	mInsertionDuplicateNoButton.removeListener ( this, &ControllerUI::AnalyseInsertKeep );
}


void acorex::interface::ControllerUI::AnalyseInitial ( )
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

	if ( bInsertingIntoCorpus )
	{
		ShowPanel ( mInsertionDuplicateQuestionPanel, bDrawInsertionDuplicateQuestionPanel, true );
		return;
	}

	Analyse ( );
}

void acorex::interface::ControllerUI::AnalyseInsertReplace ( )
{
	insertionReplacesDuplicates = true;
	Analyse ( );
}

void acorex::interface::ControllerUI::AnalyseInsertKeep ( )
{
	insertionReplacesDuplicates = false;
	Analyse ( );
}

void acorex::interface::ControllerUI::Analyse ( )
{
	HideAllPanels ( );

	std::vector<corpus::Metadata> metaset = PackSettingsIntoSet ( );
	bool success = false;
	if ( !bInsertingIntoCorpus )
	{
		success = mController.CreateCorpus ( inputPath, outputPath, metaset );
	}
	else
	{
		success = mController.InsertIntoCorpus ( inputPath, outputPath, metaset );
	}

	if ( !success )
	{
		ShowPanel ( mMainPanel, bDrawMainPanel, true );
		ofLogError ( "ControllerUI" ) << "Failed to create corpus";
		return;
	}
	else
	{
		// TODO - ask if user wants to reduce the data or view it in the corpus viewer
		// make a new panel for this with two choices
	}
}

void acorex::interface::ControllerUI::Reduce ( )
{
	if ( !bReductionInputSelected || !bReductionOutputSelected )
	{
		flashColour = 255;
		bFlashingInvalidFileSelects = true;
		ofLogError ( "ControllerUI" ) << "Reduction input or output file not selected";
		return;
	}

	// TODO - reduce

	// TODO - open in corpus viewer
}


void acorex::interface::ControllerUI::SelectAnalysisDirectory ( )
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
void acorex::interface::ControllerUI::SelectAnalysisOutputFile ( )
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
	}
	else
	{
		bInsertingIntoCorpus = false;
	}

	if ( bInsertingIntoCorpus )
	{
		std::vector<corpus::Metadata> metaset;
		mJSON.ReadMeta ( outputFile.getPath ( ), metaset, false );
		bool success = SetSettingsFromFile ( metaset, true );
		if ( !success )
		{
			ofLogError ( "ControllerUI" ) << "Failed to read metadata";
			return;
		}
	}

	ToggleAnalysisUILockout ( bInsertingIntoCorpus );
	outputPath = outputFile.getPath ( );
	mAnalysisOutputLabel = outputFile.getName ( );
	bAnalysisOutputSelected = true;
}

void acorex::interface::ControllerUI::SelectReductionInputFile ( )
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

	std::vector<corpus::Metadata> metaset;
	mJSON.ReadMeta ( inputFile.getPath ( ), metaset, false );
	bool success = SetSettingsFromFile ( metaset, true );
	if ( !success )
	{
		ofLogError ( "ControllerUI" ) << "Failed to read metadata";
		return;
	}

	inputPath = inputFile.getPath ( );
	mReductionInputLabel = inputFile.getName ( );
	bReductionInputSelected = true;
}

//TODO - fix windows save dialog defaults
	//#ifdef _WIN32
	//auto resultOut = pfd::save_file::save_file("Saving reduced analysis as...", "reduced_corpus.json", { "JSON Files", "*.json" });
	//std::string resultOutPath = resultOut.result();
	//#endif
void acorex::interface::ControllerUI::SelectReductionOutputFile ( )
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

void acorex::interface::ControllerUI::ResetDefaultValues ( )
{
	std::vector<corpus::Metadata> metaset;
	mJSON.ReadMeta ( "", metaset, true );
	SetSettingsFromFile ( metaset, false );
	ToggleAnalysisUILockout ( false );
	
	bInsertingIntoCorpus = false;
	bAnalysisDirectorySelected = false;
	bAnalysisOutputSelected = false;
	bReductionInputSelected = false;
	bReductionOutputSelected = false;

	hasBeenReduced = false;

	inputPath = "";
	outputPath = "";
	mAnalysisDirectoryLabel = "?";
	mAnalysisOutputLabel = "?";
	mReductionInputLabel = "?";
	mReductionOutputLabel = "?";
}

bool acorex::interface::ControllerUI::SetSettingsFromFile ( std::vector<corpus::Metadata>& metaset, bool cancelIfAlreadyReduced )
{
	if ( cancelIfAlreadyReduced )
	{
		for ( auto& meta : metaset )
		{
			if ( meta.key != corpus::META_HAS_BEEN_REDUCED )
			{
				continue;
			}

			if ( meta.boolValue )
			{
				ofLogError ( "ControllerUI" ) << "Analysis has already been reduced";
				return false;
			}
		}
	}

	if ( metaset.size ( ) < 14 )
	{
		ofLogError ( "ControllerUI" ) << "Too few metadata entries";
		return false;
	}

	bool timeDimension = false;
	bool analysisPitch = false; bool analysisLoudness = false; bool analysisShape = false; bool analysisMFCC = false;
	int windowSize = 1024; int hopSizeFraction = 2;
	int nBands = 40; int nCoefs = 13;
	int minFreq = 20; int maxFreq = 5000;
	int reducedDimensions = 3; int maxIterations = 200;

	for ( auto& meta : metaset )
	{
		switch ( meta.key )
		{
			case corpus::META_HAS_BEEN_REDUCED:
				break;
			case corpus::META_TIME_DIMENSION:
				timeDimension = meta.boolValue;
				break;
			case corpus::META_ANALYSIS_PITCH:
				analysisPitch = meta.boolValue;
				break;
			case corpus::META_ANALYSIS_LOUDNESS:
				analysisLoudness = meta.boolValue;
				break;
			case corpus::META_ANALYSIS_SHAPE:
				analysisShape = meta.boolValue;
				break;
			case corpus::META_ANALYSIS_MFCC:
				analysisMFCC = meta.boolValue;
				break;
			case corpus::META_WINDOW_FFT_SIZE:
				windowSize = meta.intValue;
				break;
			case corpus::META_HOP_FRACTION:
				hopSizeFraction = meta.intValue;
				break;
			case corpus::META_N_BANDS:
				nBands = meta.intValue;
				break;
			case corpus::META_N_COEFS:
				nCoefs = meta.intValue;
				break;
			case corpus::META_MIN_FREQ:
				minFreq = meta.intValue;
				break;
			case corpus::META_MAX_FREQ:
				maxFreq = meta.intValue;
				break;
			case corpus::META_REDUCED_DIMENSIONS:
				reducedDimensions = meta.intValue;
				break;
			case corpus::META_MAX_ITERATIONS:
				maxIterations = meta.intValue;
				break;
			case corpus::META_INSERTION_REPLACES_DUPLICATES:
				break;
			default:
				ofLogError ( "ControllerUI" ) << "Invalid metadata key: " << meta.key << " = " << mMetaStrings.getStringFromMeta ( meta.key );
				return false;
		}
	}

	mTimeDimensionToggle = timeDimension;
	mAnalysisPitchToggle = analysisPitch;
	mAnalysisLoudnessToggle = analysisLoudness;
	mAnalysisShapeToggle = analysisShape;
	mAnalysisMFCCToggle = analysisMFCC;
	mWindowFFTField = windowSize;
	mHopFractionField = hopSizeFraction;
	mNBandsField = nBands;
	mNCoefsField = nCoefs;
	mMinFreqField = minFreq;
	mMaxFreqField = maxFreq;
	mReducedDimensionsField = reducedDimensions;
	mMaxIterationsField = maxIterations;

	// TODO - refresh gui?

	return true;
}

std::vector<acorex::corpus::Metadata> acorex::interface::ControllerUI::PackSettingsIntoSet ( )
{
	std::vector<corpus::Metadata> metaset;

	metaset.push_back ( corpus::Metadata ( corpus::META_HAS_BEEN_REDUCED, hasBeenReduced ) );
	metaset.push_back ( corpus::Metadata ( corpus::META_TIME_DIMENSION, mTimeDimensionToggle ) );
	metaset.push_back ( corpus::Metadata ( corpus::META_ANALYSIS_PITCH, mAnalysisPitchToggle ) );
	metaset.push_back ( corpus::Metadata ( corpus::META_ANALYSIS_LOUDNESS, mAnalysisLoudnessToggle ) );
	metaset.push_back ( corpus::Metadata ( corpus::META_ANALYSIS_SHAPE, mAnalysisShapeToggle ) );
	metaset.push_back ( corpus::Metadata ( corpus::META_ANALYSIS_MFCC, mAnalysisMFCCToggle ) );
	metaset.push_back ( corpus::Metadata ( corpus::META_WINDOW_FFT_SIZE, mWindowFFTField ) );
	metaset.push_back ( corpus::Metadata ( corpus::META_HOP_FRACTION, mHopFractionField ) );
	metaset.push_back ( corpus::Metadata ( corpus::META_N_BANDS, mNBandsField ) );
	metaset.push_back ( corpus::Metadata ( corpus::META_N_COEFS, mNCoefsField ) );
	metaset.push_back ( corpus::Metadata ( corpus::META_MIN_FREQ, mMinFreqField ) );
	metaset.push_back ( corpus::Metadata ( corpus::META_MAX_FREQ, mMaxFreqField ) );
	metaset.push_back ( corpus::Metadata ( corpus::META_REDUCED_DIMENSIONS, mReducedDimensionsField ) );
	metaset.push_back ( corpus::Metadata ( corpus::META_MAX_ITERATIONS, mMaxIterationsField ) );
	metaset.push_back ( corpus::Metadata ( corpus::META_INSERTION_REPLACES_DUPLICATES, insertionReplacesDuplicates ) );

	return metaset;
}

void acorex::interface::ControllerUI::QuantiseWindowSize ( int& value )
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
	mAnalysisPanel.setWidthElements ( mAnalysisPanel.getWidth ( ) );
	mAnalysisPanel.setPosition ( mAnalysisPanel.getPosition ( ) );
}

void acorex::interface::ControllerUI::QuantiseHopFraction ( int& value )
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
	mAnalysisPanel.setWidthElements ( mAnalysisPanel.getWidth ( ) );
	mAnalysisPanel.setPosition ( mAnalysisPanel.getPosition ( ) );
}

void acorex::interface::ControllerUI::ToggleAnalysisUILockout ( bool lock )
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

void acorex::interface::ControllerUI::ShowPanel (ofxPanel& visiblePanel, bool& visibleDrawFlag, bool hideOthers )
{
	if ( hideOthers ) { HideAllPanels ( ); }
	visiblePanel.setPosition ( 40, 40 );
	visibleDrawFlag = true;
}

void acorex::interface::ControllerUI::HideAllPanels ( )
{
	mMainPanel.setPosition ( -1000, -1000 ); bDrawMainPanel = false;
	mAnalysisPanel.setPosition ( -1000, -1000 ); bDrawAnalysisPanel = false;
	mReductionPanel.setPosition ( -1000, -1000 ); bDrawReductionPanel = false;
	mInsertionDuplicateQuestionPanel.setPosition ( -1000, -1000 ); bDrawInsertionDuplicateQuestionPanel = false;
}

void acorex::interface::ControllerUI::ShowMainPanel ( )
{
	ShowPanel ( mMainPanel, bDrawMainPanel, true );
	ResetDefaultValues ( );
}

void acorex::interface::ControllerUI::ShowAnalysisPanel ( )
{
	ShowPanel ( mAnalysisPanel, bDrawAnalysisPanel, true );
	ShowPanel ( mAnalysisMetadataPanel, bDrawAnalysisPanel, false );
	ShowPanel ( mAnalysisConfirmPanel, bDrawAnalysisPanel, false );
}

void acorex::interface::ControllerUI::ShowReductionPanel ( )
{
	ShowPanel ( mReductionPanel, bDrawReductionPanel, true );
}