/*
  ==============================================================================

    Fifo.h
    Created: 22 Dec 2024 1:21:28pm
    Author:  Marc Woodbury-Smith

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>


template<typename T, size_t Size>
struct Fifo
{
    size_t getSize() const noexcept
    {
        return Size;
    }
    
    
    //used when T is AudioBuffer<float>
    void prepare(int numSamples, int numChannels)
    {
        
        static_assert (std::is_same<juce::AudioBuffer<float>, T>::value,
                              "Fifo::prepare(2 params) requires T to be AudioBuffer<float>!");
        for (auto& audioBuffer : buffer)
        {
            // don't bother clearing extra space, we are going to clear right after this call.
            audioBuffer.setSize (numChannels, numSamples, false, false, true);
            audioBuffer.clear();
        }
    }
    
    
    //used when T is std::vector<float>
    void prepare(size_t numElements)
    {
        static_assert (std::is_same<std::vector<float>, T>::value,
                              "Fifo::prepare(1 param) requires T to be vector<float>!");

        for ( auto& buf : buffer )
        {
//            static_assert(std::is_same<std::vector<float>, T>::value );
            buf.clear();
            buf.resize(numElements);
        }
    }
    
    
    bool push(const T& t)
    {
        auto writeHandle = fifo.write(1);
        
        /*
         if the blockSize1 of the ScopedWrite object is > 0,
         then you can write t into buffer[write.startIndex1] and return true,
         meaning that you pushed a value into the FIFO.
         if its not, then just return false*/
        
        if (writeHandle.blockSize1 < 1)
                return false;
        
        
        if constexpr ( IsReferenceCountedObjectPtr<T>::value )
        {
            auto tempT {buffer[writeHandle.startIndex1]};
            buffer[writeHandle.startIndex1] = t;
            
            // verify we are not about to delete the object that was at this index, if any!
            if(tempT)
            {
                jassert (tempT.get()->getReferenceCount() > 1);
            }
        }
        else
        {
            buffer[writeHandle.startIndex1] = t;
        }
        return true;

    }
          
    
    bool pull(T& t)
    {
        auto readHandle = fifo.read(1);
        
        if ( readHandle.blockSize1 > 0 )
        {
            t = buffer[readHandle.startIndex1];
            return true;
        }
        return false;
    }
    
    int getNumAvailableForReading() const
    {
        return fifo.getNumReady();
    }
    int getAvailableSpace() const
    {
        return fifo.getFreeSpace();
    }
private:
    juce::AbstractFifo fifo { Size };
    std::array<T, Size> buffer;
    
    template<typename U>
    struct IsReferenceCountedObjectPtr : std::false_type { };

    template<typename W>
    struct IsReferenceCountedObjectPtr<juce::ReferenceCountedObjectPtr<W>> : std::true_type { };
};



