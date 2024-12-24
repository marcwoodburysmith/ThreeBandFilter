/*
  ==============================================================================

    FilterParametersBase.h
    Created: 11 Dec 2024 1:53:52pm
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct FilterParametersBase
{
    float freq {440.f};
    bool bypassed {false};
    float quality {0.f};
    double sampleRate{44100.0};
};

inline bool operator==(const FilterParametersBase& lhs, const FilterParametersBase& rhs)
{
    return (lhs.freq == rhs.freq && lhs.bypassed == rhs.bypassed && lhs.quality == rhs.quality && lhs.sampleRate == rhs.sampleRate );
}


