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
#include "Fifo.h"
#include "FilterCoefficientGenerator.h"



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

using CutFilter = juce::dsp::ProcessorChain<Filter, Filter, Filter, Filter>;

using MonoChain = juce::dsp::ProcessorChain<CutFilter, Filter, CutFilter>;



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
    
    void updateFilters(double sampleRate); //, bool forceUpdate);
    
    void addFilterParamToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int FilterNum, bool isCut);
    

private:
    
    MonoChain leftChain;
    MonoChain rightChain;
    
    FilterParameters oldParametricParams;
    HighCutLowCutParameters oldCutParams;
    FilterInfo::FilterType oldFilterType;
    
    HighCutLowCutParameters oldHighCut;
    HighCutLowCutParameters oldLowCut;
    
    using ParametricCoeffPtr = decltype(CoefficientMaker::makeCoefficients(oldParametricParams));
    using CutCoeffArray = decltype(CoefficientMaker::makeCoefficients(oldCutParams));
    
    Fifo<ParametricCoeffPtr, 100> paramFifo;
    Fifo<CutCoeffArray, 100> cutFifo;
    Fifo<CutCoeffArray, 100> lowCutFifo;
    Fifo<CutCoeffArray, 100> highCutFifo;
    
    /*
     Instance x4 of FilterCoefficientGenerstor which is initialised with a reference to the above Fifo
     template <typename CoefficientType, typename ParamType, typename MakeFunction, int Size>
     */
    
    FilterCoefficientGenerator <ParametricCoeffPtr, FilterParameters, CoefficientMaker, 100> parametricCoeffGenerator { paramFifo };
    
    
    
    
    template <const int filterNum>
    void updateParametricFilter(double sampleRate);
   
    template <const int filterNum>
    void updateCutFilter(double sampleRate, HighCutLowCutParameters& oldParams, bool isLowCut);
    
    template <const int filterNum, const int subFilterNum, typename CoefficientType>
    void updateSingleCut(CoefficientType& chainCoefficients)
    {
        auto& leftSubChain = leftChain.template get<filterNum>();
        auto& rightSubChain = rightChain.template get<filterNum>();
        
        *(leftSubChain.template get<subFilterNum>().coefficients) = *(chainCoefficients[subFilterNum]);
        *(rightSubChain.template get<subFilterNum>().coefficients) = *(chainCoefficients[subFilterNum]);
       
        leftSubChain.template setBypassed<subFilterNum>(false);
        rightSubChain.template setBypassed<subFilterNum>(false);

        
    }
    
    template <const int filterNum>
    void bypassSubChain()
    {
        auto& leftSubChain = leftChain.template get<filterNum>();
        auto& rightSubChain = rightChain.template get<filterNum>();
        leftSubChain.template setBypassed<0>(true);
        leftSubChain.template setBypassed<1>(true);
        leftSubChain.template setBypassed<2>(true);
        leftSubChain.template setBypassed<3>(true);
        rightSubChain.template setBypassed<0>(true);
        rightSubChain.template setBypassed<1>(true);
        rightSubChain.template setBypassed<2>(true);
        rightSubChain.template setBypassed<3>(true);
    }
//    juce::AudioParameterFloat* p_gain{nullptr};
//    juce::AudioParameterFloat* p_freq{nullptr};
//    juce::AudioParameterFloat* p_quality{nullptr};
//    juce::AudioParameterChoice* p_slope {nullptr};
//    juce::AudioParameterBool* p_bypassed {nullptr};
//    juce::AudioParameterChoice* p_filterType {nullptr};
    
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    
//    void updateFilters(double sampleRate); //, bool forceUpdate);
//    
//    void addFilterParamToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int FilterNum);
    
    
   
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ThreeBandFilterAudioProcessor)
};
