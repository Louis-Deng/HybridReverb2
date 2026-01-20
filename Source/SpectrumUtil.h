/*
  ==============================================================================

    SpectrumUtil.h
    Created: 26 Sep 2024 12:35:54pm
    Author:  Louis Deng

  ==============================================================================
*/

#pragma once
namespace SpectrumUtil
{
    inline float amp2db(float amp)
    {
        float dbNegative;
        dbNegative = 20.0f*log10(amp);
        
        return dbNegative;
    }
    
    /// fft bin to frequency conversion - probably won't use this one in spectrometer
    inline float bin2freq(float sr, uint32_t maxBin, uint32_t bin)
    {
        float freqInterval = sr/(maxBin-1);
        float frequency;
        frequency = freqInterval*bin;
        return frequency;
    }
}
