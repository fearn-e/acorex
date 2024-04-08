#pragma once

#include "./RawView.h"
#include <ofSystemUtils.h>
#include <ofLog.h>


bool AcorexExplorer::RawView::LoadCorpus ( )
{
	ofFileDialogResult corpusFile = ofSystemLoadDialog ( "Select corpus file" );
	if ( !corpusFile.bSuccess )
	{
		ofLogError ( "RawView" ) << "Invalid load query";
		return false;
	}
	
	bool success = LoadCorpus ( corpusFile.getPath ( ), corpusFile.getName ( ) );
	
	return success;
}

bool AcorexExplorer::RawView::LoadCorpus ( const std::string& path, const std::string& name )
{
	if ( name.find ( ".json" ) == std::string::npos )
	{
		ofLogError ( "RawView" ) << "Invalid file type";
		return false;
	}
	if ( !ofFile::doesFileExist ( path ) )
	{
		ofLogError ( "RawView" ) << "File does not exist";
		return false;
	}

	bool success = mJSON.Read ( path, mDataset );
	
	return success;
}