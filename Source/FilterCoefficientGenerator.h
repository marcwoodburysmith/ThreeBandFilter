/*
  ==============================================================================

    FilterCoefficientGenerator.h
    Created: 3 Jan 2025 10:29:01am
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once


#include <JuceHeader.h>
#include "Fifo.h"
#include "CoefficientMaker.h"



template <typename CoefficientType, typename ParamType, typename MakeFunction, size_t Size>
struct FilterCoefficientGenerator : juce::Thread
{
    FilterCoefficientGenerator( Fifo<CoefficientType, Size>& processFifo ) : Thread {"New Thread"}, coeffFifo {processFifo}
    {
        startThread();
    }
    ~FilterCoefficientGenerator()
    {
        stopThread(50);
    }
    
    //changeParameters will push params into a Fifo<> that the run() function will consume.
    void changeParameters(ParamType params)
    {
        paramFifo.push(params);
        parametersChanged.set(true);
    }
    
    
    void run() override
    {
        while ( !threadShouldExit() )
        {
            wait(10);
            if ( parametersChanged.compareAndSetBool(false, true) )
            {
                //make new coefficients
                ParamType params;
                paramFifo.pull(params);
                auto coeffs = MakeFunction::makeCoefficients(params);
                coeffFifo.push(coeffs);
                // add them to a Fifo<> owned by the audio processor. Give your class a reference member variable to this Fifo<> that your AudioProcessor owns
                //processBlock() will consume these coefficients.
                
            }
            
        }
        
    }
    
private:
    juce::Atomic<bool> parametersChanged {false};
    Fifo<CoefficientType, Size>& coeffFifo;
    Fifo<ParamType, 100> paramFifo;
    
    
    
};
