/*
  ==============================================================================

    FilterInfo.h
    Created: 8 Dec 2024 3:10:01pm
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/






#pragma once

namespace FilterInfo
{

enum class FilterType
{
    FirstOrderLowPass,
    FirstOrderHighPass,
    FirstOrderAllPass,
    LowPass,
    HighPass,
    BandPass,
    Notch,
    AllPass,
    LowShelf,
    HighShelf,
    Peak
};

enum class FilterParameterNames
{
    FilterType_Low_Band,
    FilterType_Mid_Band,
    FilterType_High_Band,
    
    Freq_Low_Band,
    Freq_Mid_Band,
    Freq_High_Band,
    
    Quality_Low_Band,
    Quality_Mid_Band,
    Quality_High_Band,
    
    Gain_Low_Band,
    Gain_Mid_Band,
    Gain_High_Band,
    
    Bypassed_Low_Band,
    Bypassed_Mid_Band,
    Bypassed_High_Band,
    
    Slope_Low_Band,
    Slope_Mid_Band,
    Slope_High_Band

};

inline const std::map<FilterInfo::FilterParameterNames, juce::String>& getParameterNames()
{
    static std::map<FilterInfo::FilterParameterNames, juce::String> parameterNames =
    {
        {FilterParameterNames::FilterType_Low_Band, 
            "Filter Type Low Band"},
        {FilterParameterNames::FilterType_Mid_Band,
            "Filter Type Mid Band"},
        {FilterParameterNames::FilterType_High_Band,
            "Filter Type High Band"},
        
        {FilterParameterNames::Freq_Low_Band,
           "Freq Low Band"},
        {FilterParameterNames::Freq_Mid_Band,
           "Freq Mid Band"},
        {FilterParameterNames::Freq_High_Band,
           "Freq High Band"},
        
        {FilterParameterNames::Quality_Low_Band,
            "Quality low band"},
        {FilterParameterNames::Quality_Mid_Band,
            "Quality mid band"},
        {FilterParameterNames::Quality_High_Band,
            "Quality high band"},
        
        {FilterParameterNames::Gain_Low_Band,
            "Gain low band"},
        {FilterParameterNames::Gain_Mid_Band,
            "Gain mid band"},
        {FilterParameterNames::Gain_High_Band,
            "Gain high band"},
        
        {FilterParameterNames::Bypassed_Low_Band,
            "Bypassed low band"},
        {FilterParameterNames:: Bypassed_Mid_Band,
            "Bypassed mid band"},
        {FilterParameterNames:: Bypassed_High_Band,
            "Bypassed high band"},
        
        {FilterParameterNames::Slope_Low_Band,
            "Slope low band"},
        {FilterParameterNames::Slope_Mid_Band,
            "Slope mid band"},
        {FilterParameterNames::Slope_High_Band,
            "Slope high band"}
        
    };
    return parameterNames;
    
}



const std::map<FilterInfo::FilterParameterNames, juce::String> filterToFilter
{
    {FilterParameterNames::FilterType_Low_Band,
        "Filter Type Low Band"},
    {FilterParameterNames::FilterType_Mid_Band,
        "Filter Type Mid Band"},
    {FilterParameterNames::FilterType_High_Band,
        "Filter Type High Band"},
    
    {FilterParameterNames::Freq_Low_Band,
       "Freq Low Band"},
    {FilterParameterNames::Freq_Mid_Band,
       "Freq Mid Band"},
    {FilterParameterNames::Freq_High_Band,
       "Freq High Band"},
    
    {FilterParameterNames::Quality_Low_Band,
        "Quality low band"},
    {FilterParameterNames::Quality_Mid_Band,
        "Quality mid band"},
    {FilterParameterNames::Quality_High_Band,
        "Quality high band"},
    
    {FilterParameterNames::Gain_Low_Band,
        "Gain low band"},
    {FilterParameterNames::Gain_Mid_Band,
        "Gain mid band"},
    {FilterParameterNames::Gain_High_Band,
        "Gain high band"},
    
    {FilterParameterNames::Bypassed_Low_Band,
        "Bypassed low band"},
    {FilterParameterNames:: Bypassed_Mid_Band,
        "Bypassed mid band"},
    {FilterParameterNames:: Bypassed_High_Band,
        "Bypassed high band"},
    
    {FilterParameterNames::Slope_Low_Band,
        "Slope low band"},
    {FilterParameterNames::Slope_Mid_Band,
        "Slope mid band"},
    {FilterParameterNames::Slope_High_Band,
        "Slope high band"}
   
};

inline const std::map<FilterInfo::FilterType, juce::String> getFilterTypes()
{
    static std::map<FilterInfo::FilterType, juce::String> filterType =
    {
        {FilterType::FirstOrderLowPass, "First Order Low Pass"},
        {FilterType::FirstOrderHighPass, "First Order High Pass"},
        {FilterType::FirstOrderAllPass, "First Order All Pass"},
        {FilterType::LowPass, "Low Pass"},
        {FilterType::HighPass, "High Pass"},
        {FilterType::BandPass, "Band Pass"},
        {FilterType::Notch, "Notch"},
        {FilterType::AllPass, "All Pass"},
        {FilterType::LowShelf, "Low Shelf"},
        {FilterType::HighShelf, "High Shelf"},
        {FilterType::Peak, "Peak"}
    };
    
    return filterType;
}

enum class Slope
{
    slope12,
    slope24,
    slope36,
    slope48
};

const std::map<FilterInfo::Slope, juce::String> slopeToString
{
    {Slope::slope12, "12 dB/oct"},
    {Slope::slope24, "24 dB/oct"},
    {Slope::slope36, "36 dB/oct"},
    {Slope::slope48, "48 dB/oct"}
};



}; //end of namespace
