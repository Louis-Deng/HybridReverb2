/*
  ==============================================================================

    DWmixer.h
    Created: 10 Aug 2024 11:40:58am
    Author:  Louis Deng

  ==============================================================================
*/

#pragma once

template <typename SignalType>
class DWmixer
{
public:
    
    DWmixer()
    {
    }
    
    ~DWmixer()
    {
    }
        
    /// process buffered input (R+W Permission for wet, R Permission for dry): apply sample-wise tick calculation, and replace buffer with output.
    void processBuffer(const float *dryBufferRead, float *wetBufferWrite, int numSamps)
    {
        for (int i=0;i<numSamps;i++)
        {
            *(wetBufferWrite+i) = mix(*(dryBufferRead+i),*(wetBufferWrite+i));
        }
    }    
    
    void injectProportion(double input)
    {
        proportion = input;
    }
    
private:
    // basics
    SignalType outSamp_ = 0.0;
    double proportion = 0.0;
    
    SignalType mix(SignalType dryInput, SignalType wetInput)
    {
        outSamp_ = ((double)1.0-proportion)*(double)dryInput+proportion*(double)wetInput;
        return outSamp_;
    }
};
