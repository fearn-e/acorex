#include "./RawView.h"
#include <ofSystemUtils.h>
#include <ofLog.h>

using namespace Acorex;

bool Explorer::RawView::LoadCorpus ( )
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

bool Explorer::RawView::LoadCorpus ( const std::string& path, const std::string& name )
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

	if ( success ) { mCorpusName = name.substr ( 0, name.size ( ) - 5 ); }
	
	return success;
}

bool Explorer::RawView::IsTimeAnalysis ( ) const
{
	return mDataset.analysisSettings.bTime;
}

bool Explorer::RawView::IsReduction ( ) const
{
	return mDataset.analysisSettings.hasBeenReduced;
}

std::vector<std::string> Explorer::RawView::GetDimensions ( ) const
{
	return mDataset.dimensionNames;
}

std::vector<std::string> Explorer::RawView::GetStatistics ( ) const
{
	return mDataset.statisticNames;
}

std::string Explorer::RawView::GetCorpusName ( ) const
{
	return mCorpusName;
}

Utils::TimeData* Explorer::RawView::GetTimeData ( )
{
	return &mDataset.time;
}

Utils::StatsData* Explorer::RawView::GetStatsData ( )
{
	return &mDataset.stats;
}

Utils::DataSet* Explorer::RawView::GetDataset ( )
{
	return &mDataset;
}