/*
  ==============================================================================

    FilterParameters.h
    Created: 11 Dec 2024 1:53:40pm
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "FilterInfo.h"

using namespace FilterInfo;

struct FilterParameters : public FilterParametersBase
{
    FilterType filterType {FilterType::FirstOrderLowPass};
    float gain {0.f};
};

inline bool operator!=(const FilterParametersBase& lhs, const FilterParametersBase& rhs)
{
    return !( lhs == rhs );
}

inline bool operator==(const FilterParameters& lhs, const FilterParameters& rhs)
{
    return (lhs.gain == rhs.gain && lhs.filterType == rhs.filterType &&
            static_cast<const FilterParametersBase&>(lhs) == static_cast<const FilterParametersBase&>(rhs) );
}

inline bool operator!=(const FilterParameters& lhs, const FilterParameters& rhs)
{
    return !( lhs == rhs );
}


