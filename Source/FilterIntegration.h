/*
  ==============================================================================

    FilterIntegration.h
 
    It's a sample-wise biquad filter function in Juce
    
    manages mono sample wise processing
 
    Created: 26 Jun 2024 11:31:07am
    Author:  Louis Deng

  ==============================================================================
*/

#pragma once

template <typename SignalType>
/// Custom class of Filter Integration, holds processBuffer(const float ptr) function for processing a buffer of input
class FilterIntegration
{
public:
    FilterIntegration()
    {
        init();
    }
    
    ~FilterIntegration()
    {
        
    }
    
    /// process buffered input (R+W Permission): apply sample-wise tick calculation, and replace buffer with output.
    void processBuffer(float *buffer, int numSamps)
    {
        for (int i=0;i<numSamps;i++)
        {
            *(buffer+i) = tick(*(buffer+i));
        }
    }
        
    void changeCoeffs(bool switchInput)
    {
        if (switchInput == false){
            biquad->setCoefficients(lpCoeffs);
            //biquad->reset();
        }
        else if (switchInput == true){
            biquad->setCoefficients(hpCoeffs);
            //biquad->reset();
        }
        
    }
    
    
private:
    
    /// internal params test
    juce::IIRCoefficients lpCoeffs;
    juce::IIRCoefficients hpCoeffs;
    
    /// Biquad filter
    std::unique_ptr< juce::IIRFilter > biquad;
    
    /// FI Internal sample-wise value
    SignalType outsamp;
    
    ///FI initialization function
    void init()
    {
        lpCoeffs = juce::IIRCoefficients::makeLowPass(48000.0f, 4000.0f, 0.5f);
        hpCoeffs = juce::IIRCoefficients::makeHighPass(48000.0f, 4000.0f, 0.5f);
        
        biquad.reset( new juce::IIRFilter() );
        biquad->reset();
    }
    
    ///FI sample-wise calculation function
    SignalType tick(SignalType input)
    {
        outsamp = biquad->processSingleSampleRaw(input);
        return outsamp;
    }
    
    
    
};
