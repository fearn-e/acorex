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

#include "Utilities/DatasetConversion.h"

using namespace Acorex;

void Utilities::DatasetConversion::CorpusToFluid ( fluid::FluidDataSet<std::string, double, 1>& fluidset, const Utilities::DataSet& dataset, std::vector<int>& filePointLength )
{
    filePointLength.resize ( dataset.trails.raw.size ( ) );
    int fileMarker = 0;

    for ( int file = 0; file < dataset.trails.raw.size ( ); file++ )
    {
        for ( int timepoint = 0; timepoint < dataset.trails.raw[file].size ( ); timepoint++ )
        {
            fluid::RealVector point ( dataset.trails.raw[file][timepoint].size ( ) );

            for ( int dimension = 0; dimension < dataset.trails.raw[file][timepoint].size ( ); dimension++ )
            {
                point[dimension] = dataset.trails.raw[file][timepoint][dimension];
            }

            fluidset.add ( std::to_string ( fileMarker + timepoint ), point );
        }

        filePointLength[file] = dataset.trails.raw[file].size ( );
        fileMarker += dataset.trails.raw[file].size ( );
    }
}

void Utilities::DatasetConversion::FluidToCorpus ( Utilities::DataSet& dataset, const fluid::FluidDataSet<std::string, double, 1>& fluidset, const std::vector<int>& filePointLength, const int reducedDimensionCount )
{
    dataset.trails.raw.clear ( );

    int fileMarker = 0;

    for ( int filePoint = 0; filePoint < filePointLength.size ( ); filePoint++ )
    {
        dataset.trails.raw.push_back ( std::vector<std::vector<double>> ( ) );

        for ( int timePoint = 0; timePoint < filePointLength[filePoint]; timePoint++ )
        {
            dataset.trails.raw[filePoint].push_back ( std::vector<double> ( reducedDimensionCount, 0.0 ) );

            fluid::RealVector pointVals ( reducedDimensionCount );
            fluidset.get ( std::to_string ( fileMarker + timePoint ), pointVals );

            for ( int dimension = 0; dimension < reducedDimensionCount; dimension++ )
            {
                dataset.trails.raw[filePoint][timePoint][dimension] = pointVals[dimension];
            }
        }

        fileMarker += filePointLength[filePoint];
    }
}
