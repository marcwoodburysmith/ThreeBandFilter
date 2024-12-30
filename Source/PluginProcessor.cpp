/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "ParameterHelpers.h"



//==============================================================================
ThreeBandFilterAudioProcessor::ThreeBandFilterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    
//    auto params = FilterInfo::getParameterNames();

//    p_gain = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(FilterInfo::FilterParameterNames::Gain_Low_Band)));
//    p_freq = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(FilterInfo::FilterParameterNames::Freq_Low_Band)));
//    p_quality = dynamic_cast<juce::AudioParameterFloat*>(apvts.getParameter(params.at(FilterInfo::FilterParameterNames::Quality_Low_Band)));
//    p_slope = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(params.at(FilterInfo::FilterParameterNames::Slope_Low_Band)));
//    p_bypassed = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(params.at(FilterInfo::FilterParameterNames::Bypassed_Low_Band)));
//    p_filterType = dynamic_cast<juce::AudioParameterChoice*>(apvts.getParameter(params.at(FilterInfo::FilterParameterNames::FilterType_Low_Band)));
    
}

ThreeBandFilterAudioProcessor::~ThreeBandFilterAudioProcessor()
{
}

//==============================================================================

juce::AudioProcessorValueTreeState::ParameterLayout ThreeBandFilterAudioProcessor::createParameterLayout()
{
//    auto params = FilterInfo::getParameterNames();
    
//    using ParamLayout = juce::AudioProcessorValueTreeState::ParameterLayout;
    
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    addFilterParamToLayout(layout, 0, true);
    addFilterParamToLayout(layout, 1, false);
    addFilterParamToLayout(layout, 2, true);
    
    return layout;
}

void ThreeBandFilterAudioProcessor::addFilterParamToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int filterNum, bool isCut)
{
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{createBypassedParameters(filterNum), 1}, 
                                                          createBypassedParameters(filterNum),
                                                          false));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{createFreqParameters(filterNum), 1},
                                                           createFreqParameters(filterNum),
                                                           juce::NormalisableRange<float>(40.f, 22000.f, 1.f, .6f),
                                                           400.f));
    
   
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{createQualityParameters(filterNum), 1},
                                                           createQualityParameters(filterNum),
                                                           juce::NormalisableRange<float>(1.f, 10.f, 0.5f, 1.f),
                                                           1.f));
    if (!isCut)
    {
        layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{createGainParameters(filterNum), 1},
                                                               createGainParameters(filterNum),
                                                               juce::NormalisableRange<float>(-24.f, 24.f, 0.5f, 1.f),
                                                               0.f));
        
        auto filterTypes = FilterInfo::getFilterTypes();
        
        juce::StringArray filterType;
        for ( const auto& [id, name] :  filterTypes)
        {
            filterType.add(name);
        }
        
        layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{createFilterTypeParameters(filterNum), 1}, createFilterTypeParameters(filterNum), filterType, 0));
        
    }
    else
    {
        juce::StringArray slopeArray;
        for ( const auto& [id, slope] :  FilterInfo::slopeToString )
        {
            slopeArray.add(slope);
        }
        
        layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{createSlopeParameters(filterNum), 1}, createSlopeParameters(filterNum), slopeArray, 0));
        
    }
    
    
    //    juce::StringArray stringArray;
    //        for( int i = 0; i < 4; ++i )
    //        {
    //            juce::String str;
    //            str << (12 + i*12);
    //            str << " db/Oct";
    //            stringArray.add(str);
    //        }
    
   
    
   
}

//==============================================================================
const juce::String ThreeBandFilterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ThreeBandFilterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ThreeBandFilterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool ThreeBandFilterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ThreeBandFilterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ThreeBandFilterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ThreeBandFilterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ThreeBandFilterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ThreeBandFilterAudioProcessor::getProgramName (int index)
{
    return {};
}

void ThreeBandFilterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ThreeBandFilterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::dsp::ProcessSpec spec;
    spec.maximumBlockSize = samplesPerBlock;
    spec.sampleRate = sampleRate;
    spec.numChannels = getTotalNumOutputChannels();
    
    leftChain.prepare(spec);
    rightChain.prepare(spec);
}

void ThreeBandFilterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ThreeBandFilterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ThreeBandFilterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());


    updateFilters(getSampleRate() );

    juce::dsp::AudioBlock<float> block(buffer);
    auto leftChannel = block.getSingleChannelBlock(0);
    auto rightChannel = block.getSingleChannelBlock(1);
    
    juce::dsp::ProcessContextReplacing<float> leftContext(leftChannel);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightChannel);
    
    leftChain.process(leftContext);
    rightChain.process(rightContext);
    
}

//==============================================================================
bool ThreeBandFilterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ThreeBandFilterAudioProcessor::createEditor()
{
//    return new ThreeBandFilterAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void ThreeBandFilterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ThreeBandFilterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ThreeBandFilterAudioProcessor();
}

//==============================================================================

template <const int filterNum>
void ThreeBandFilterAudioProcessor::updateCutFilter(double sampleRate, bool isLowCut)
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
    
    if (true)//(newHighCutLowCut != oldParams)
    {
        auto cutCoefficients = CoefficientMaker::makeCoefficients(newHighCutLowCut);
        
        //FIFO HERE
        
        leftChain.setBypassed<filterNum>(bypassed);
        rightChain.setBypassed<filterNum>(bypassed);
//        bypassSubChain<filterNum>();
        if ( !bypassed )
        {
//
            switch (slope)
            {
                case Slope::slope48:
                case Slope::slope42:
                    updateSingleCut<filterNum, 3>(cutCoefficients);
                case Slope::slope36:
                case Slope::slope30:
                    updateSingleCut<filterNum, 2>(cutCoefficients);
                case Slope::slope24:
                case Slope::slope18:
                    updateSingleCut<filterNum, 1>(cutCoefficients);
                case Slope::slope12:
                case Slope::slope6:
                    updateSingleCut<filterNum, 0>(cutCoefficients);                
            }
        }
    }
//    oldParams = newHighCutLowCut;
}

template <const int filterNum>
void ThreeBandFilterAudioProcessor::updateParametricFilter(double sampleRate)
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
            auto newCoefficients = CoefficientMaker::makeCoefficients(newHighCutLowCut);
            
            //FIFO HERE
            
            leftChain.setBypassed<0>(bypassed);
            rightChain.setBypassed<0>(bypassed);
            *(leftChain.get<filterNum>().coefficients) = *(newCoefficients[0]);
            *(rightChain.get<filterNum>().coefficients) = *(newCoefficients[0]);
//            oldCutParams = newHighCutLowCut;
        
            
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
            auto newCoefficients = CoefficientMaker::makeCoefficients(newFilterParameters);
            
            //FIFO HERE
            
            leftChain.setBypassed<filterNum>(bypassed);
            rightChain.setBypassed<filterNum>(bypassed);
            *(leftChain.get<filterNum>().coefficients) = *newCoefficients;
            *(rightChain.get<filterNum>().coefficients) = *newCoefficients;
        }
        oldParametricParams = newFilterParameters;
    }
    
 
}

void ThreeBandFilterAudioProcessor::updateFilters(double sampleRate)
{
//    updateCutFilter<0>(sampleRate, oldCutParams, true);
    updateCutFilter<0>(sampleRate, true);
    updateParametricFilter<1>(sampleRate);
    updateCutFilter<2>(sampleRate, false);
    
}
