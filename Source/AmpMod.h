/*
  ==============================================================================

    AmpMod.h
    Created: 31 Jul 2024 11:27:28am
    Author:  Louis Deng

  ==============================================================================
*/

#pragma once

#include <random>


class AmplitudeModulationCurve
{
public:
    AmplitudeModulationCurve()
    {
        // randomize additional phase
        std::random_device rd[2];
        std::mt19937 phigen(rd[0]());
        std::mt19937 vargen(rd[1]());
        std::uniform_real_distribution<float> phirange(0.0f, twoPi);
        std::uniform_real_distribution<float> varrange(0.0f, 0.33f);
        phi_ = phirange(phigen);
        lModSpdVar_ = varrange(vargen);
    }
    ~AmplitudeModulationCurve()
    {
    }
    /// Additional Modulation AMP (Samps) - bound proportionally to LBCF channel length
    float lModAmpAdd_;
    /// Modulation RATE (Hz)
    float lModSpd_;
    
    
    void setSR(float iSR) {
        fs_ = iSR;
    }
    
    float tick() {
        clock_ += lModSpd_/fs_;
        clockbound();
        outsamp_ = (lModAmp_ + lModAmpAdd_) * sin(twoPi * (lModSpd_+lModSpdVar_) * clock_ + phi_);
        return outsamp_;
    }
    
private:
    /// just 2π
    float twoPi = juce::MathConstants<float>::twoPi;
    
    float fs_ = 48000.0f;
    float outsamp_ = 0.0f;
    float clock_ = 0.0f;
    
    /// additional phase
    float phi_;
    /// Modulation Rate Variation
    float lModSpdVar_;
    /// Modulation AMP (Samps)
    //float lModAmp_ = 13.4f;
    float lModAmp_ = 0.0f;
    
    /// limit clock into 2pi boundaries
    void clockbound()
    {
        while(clock_>juce::MathConstants<float>::twoPi)
        {
            clock_-=juce::MathConstants<float>::twoPi;
        }
    }
};
