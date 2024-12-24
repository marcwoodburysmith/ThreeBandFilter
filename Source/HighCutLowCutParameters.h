/*
  ==============================================================================

    HighCutLowCutParameters.h
    Created: 11 Dec 2024 1:54:27pm
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


struct HighCutLowCutParameters : public FilterParametersBase
{
    int order {1};
    bool isLowCut{true};
};

