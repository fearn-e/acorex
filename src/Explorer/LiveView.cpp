#include "./LiveView.h"
#include <ofLog.h>
#include "ofGraphics.h"
#include <of3dUtils.h>

using namespace Acorex;

void Explorer::LiveView::SetRawView ( std::shared_ptr<RawView>& rawPointer )
{
	mRawView = rawPointer;
}

void Explorer::LiveView::Initialise ( )
{
	mStatsCorpus.clear ( );
	mTimeCorpus.clear ( );

	mTemporaryCam.setPosition ( 250, 250, 250 );
	mTemporaryCam.lookAt ( { 0, 0, 0 } );
	mTemporaryCam.setNearClip ( 0.1 );
	mTemporaryCam.setFarClip ( 10000 );
}

void Explorer::LiveView::Draw ( )
{
	if ( !bDraw ) { return; }
	
	ofEnableDepthTest ( );
	mTemporaryCam.begin ( );
	ofDrawAxis ( 1000 );

	// Draw points ------------------------------
	if ( mRawView->IsTimeAnalysis ( ) ) // Time
	{
		for ( int file = 0; file < mTimeCorpus.size ( ); file++ )
		{
			mTimeCorpus[file].setMode ( OF_PRIMITIVE_LINE_STRIP );
			mTimeCorpus[file].draw ( );
			mTimeCorpus[file].setMode ( OF_PRIMITIVE_POINTS );
			mTimeCorpus[file].draw ( );
		}
	}
	else // Stats
	{
		mStatsCorpus.setMode ( OF_PRIMITIVE_POINTS );
		mStatsCorpus.draw ( );
	}

	mTemporaryCam.end ( );
	ofDisableDepthTest ( );
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
				mesh.addColor ( { 255, 255, 255 } );
			}
			mTimeCorpus.push_back ( mesh );
		}

		bDraw = true;
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
				mStatsCorpus.addColor ( { 255, 255, 255 } );
			}
		}

		bDraw = true;
		return;
	}
	// ------------------------------

	{
		for ( int file = 0; file < stats.reduced.size ( ); file++ )
		{
			for ( int point = 0; point < stats.reduced[file].size ( ); point++ )
			{
				mStatsCorpus.addVertex ( { 0, 0, 0 } );
				mStatsCorpus.addColor ( { 255, 255, 255 } );
			}
		}

		bDraw = true;
		return;
	}
}

void Explorer::LiveView::FillDimensionTime ( int dimensionIndex, Axis axis )
{
	Utils::TimeData time = mRawView->GetTimeData ( );

	for ( int file = 0; file < time.raw.size ( ); file++ )
	{
		for ( int timepoint = 0; timepoint < time.raw[file].size ( ); timepoint++ )
		{
			double value = 0.0;
			if ( dimensionIndex == -1 ) { value = time.seconds[file][timepoint]; }
			else { value = time.raw[file][timepoint][dimensionIndex]; }

			if ( axis == Axis::COLOR )
			{
				ofColor currentColor = mTimeCorpus[file].getColor ( timepoint );
				currentColor.setHsb ( value, 255, 255 );
				mTimeCorpus[file].setColor ( timepoint, currentColor );
			}
			else
			{
				glm::vec3 currentPoint = mTimeCorpus[file].getVertex ( timepoint );
				currentPoint[axis] = value;
				mTimeCorpus[file].setVertex ( timepoint, currentPoint );
			}
		}
	}
}

void Explorer::LiveView::FillDimensionStats ( int dimensionIndex, Axis axis )
{
	int statisticIndex = dimensionIndex % mRawView->GetStatistics ( ).size ( );
	dimensionIndex /= mRawView->GetStatistics ( ).size ( );

	Utils::StatsData stats = mRawView->GetStatsData ( );

	for ( int file = 0; file < stats.raw.size ( ); file++ )
	{
		if ( axis == Axis::COLOR )
		{
			ofColor currentColor = mStatsCorpus.getColor ( file );
			currentColor.setHsb ( stats.raw[file][dimensionIndex][statisticIndex], 255, 255 );
			mStatsCorpus.setColor ( file, currentColor );
		}
		else
		{
			glm::vec3 currentPoint = mStatsCorpus.getVertex ( file );
			currentPoint[axis] = stats.raw[file][dimensionIndex][statisticIndex];
			mStatsCorpus.setVertex ( file, currentPoint );
		}
	}
}

void Explorer::LiveView::FillDimensionStatsReduced ( int dimensionIndex, Axis axis )
{
	Utils::StatsData stats = mRawView->GetStatsData ( );

	for ( int file = 0; file < stats.reduced.size ( ); file++ )
	{
		if ( axis == Axis::COLOR )
		{
			ofColor currentColor = mStatsCorpus.getColor ( file );
			currentColor.setHsb ( stats.reduced[file][dimensionIndex], 255, 255 );
			mStatsCorpus.setColor ( file, currentColor );
		}
		else
		{
			glm::vec3 currentPoint = mStatsCorpus.getVertex ( file );
			currentPoint[axis] = stats.reduced[file][dimensionIndex];
			mStatsCorpus.setVertex ( file, currentPoint );
		}
	}
}

void Explorer::LiveView::FillDimensionNone ( Axis axis )
{
	if ( mRawView->IsTimeAnalysis ( ) )
	{
		for ( int file = 0; file < mTimeCorpus.size ( ); file++ )
		{
			for ( int timepoint = 0; timepoint < mTimeCorpus[file].getNumVertices ( ); timepoint++ )
			{
				if ( axis == Axis::COLOR )
				{
					ofColor currentColor = mTimeCorpus[file].getColor ( timepoint );
					currentColor.set ( 255, 255, 255 );
					mTimeCorpus[file].setColor ( timepoint, currentColor );
				}
				else
				{
					glm::vec3 currentPoint = mTimeCorpus[file].getVertex ( timepoint );
					currentPoint[axis] = 0;
					mTimeCorpus[file].setVertex ( timepoint, currentPoint );
				}
			}
		}
	}
	else
	{
		for ( int file = 0; file < mStatsCorpus.getNumVertices ( ); file++ )
		{
			if ( axis == Axis::COLOR )
			{
				ofColor currentColor = mStatsCorpus.getColor ( file );
				currentColor.set ( 255, 255, 255 );
				mStatsCorpus.setColor ( file, currentColor );
			}
			else
			{
				glm::vec3 currentPoint = mStatsCorpus.getVertex ( file );
				currentPoint[axis] = 0;
				mStatsCorpus.setVertex ( file, currentPoint );
			}
		}
	}
}