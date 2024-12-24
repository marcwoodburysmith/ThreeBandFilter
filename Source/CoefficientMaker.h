/*
  ==============================================================================

    CoefficientMaker.h
    Created: 11 Dec 2024 1:53:24pm
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


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
        return makeCoefficients(parameters.sampleRate, parameters.freq, parameters.quality, parameters.filterType, parameters.gain);
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
