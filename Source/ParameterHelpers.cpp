/*
  ==============================================================================

    ParameterHelpers.cpp
    Created: 29 Dec 2024 7:35:16am
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#include "ParameterHelpers.h"

 
juce::String createParameters(juce::String label, int bandNum)
{
    return "Filter_" + juce::String(bandNum) + "_" + label;
}
 
juce::String createGainParameters(int bandNum)
{
    return createParameters("Gain", bandNum);
}

juce::String createFreqParameters(int bandNum)
{
   return createParameters("Freq", bandNum);
}

juce::String createQualityParameters(int bandNum)
{
   return createParameters("Qual", bandNum);
}

juce::String createSlopeParameters(int bandNum)
{
   return createParameters("Slope", bandNum);
}

juce::String createBypassedParameters(int bandNum)
{
   return createParameters("Bypassed", bandNum);
}
 
juce::String createFilterTypeParameters(int bandNum)
{
   return createParameters("FilterType", bandNum);
}
