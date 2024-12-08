/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class ThreeBandFilterAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    ThreeBandFilterAudioProcessorEditor (ThreeBandFilterAudioProcessor&);
    ~ThreeBandFilterAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    ThreeBandFilterAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThreeBandFilterAudioProcessorEditor)
};
