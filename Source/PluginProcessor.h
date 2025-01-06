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
#include "ReleasePool.h"
#include "ParameterHelpers.h"
#include "FilterLink.h"



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
    
    template <const int filterNum>
    void updateParametricFilter(double sampleRate)
    {
        using namespace FilterInfo;
        
    //    auto params = FilterInfo::getParameterNames();
        
        FilterType filterType = static_cast<FilterType>(apvts.getRawParameterValue(createFilterTypeParameters(filterNum))->load() );
        
        float freq = apvts.getRawParameterValue(createFreqParameters(filterNum))->load();
        float quality = apvts.getRawParameterValue(createQualityParameters(filterNum))->load();
        bool bypassed = apvts.getRawParameterValue(createBypassedParameters(filterNum))->load() > 0.5f;
        
        float gainDb = apvts.getRawParameterValue(createGainParameters(filterNum))->load();
        auto gain = juce::Decibels::decibelsToGain(gainDb);

        
    //    float freq = p_freq->get();
    //    float quality = p_quality->get();
    //    bool bypassed = p_bypassed->get();
    //
    //    auto slope = p_slope->getCurrentChoiceName().getFloatValue(); //need to check this
        
        if ( filterType == FilterType::FirstOrderLowPass || filterType == FilterType::FirstOrderHighPass || filterType == FilterType::LowPass || filterType == FilterType::HighPass )
        {
            HighCutLowCutParameters newHighCutLowCut;
            newHighCutLowCut.isLowCut = ( filterType == FilterType::LowPass || filterType == FilterType::FirstOrderLowPass );
            newHighCutLowCut.order = 1;
            newHighCutLowCut.freq = freq;
            newHighCutLowCut.bypassed = bypassed;
            
            if ( filterType == FilterType::LowPass || filterType == FilterType::HighPass )
            {
                newHighCutLowCut.order = 2;
            }
            
            newHighCutLowCut.sampleRate = sampleRate;
            newHighCutLowCut.quality = quality;
            
            if (newHighCutLowCut != oldCutParams || filterType  != oldFilterType )
            {
                //            auto newCoefficients = CoefficientMaker::makeCoefficients(newHighCutLowCut);
                //            decltype(newCoefficients) newCutCoefficients;
                
                cutCoeffGenerator.changeParameters(newHighCutLowCut);
                
                //            cutFifo.push(newCoefficients);
                //            cutFifo.pull(newCutCoefficients);
            }
            CutCoeffArray newChainCoefficients;
            bool newChainAvailable = cutFifo.pull(newChainCoefficients);
            
            if ( newChainAvailable )
            {
                leftChain.setBypassed<0>(bypassed);
                rightChain.setBypassed<0>(bypassed);
                *(leftChain.get<filterNum>().coefficients) = *(newChainCoefficients[0]);
                *(rightChain.get<filterNum>().coefficients) = *(newChainCoefficients[0]);
    //            oldCutParams = newHighCutLowCut;
                parametricCoeffPool.add(newChainCoefficients[0]);
            }
            oldCutParams = newHighCutLowCut;
            
        }
        else
        {
            FilterParameters newFilterParameters;
            
            FilterType filterType = static_cast<FilterType>(apvts.getRawParameterValue(createFilterTypeParameters(filterNum))->load() );
            
            float gain = juce::Decibels::decibelsToGain(apvts.getRawParameterValue(createGainParameters(filterNum))->load() );
            
            newFilterParameters.filterType = filterType;
            newFilterParameters.gain = gain;
            newFilterParameters.freq = freq;
            newFilterParameters.quality = quality;
            newFilterParameters.bypassed = bypassed;
            
            if ( newFilterParameters != oldParametricParams || filterType  != oldFilterType  )
            {
    //            auto newCoefficients = CoefficientMaker::makeCoefficients(newFilterParameters);
    //
    //            paramFifo.push(newCoefficients);
    //            decltype(newCoefficients) newParamCoefficients;
    //            paramFifo.pull(newParamCoefficients);
    //
    //            leftChain.setBypassed<filterNum>(bypassed);
    //            rightChain.setBypassed<filterNum>(bypassed);
    //            *(leftChain.get<filterNum>().coefficients) = *newParamCoefficients;
    //            *(rightChain.get<filterNum>().coefficients) = *newParamCoefficients;
                
                parametricCoeffGenerator.changeParameters(newFilterParameters);
            }
    //        CutCoeffArray newChainCoefficients;
    //        bool newChainAvailable = cutFifo.pull(newChainCoefficients);
            
            ParametricCoeffPtr newChainCoefficients;
            bool newChainAvailable = paramFifo.pull(newChainCoefficients);
            
            if ( newChainAvailable )
            {
                leftChain.setBypassed<filterNum>(bypassed);
                rightChain.setBypassed<filterNum>(bypassed);
                
                *(leftChain.get<filterNum>().coefficients) = *newChainCoefficients;
                *(rightChain.get<filterNum>().coefficients) = *newChainCoefficients;
                
                // prevent in thread deletion
                parametricCoeffPool.add(newChainCoefficients);
            }
            
            
            oldParametricParams = newFilterParameters;
            oldFilterType = filterType;
        }
        
     
    }
    
    template <const int filterNum>
    void updateCutFilter(double sampleRate, HighCutLowCutParameters& oldParams, bool isLowCut)
    {
        using namespace FilterInfo;
        
        float freq = apvts.getRawParameterValue(createFreqParameters(filterNum))->load();
    //    float quality = apvts.getRawParameterValue(createQualityParameters(filterNum))->load();
        bool bypassed = apvts.getRawParameterValue(createBypassedParameters(filterNum))->load() > 0.5f;
        
        Slope slope = static_cast<Slope> (apvts.getRawParameterValue(createSlopeParameters(filterNum))->load());
        
        HighCutLowCutParameters newHighCutLowCut;
        newHighCutLowCut.isLowCut = isLowCut;
        newHighCutLowCut.order = static_cast<int>(slope) + 1;
        newHighCutLowCut.freq = freq;
        newHighCutLowCut.bypassed = bypassed;
        newHighCutLowCut.quality = 1.f;
        
        if (oldParams != newHighCutLowCut)//(newHighCutLowCut != oldParams)
        {
            //        auto cutCoefficients = CoefficientMaker::makeCoefficients(newHighCutLowCut);
            //        decltype(cutCoefficients) newCutCoefficients;
            
            if ( isLowCut )
            {
                lowCutCoeffGenerator.changeParameters(newHighCutLowCut);
            }
            else
            {
                highCutCoeffGenerator.changeParameters(newHighCutLowCut);
            }
        }
            
        CutCoeffArray newChainCoefficients;
        bool newChainAvailable;
            
        if (isLowCut)
        {
    //            lowCutFifo.push(cutCoefficients);
    //            lowCutFifo.pull(newCutCoefficients);
            newChainAvailable = lowCutFifo.pull(newChainCoefficients);
        }
        else
        {
    //            highCutFifo.push(cutCoefficients);
    //            highCutFifo.pull(newCutCoefficients);
            newChainAvailable = highCutFifo.pull(newChainCoefficients);
        }
            
            
        if ( newChainAvailable )
        {
            leftChain.setBypassed<filterNum>(bypassed);
            rightChain.setBypassed<filterNum>(bypassed);
            bypassSubChain<filterNum>();
            if ( !bypassed )
            {
                //
                switch (slope)
                {
                    case Slope::slope48:
                    case Slope::slope42:
                        updateSingleCut<filterNum, 3>(newChainCoefficients, isLowCut);
                    case Slope::slope36:
                    case Slope::slope30:
                        updateSingleCut<filterNum, 2>(newChainCoefficients, isLowCut);
                    case Slope::slope24:
                    case Slope::slope18:
                        updateSingleCut<filterNum, 1>(newChainCoefficients, isLowCut);
                    case Slope::slope12:
                    case Slope::slope6:
                        updateSingleCut<filterNum, 0>(newChainCoefficients, isLowCut);
                }
            }
        }
        oldParams = newHighCutLowCut;
    }
    
    MonoChain leftChain;
    MonoChain rightChain;
    
    FilterParameters oldParametricParams;
    HighCutLowCutParameters oldCutParams;
    FilterInfo::FilterType oldFilterType;
    
    HighCutLowCutParameters oldHighCut;
    HighCutLowCutParameters oldLowCut;
    
    static const int fifoSize = 100;
     // in testing i could fill the pool with enough fiddling with pool size =100, not so with 1000
    static const int poolSize = 1000;
    static const int cleanupInterval = 2000; // ms
    
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
    
    FilterCoefficientGenerator<CutCoeffArray, HighCutLowCutParameters, CoefficientMaker, 100> highCutCoeffGenerator { highCutFifo };
    
    FilterCoefficientGenerator<CutCoeffArray, HighCutLowCutParameters, CoefficientMaker, 100> lowCutCoeffGenerator { lowCutFifo };
    
    FilterCoefficientGenerator<CutCoeffArray, HighCutLowCutParameters, CoefficientMaker, 100> cutCoeffGenerator { cutFifo }; 
    
    // Release pools
    //
    using Coefficients = juce::dsp::IIR::Coefficients<float>;
    ReleasePool<Coefficients, 1000> lowCutCoeffPool {1000, 2000};
    ReleasePool<Coefficients, 100> parametricCoeffPool {100, 2000};
    ReleasePool<Coefficients, 1000> highCutCoeffPool {1000, 2000};
    
    FilterLink<Filter, FilterCoeffPtr, FilterParameters, CoefficientMaker> paraFilterLink;
    

    
    
//    template <const int filterNum>
//    void updateParametricFilter(double sampleRate);
//   
//    template <const int filterNum>
//    void updateCutFilter(double sampleRate, HighCutLowCutParameters& oldParams, bool isLowCut);
    
    template <const int filterNum, const int subFilterNum, typename CoefficientType>
    void updateSingleCut(CoefficientType& chainCoefficients, bool isLowCut)
    {
        auto& leftSubChain = leftChain.template get<filterNum>();
        auto& rightSubChain = rightChain.template get<filterNum>();
        
        *(leftSubChain.template get<subFilterNum>().coefficients) = *(chainCoefficients[subFilterNum]);
        *(rightSubChain.template get<subFilterNum>().coefficients) = *(chainCoefficients[subFilterNum]);
        
        // add to correct release pool
       if(isLowCut)
       {
           lowCutCoeffPool.add(chainCoefficients[subFilterNum]);
       }
       else
       {
           highCutCoeffPool.add(chainCoefficients[subFilterNum]);
       }

       
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
