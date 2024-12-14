/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "FilterInfo.h"



/*
Start with 1 band doing stereo processing
The band can be bypassed
The band can have any filter type, chosen with a ComboBox
The band has variable Freq, Q, Gain/Slope
AudioParameters are needed for these things
 
1. APVTS
2. Create parameter layout()
3.
 */

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

using namespace FilterInfo;


struct FilterParametersBase
{
    float freq {440.f};
    bool bypassed {false};
    float quality {0.f};
    double sampleRate{44100.0};
};

struct FilterParameters : public FilterParametersBase
{
    FilterInfo::FilterType filterType {FilterType::FirstOrderLowPass};
    float gainInDecibels {0.f};
};

struct HighCutLowCutParameters : public FilterParametersBase
{
    int order {1};
    bool isLowCut{true};
};



struct CoefficientMaker{
    
    static auto makeCoefficients(float sampleRate, float freq, float qual, FilterInfo::FilterType filterType, float gain)
    {
        switch(filterType)
        {
            case FilterType::FirstOrderLowPass:
                return juce::dsp::IIR::Coefficients<float>::makeFirstOrderLowPass(sampleRate, freq);
            case FilterType::FirstOrderHighPass:
                return juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(sampleRate, freq);
            case FilterType::FirstOrderAllPass:
                return juce::dsp::IIR::Coefficients<float>::makeFirstOrderAllPass(sampleRate, freq);
            case FilterType::LowPass:
                return juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, freq);
            case FilterType::HighPass:
                return juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, freq);
            case FilterType::BandPass:
                return juce::dsp::IIR::Coefficients<float>::makeBandPass(sampleRate, freq);
            case FilterType::Notch:
                return juce::dsp::IIR::Coefficients<float>::makeNotch(sampleRate, freq);
            case FilterType::AllPass:
                return juce::dsp::IIR::Coefficients<float>::makeAllPass(sampleRate, freq);
            case FilterType::LowShelf:
                return juce::dsp::IIR::Coefficients<float>::makeLowShelf(sampleRate, freq, qual, gain);
            case FilterType::HighShelf:
                return juce::dsp::IIR::Coefficients<float>::makeHighShelf(sampleRate, freq, qual, gain);
            case FilterType::Peak:
                return juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, freq, qual, gain);
        }
    }
    
    static auto makeCoefficients(FilterParameters parameters)
    {
        return makeCoefficients(parameters.sampleRate, parameters.freq, parameters.quality, parameters.filterType, parameters.gainInDecibels);
    }
    
    static auto makeCoefficients(HighCutLowCutParameters highcutLowcut)
    {
        if ( highcutLowcut.isLowCut)
        {
            return juce::dsp::FilterDesign<float>::designIIRLowpassHighOrderButterworthMethod(highcutLowcut.freq, highcutLowcut.sampleRate,
                                                                                               highcutLowcut.order);
        }
        return juce::dsp::FilterDesign<float>::designIIRHighpassHighOrderButterworthMethod(highcutLowcut.freq, highcutLowcut.sampleRate,
                                                                                           highcutLowcut.order);
    }
    

    
};





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
    
    FilterParameters oldParams;
    HighCutLowCutParameters oldHighLowParams;
    
    juce::AudioParameterFloat* p_gain{nullptr};
    juce::AudioParameterFloat* p_freq{nullptr};
    juce::AudioParameterFloat* p_quality{nullptr};
    juce::AudioParameterChoice* p_slope {nullptr};
    juce::AudioParameterBool* p_bypassed {nullptr};
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
   
    
    
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThreeBandFilterAudioProcessor)
};
