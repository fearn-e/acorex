#pragma once

#include "./Data.h"
#include <vector>

#if __has_include(<omp.h>)
#include <omp.h>
#endif

namespace Acorex {
namespace Utils {

class DimensionBounds {
public:
	DimensionBounds ( ) { }
	~DimensionBounds ( ) { }

	void CalculateBounds ( const DataSet& dataset )
	{
		minBounds.clear ( );
		maxBounds.clear ( );

		minBounds.resize ( dataset.dimensionNames.size ( ) );
		maxBounds.resize ( dataset.dimensionNames.size ( ) );

#pragma omp parallel for
		for ( int dimension = 0; dimension < dataset.dimensionNames.size ( ); dimension++ )
		{
			minBounds[dimension] = std::numeric_limits<double>::max ( );
			maxBounds[dimension] = std::numeric_limits<double>::max ( ) * -1;

			for ( int file = 0; file < dataset.fileList.size ( ); file++ )
			{
				if ( dataset.analysisSettings.bTime )
				{
					for ( int timepoint = 0; timepoint < dataset.time.raw[file].size ( ); timepoint++ )
					{
						if ( dataset.time.raw[file][timepoint][dimension] < minBounds[dimension] ) { minBounds[dimension] = dataset.time.raw[file][timepoint][dimension]; }
						if ( dataset.time.raw[file][timepoint][dimension] > maxBounds[dimension] ) { maxBounds[dimension] = dataset.time.raw[file][timepoint][dimension]; }
					}
				}
				else if ( dataset.analysisSettings.hasBeenReduced )
				{
					if ( dataset.stats.reduced[file][dimension] < minBounds[dimension] ) { minBounds[dimension] = dataset.stats.reduced[file][dimension]; }
					if ( dataset.stats.reduced[file][dimension] > maxBounds[dimension] ) { maxBounds[dimension] = dataset.stats.reduced[file][dimension]; }
				}
				else
				{
					int tempDim = dimension / DATA_NUM_STATS;
					int tempStat = dimension % DATA_NUM_STATS;
					if ( dataset.stats.raw[file][tempDim][tempStat] < minBounds[dimension] ) { minBounds[dimension] = dataset.stats.raw[file][tempDim][tempStat]; }
					if ( dataset.stats.raw[file][tempDim][tempStat] > maxBounds[dimension] ) { maxBounds[dimension] = dataset.stats.raw[file][tempDim][tempStat]; }
				}
			}
		}
	}

	double GetMinBound ( int dimension ) const { return minBounds[dimension]; }
	double GetMaxBound ( int dimension ) const { return maxBounds[dimension]; }

private:
	std::vector<double> minBounds;
	std::vector<double> maxBounds;
};

} // namespace Utils
} // namespace Acorex