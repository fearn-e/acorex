/*
The MIT License (MIT)

Copyright (c) 2024-2026 Elowyn Fearne

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "Utils/Data.h"

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
        Clear ( );

        bounds.min.resize ( dataset.dimensionNames.size ( ) );
        bounds.max.resize ( dataset.dimensionNames.size ( ) );

#pragma omp parallel for
        for ( int dimension = 0; dimension < dataset.dimensionNames.size ( ); dimension++ )
        {
            bounds.min[dimension] = std::numeric_limits<double>::max ( );
            bounds.max[dimension] = std::numeric_limits<double>::max ( ) * -1;

            for ( int file = 0; file < dataset.fileList.size ( ); file++ )
            {
                for ( int timepoint = 0; timepoint < dataset.trails.raw[file].size ( ); timepoint++ )
                {
                    if ( dataset.trails.raw[file][timepoint][dimension] < bounds.min[dimension] ) { bounds.min[dimension] = dataset.trails.raw[file][timepoint][dimension]; }
                    if ( dataset.trails.raw[file][timepoint][dimension] > bounds.max[dimension] ) { bounds.max[dimension] = dataset.trails.raw[file][timepoint][dimension]; }
                }
            }
        }
    }

    void Clear ( )
    {
        bounds.min.clear ( );
        bounds.max.clear ( );
    }

    double GetMinBound ( int dimension ) const { return bounds.min[dimension]; }
    double GetMaxBound ( int dimension ) const { return bounds.max[dimension]; }
    Utils::DimensionBoundsData GetBoundsData ( ) const { return bounds; }

private:
    Utils::DimensionBoundsData bounds;
};

} // namespace Utils
} // namespace Acorex