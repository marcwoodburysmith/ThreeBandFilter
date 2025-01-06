/*
  ==============================================================================

    FilterLink.h
    Created: 5 Jan 2025 6:14:33pm
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "ReleasePool.h"
#include "Fifo.h"



template<typename FilterType, typename FifoDataType, typename ParamType, typename FunctionType>
struct FilterLink
{
    void prepare(const juce::dsp::ProcessSpec&);
    void reset();
    
    template<typename ContextType>
    void process(const ContextType& context);
    
    //stuff for the juce::SmoothedValue instances.
    void updateSmootherTargets();
    void resetSmoothers(float rampTime);
    bool isSmoothing() const;
    void checkIfStillSmoothing();
    void advanceSmoothers(int numSamples);
    
    //stuff for updating the params
    void updateParams(const ParamType& params);
    
    //stuff for updating the coefficients from processBlock, prepareToPlay, or setStateInformation
    void updateCoefficients(const FifoDataType& coefficents);
    void loadCoefficients(bool fromFifo);
    void generateNewCoefficientsIfNeeded();
    
    //stuff for configuring the filter before processing
    void performPreloopUpdate(const ParamType& params);
    void performInnerLoopFilterUpdate(bool onRealTimeThread, int numSamplesToSkip);
    void initialize(const ParamType& params, float rampTime, bool onRealTimeThread, double sr);
private:
    //stuff for setting the coefficients of the FilterType instance.
//    updateFilterState(Ptr& oldState, Ptr newState);
    void configureCutFilterChain(const FifoDataType& coefficients);
    
    /*
     Need instances of:
     
     Fifo
     FilterCoefficientGenerator
     ReleasePool
     
     */
    static const int fifoSize = 100;
    static const int poolSize = 1000;
    static const int cleanupInterval = 2000; // ms
    
//    FilterLink<Filter, FilterCoeffPtr, FilterParameters, CoefficientMaker> paraFilterLink;

    
    using Coefficients = juce::dsp::IIR::Coefficients<float>;
    
    ReleasePool<Coefficients, poolSize> releasePool {poolSize, cleanupInterval};
    
    Fifo <FifoDataType, fifoSize>  coeffFifo;
    
    FilterCoefficientGenerator<FifoDataType, ParamType, CoefficientMaker, fifoSize> coeffGen {coeffFifo};
};
