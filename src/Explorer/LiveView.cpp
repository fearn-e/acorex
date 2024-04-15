#include "./LiveView.h"
#include <ofLog.h>

using namespace Acorex;

void Explorer::LiveView::SetRawView ( std::shared_ptr<RawView>& rawPointer )
{
	mRawView = rawPointer;
}

void Explorer::LiveView::Initialise ( )
{
	mStatsCorpus.clear ( );
	mTimeCorpus.clear ( );
}

void Explorer::LiveView::CreatePoints ( )
{
	if ( mRawView->IsTimeAnalysis ( ) )
	{
		Utils::TimeData time = mRawView->GetTimeData ( );

		for ( int file = 0; file < time.raw.size ( ); file++ )
		{
			ofMesh mesh;
			for ( int timepoint = 0; timepoint < time.raw[file].size ( ); timepoint++ )
			{
				mesh.addVertex ( { 0, 0, 0 } );
			}
			mTimeCorpus.push_back ( mesh );
		}

		return;
	}
	// ------------------------------

	Utils::StatsData stats = mRawView->GetStatsData ( );

	if ( !mRawView->IsReduction ( ) )
	{
		for ( int file = 0; file < stats.raw.size ( ); file++ )
		{
			for ( int point = 0; point < stats.raw[file].size ( ); point++ )
			{
				mStatsCorpus.addVertex ( { 0, 0, 0 } );
			}
		}

		return;
	}
	// ------------------------------

	for ( int file = 0; file < stats.reduced.size ( ); file++ )
	{
		for ( int point = 0; point < stats.reduced[file].size ( ); point++ )
		{
			mStatsCorpus.addVertex ( { 0, 0, 0 } );
		}
	}
}

void Explorer::LiveView::FillDimension ( std::string& dimension, Axis axis )
{
	int dimensionIndex = GetDimensionIndex ( dimension );

	if ( mRawView->IsTimeAnalysis ( ) )
	{
		Utils::TimeData time = mRawView->GetTimeData ( );

		for ( int file = 0; file < time.raw.size ( ); file++ )
		{
			for ( int timepoint = 0; timepoint < time.raw[file].size ( ); timepoint++ )
			{
				glm::vec3 currentPoint = mTimeCorpus[file].getVertex ( timepoint );
				currentPoint[axis] = time.raw[file][timepoint][dimensionIndex];
				mTimeCorpus[file].setVertex ( timepoint, currentPoint );
			}
		}
	}
	else
	{
		Utils::StatsData stats = mRawView->GetStatsData ( );

		if ( !mRawView->IsReduction ( ) )
		{
			// TODO - needs extra GUI to select statistic
		}
		else
		{
			for ( int file = 0; file < stats.reduced.size ( ); file++ )
			{
				glm::vec3 currentPoint = mStatsCorpus.getVertex ( file );
				currentPoint[axis] = stats.reduced[file][dimensionIndex];
				mStatsCorpus.setVertex ( file, currentPoint );
			}
		}
	}
}

int Explorer::LiveView::GetDimensionIndex ( std::string& dimension )
{
	for ( int i = 0; i < mRawView->GetDimensions ( ).size ( ); i++ )
	{
		if ( mRawView->GetDimensions ( )[i] == dimension )
		{
			return i;
		}
	}
	ofLogWarning ( "LiveView" ) << "Dimension " << dimension << " not found";
}