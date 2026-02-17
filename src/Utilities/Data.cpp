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

#include <Utilities/Data.h>

using namespace Acorex;

// -------------------------------------------------------------------------
// -------------------------- VisualPlayhead -------------------------------
// -------------------------------------------------------------------------

void Utilities::VisualPlayhead::ResizeBox ( size_t playheadIndexUI, size_t topBarHeight, size_t windowHeight, size_t windowWidth )
{
    //TODO - put these in InterfaceDefs.h and have them affected by HiDpi
    int rectHeight = windowHeight / 16;
    int rectWidth = rectHeight * 5;

    int rectSpacing = windowWidth / 64;

    float x = 0;
    float y = topBarHeight + rectSpacing + (playheadIndexUI * (rectHeight + rectSpacing));

    panelRect = ofRectangle ( x, y, rectWidth, rectHeight );
    playheadColorRect = ofRectangle ( x, y, rectWidth / 3, rectHeight );
    killButtonRect = ofRectangle ( x + rectWidth - rectHeight, y, rectHeight, rectHeight );
}

// -------------------------------------------------------------------------
// -------------------------- VisualPlayheadTrail --------------------------
// -------------------------------------------------------------------------

Utilities::VisualPlayheadTrail::VisualPlayheadTrail ( size_t ID, ofColor playheadColor, size_t maxTrailLength, int fadeUpdateIntervalMillis )
    : playheadID ( ID ), playheadColor ( playheadColor ), maxTrailSize ( maxTrailLength ), dying ( false ),
    currentFadeStep ( 0 ), lastFadeUpdateTime ( 0 ), fadeUpdateInterval ( fadeUpdateIntervalMillis )
{ }

void Utilities::VisualPlayheadTrail::UpdateTrail ( )
{
    for ( int i = 0; i < color.size ( ); i++ )
    {
        // trail points fade out - and the colour is initally 100% playhead colour, but then slowly shifts to the actual colour of the point as it fades out
        float fadeFactor = ofMap ( i, 1, maxTrailSize, 1.0f, 0.0f, true );
        ofColor blendedColor = playheadColor.getLerped ( color[i], 1.0f - fadeFactor );
        displayedColor[i] = ofColor ( blendedColor.r, blendedColor.g, blendedColor.b, 255 * fadeFactor );
    }
}

void Utilities::VisualPlayheadTrail::Kill ( )
{
    dying = true;
    currentFadeStep = 0;
    lastFadeUpdateTime = ofGetElapsedTimeMillis ( );

    ofColor inactiveEffect = playheadColor.getLerped ( ofColor ( 255, 255, 255, 255 ), 0.5f );
    playheadColor = ofColor ( inactiveEffect.r, inactiveEffect.g, inactiveEffect.b, playheadColor.a );

    for ( int i = 0; i < color.size ( ); i++ )
    {
        // gray out all points slightly to signify that the trail is dying, and then they will fade out from there
        ofColor immediateGreyEffect = color[i].getLerped ( ofColor ( 255, 255, 255, 255 ), 0.5f );
        color[i] = ofColor ( immediateGreyEffect.r, immediateGreyEffect.g, immediateGreyEffect.b, color[i].a );
    }
}

void Utilities::VisualPlayheadTrail::Draw ( ) const
{
    //ofEnableDepthTest ( );
    //ofEnableAlphaBlending ( );
    for ( int i = 0; i < position.size ( ); i++ )
    {
        ofSetColor ( displayedColor[i] );
        ofDrawSphere ( position[i], 20 );
        if ( i < position.size ( ) - 1 )
        {
            // if i+1 to i is a jump, draw a dashed line
            if ( fileIndex[i] != fileIndex[i + 1] || timePointIndex[i] != timePointIndex[i + 1] + 1 )
            {
                ofSetColor ( displayedColor[i].r, displayedColor[i].g, displayedColor[i].b, displayedColor[i].a / 2 );
                for ( float t = 0; t < 1; t += 0.1 )
                {
                    glm::vec3 pointOnLine = position[i] + t * ( position[i + 1] - position[i] );
                    ofDrawSphere ( pointOnLine, 10 );
                }
            }
            else
            {
                ofSetColor ( displayedColor[i].r, displayedColor[i].g, displayedColor[i].b, displayedColor[i].a / 4 );
                ofDrawLine ( position[i], position[i + 1] );
            }
        }
    }
}

bool Utilities::VisualPlayheadTrail::Update ( int currentTime )
{
    if ( !dying ) { return false; }

    if ( currentTime - lastFadeUpdateTime < fadeUpdateInterval ) { return false; }

    currentFadeStep++;

    for ( int i = 0; i < color.size ( ); i++ )
    {
        float fadeFactor = ofMap ( i, 1 - currentFadeStep, maxTrailSize - currentFadeStep, 1.0f, 0.0f, true );
        ofColor blendedColor = playheadColor.getLerped ( color[i], 1.0f - fadeFactor );
        displayedColor[i] = ofColor ( blendedColor.r, blendedColor.g, blendedColor.b, 255 * fadeFactor );
    }

    lastFadeUpdateTime = currentTime;

    return currentFadeStep >= maxTrailSize;
}

void Utilities::VisualPlayheadTrail::AddTrailPoint ( size_t file, size_t timePoint, const glm::vec3& pos, const ofColor& col ) {
    if ( dying ) { return; }

    if ( fileIndex.size ( ) > 0 && file == fileIndex.front ( ) && timePoint == timePointIndex.front ( ) ) { return; }
        
    fileIndex.push_front ( file );
    timePointIndex.push_front ( timePoint );
    position.push_front ( pos );
    color.push_front ( col );
    displayedColor.push_front ( playheadColor );

    if ( position.size ( ) > maxTrailSize ) {
        fileIndex.pop_back ( );
        timePointIndex.pop_back ( );
        position.pop_back ( );
        color.pop_back ( );
        displayedColor.pop_back ( );
    }

    UpdateTrail ( );
}