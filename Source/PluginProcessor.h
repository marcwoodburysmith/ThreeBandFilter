/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "FilterInfo.h"
#include "FilterParametersBase.h"
#include "FilterParameters.h"
#include "HighCutLowCutParameters.h"
#include "CoefficientMaker.h"



//TODO: Create APVTS object
//TODO: Create parameters in parameterLayout()
//TODO: BOD Struct for parameters (base, parameteric, lowCutHighCut)
//TODO: dsp ProcessorChain for Filter dsp
//TODO: Pointers to parameters
//TODO: GenericAudioProcessorEditor
//TODO: get/set parameters
//TODO: PrepareToPlay
//TODO: processBlock()
//TODO: Add FilterInfo namespace


using Filter = juce::dsp::IIR::Filter<float>;

using MonoChain = juce::dsp::ProcessorChain<Filter>;



//==============================================================================
/**
*/
class ThreeBandFilterAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ThreeBandFilterAudioProcessor();
    ~ThreeBandFilterAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    juce::AudioProcessorValueTreeState apvts { *this, nullptr, "Parameters", createParameterLayout() };
    

private:
    
    MonoChain leftChain;
    MonoChain rightChain;
    
    FilterParameters oldParametricParams;
    HighCutLowCutParameters oldCutParams;
    FilterInfo::FilterType oldFilterType;
    
//    juce::AudioParameterFloat* p_gain{nullptr};
//    juce::AudioParameterFloat* p_freq{nullptr};
//    juce::AudioParameterFloat* p_quality{nullptr};
//    juce::AudioParameterChoice* p_slope {nullptr};
//    juce::AudioParameterBool* p_bypassed {nullptr};
//    juce::AudioParameterChoice* p_filterType {nullptr};
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
   
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThreeBandFilterAudioProcessor)
};
