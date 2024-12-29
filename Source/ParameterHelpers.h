/*
  ==============================================================================

    ParameterHelpers.h
    Created: 29 Dec 2024 7:35:16am
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>



juce::String createGainParameters(int bandNum);

juce::String createFreqParameters(int bandNum);

juce::String createQualityParameters(int bandNum);

juce::String createSlopeParameters(int bandNum);

juce::String createBypassedParameters(int bandNum);

juce::String createFilterTypeParameters(int bandNum);
