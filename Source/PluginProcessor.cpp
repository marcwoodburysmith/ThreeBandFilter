/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

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
    
}

ThreeBandFilterAudioProcessor::~ThreeBandFilterAudioProcessor()
{
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
    
    /*
     Query your APVTS parameters for the filter type, freq, gain, slope, etc..
     package these parameter settings into a FilterParameterBase-derived class, based on the FilterType
     If it's a LowCutHighCut-type of filter, then you'll create a LowCutHighCutParameters object.
     If it's not, then you'll create a FilterParameters object.
     If your parameters changed since the last time processBlock was called, create new coefficients using your CoefficientMaker
     You’ll need a way of determining if your parameters changed.
     The easiest way is to simply store the old parameters as a member variable so you can compare the current parameters with the old parameters to see if they changed.
     You’ll need one old___ for each FilterParameterBase-derived type. i.e. oldCutParams oldParametricParam
     Once you have your coefficients, assign them to your filter, and process your audio accordingly.
     Easy peezy lemon squeezy!!
     */
    
    using namespace FilterInfo;
    
    auto params = FilterInfo::getParameterNames();
    
    FilterInfo::FilterType filterType = static_cast<FilterInfo::FilterType>(apvts.getRawParameterValue(params.at(FilterParameterNames::FilterType_Low_Band))->load() );
    float gain = apvts.getRawParameterValue(params.at(FilterParameterNames::Gain_Low_Band))->load();
    float freq = apvts.getRawParameterValue(params.at(FilterParameterNames::Freq_Low_Band))->load();
    float quality = apvts.getRawParameterValue(params.at(FilterParameterNames::Quality_Low_Band))->load();
    bool bypassed = apvts.getRawParameterValue(params.at(FilterParameterNames::Bypassed_Low_Band))->load() > 0.5f;
    
    
    
//
    
    /*
     If it's a LowCutHighCut-type of filter, then you'll create a LowCutHighCutParameters object.
     If it's not, then you'll create a FilterParameters object.*/
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
        
        //Now need to generate coefficients and build new Filter with these
       
        auto newCoefficients = CoefficientMaker::makeCoefficients(newHighCutLowCut);
        *(leftChain.get<0>().coefficients) = *(newCoefficients[0]);
        *(rightChain.get<0>().coefficients) = *(newCoefficients[1]);
        
    }
    else
    {
        FilterParameters newFilterParameters;
        
        newFilterParameters.gainInDecibels = gain;
        newFilterParameters.freq = freq;
        newFilterParameters.quality = quality;
        newFilterParameters.bypassed = bypassed;
        
        auto newCoefficients = CoefficientMaker::makeCoefficients(newFilterParameters);
        *(leftChain.get<0>().coefficients) = *(newCoefficients);
        *(rightChain.get<0>().coefficients) = *(newCoefficients);
        
    }
    
    
    

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



juce::AudioProcessorValueTreeState::ParameterLayout ThreeBandFilterAudioProcessor::createParameterLayout()
{
    auto params = FilterInfo::getParameterNames();
    
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{params.at(FilterInfo::FilterParameterNames::Bypassed_Low_Band)}, params.at(FilterInfo::FilterParameterNames::Bypassed_Low_Band), false));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{params.at(FilterInfo::FilterParameterNames::Freq_Low_Band), 1},
                                    params.at(FilterInfo::FilterParameterNames::Freq_Low_Band),
                                    juce::NormalisableRange<float>(-12.f, 12.f, 0.1f, 1.f),
                                    0.f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{params.at(FilterInfo::FilterParameterNames::Gain_Low_Band), 1},
                                    params.at(FilterInfo::FilterParameterNames::Gain_Low_Band),
                                    juce::NormalisableRange<float>(-12.f, 12.f, 0.1f, 1.f),
                                    0.f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{params.at(FilterInfo::FilterParameterNames::Quality_Low_Band), 1},
                                    FilterInfo::filterToFilter.at(FilterParameterNames::Quality_Low_Band),
                                    juce::NormalisableRange<float>(-12.f, 12.f, 0.1f, 1.f),
                                    0.f));
    
//    juce::StringArray stringArray;
//        for( int i = 0; i < 4; ++i )
//        {
//            juce::String str;
//            str << (12 + i*12);
//            str << " db/Oct";
//            stringArray.add(str);
//        }
    
    juce::StringArray slopeArray;
    for ( const auto& [id, slope] :  FilterInfo::slopeToString )
    {
        slopeArray.add(slope);
    }
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{params.at(FilterInfo::FilterParameterNames::Slope_Low_Band), 1}, params.at(FilterInfo::FilterParameterNames::Slope_Low_Band), slopeArray, 0));
    
    auto filterTypes = FilterInfo::getFilterTypes();
    
    juce::StringArray filterType;
    for ( const auto& [id, name] :  filterTypes)
    {
        filterType.add(name);
    }
    
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{params.at(FilterInfo::FilterParameterNames::FilterType_Low_Band), 1}, params.at(FilterInfo::FilterParameterNames::FilterType_Low_Band), filterType, 0));
    
    return layout;
}
