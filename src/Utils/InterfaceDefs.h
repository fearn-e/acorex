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

#include <ofxGui.h>

namespace Acorex {
namespace Utils {

inline constexpr unsigned int ofxDropdownScrollSpeed = 32;

struct Colors {
    // normal and locked text colour
    const ofColor normalTextColor = 255;
    const ofColor lockedTextColor = 130;

    // interface background colour
    const ofColor interfaceBackgroundColor = { 0, 0, 0, 150 };
    const ofColor transparent = { 0, 0, 0, 0 };
};

struct MenuLayout {
private:
    bool bHiDpi = false;
    const unsigned int hiDpiMultiplier = 2;

    // default values when bHiDpi = false
    unsigned int topBarHeight               = 40;
    unsigned int topBarButtonWidth          = 100;
    unsigned int analyseMainPanelWidth      = 200;
    unsigned int analyseAnalysisPanelWidth  = 315;
    unsigned int analyseReductionPanelWidth = 300;
    unsigned int explorePanelWidth          = 315;
    unsigned int interPanelSpacing          = 5;
    unsigned int panelBackgroundMargin      = 5;

    int modePanelOriginY = topBarHeight + interPanelSpacing;
    const int analysePanelOriginX = 0;

public:

    void toggleHiDpi ( bool hiDpi )
    {
        if ( hiDpi == bHiDpi ) { return; }

        bHiDpi = hiDpi;

        topBarHeight                = (hiDpi ? topBarHeight * hiDpiMultiplier               : topBarHeight / hiDpiMultiplier);
        topBarButtonWidth           = (hiDpi ? topBarButtonWidth * hiDpiMultiplier          : topBarButtonWidth / hiDpiMultiplier);
        analyseMainPanelWidth       = (hiDpi ? analyseMainPanelWidth * hiDpiMultiplier      : analyseMainPanelWidth / hiDpiMultiplier);
        analyseAnalysisPanelWidth   = (hiDpi ? analyseAnalysisPanelWidth * hiDpiMultiplier  : analyseAnalysisPanelWidth / hiDpiMultiplier);
        analyseReductionPanelWidth  = (hiDpi ? analyseReductionPanelWidth * hiDpiMultiplier : analyseReductionPanelWidth / hiDpiMultiplier);
        explorePanelWidth           = (hiDpi ? explorePanelWidth * hiDpiMultiplier          : explorePanelWidth / hiDpiMultiplier);
        interPanelSpacing           = (hiDpi ? interPanelSpacing * hiDpiMultiplier          : interPanelSpacing / hiDpiMultiplier);
        panelBackgroundMargin       = (hiDpi ? panelBackgroundMargin * hiDpiMultiplier      : panelBackgroundMargin / hiDpiMultiplier);

        modePanelOriginY = topBarHeight + interPanelSpacing;
    }
    
    bool isHiDpi ( ) const { return bHiDpi; }

    unsigned int getTopBarHeight ( ) const { return topBarHeight; }
    unsigned int getTopBarButtonWidth ( ) const { return topBarButtonWidth; }
    unsigned int getAnalyseMainPanelWidth ( ) const { return analyseMainPanelWidth; }
    unsigned int getAnalyseAnalysisPanelWidth ( ) const { return analyseAnalysisPanelWidth; }
    unsigned int getAnalyseReductionPanelWidth ( ) const { return analyseReductionPanelWidth; }
    unsigned int getExplorePanelWidth ( ) const { return explorePanelWidth; }
    unsigned int getInterPanelSpacing ( ) const { return interPanelSpacing; }
    unsigned int getPanelBackgroundMargin ( ) const { return panelBackgroundMargin; }
    int getModePanelOriginY ( ) const { return modePanelOriginY; }
    int getAnalysePanelOriginX ( ) const { return analysePanelOriginX; }
};
} // namespace Utils
} // namespace Acorex