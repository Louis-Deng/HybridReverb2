/*
  ==============================================================================

    BiquadCombChannel.h
 
    a channel consisting a delay and a biquad filter, when incorporated into a feedback network, this act as a damped comb filter.
 
    Created: 4 Feb 2024 3:11:39pm
    Author:  Louis Deng

  ==============================================================================
*/

#pragma once
#include "AmpMod.h"

/// Parameter struct for LBCF
struct LBCFParam
{
    /// LBCF Gain gc
    float gc_;
    /// CombFilter Delayline Length
    float combL_;
    /// CombFilter Length Modulation Addition
    float combLModADD_;
    /// returns value after length modulation
    float sumCombL() const{
        return combL_+combLModADD_;
    }
};

template <typename SignalType>
class BiquadCombChannel
{
public:
    BiquadCombChannel(float iSamplingRate = 48000.0)
    : fs_(iSamplingRate)
    {
        init();
    }
    ~BiquadCombChannel()
    {
    }
    
    void setSamplingRate(float inputFs) // this MUST be called before setting delay lengths (fs-related calc) etc. make sure sampling rate changes are done before. in plugin processor.
    {
        fs_ = inputFs;
        
        delayDefaultProcess_.sampleRate = fs_;
        delayDefaultProcess_.maximumBlockSize = 2;   //2 samples long buffer due to only using sample-wise processing
        delayDefaultProcess_.numChannels = 1;
        delayComb_->prepare(delayDefaultProcess_);
        delayComb_->setMaximumDelayInSamples( ((int)fs_) << 1 );
        delayComb_->reset();
        
        combLMod_->setSR(fs_);
    }
    
    void injectBiquadFreq(float freq, char filterType)
    {
        switch (filterType)
        {
            case 1: 
                //DBG("attempting to set freq for biquad at " + juce::String(freq) + " LP");
                biquadCoeffs_ = juce::IIRCoefficients::makeLowPass(fs_,freq);
                break;
            case 2: 
                //DBG("attempting to set freq for biquad at " + juce::String(freq) + " HP");
                biquadCoeffs_ = juce::IIRCoefficients::makeHighPass(fs_,freq);
                break;
            default: 
                break;
        }
        biquadFilter_->setCoefficients(biquadCoeffs_);
        //biquadFilter_->reset();
        
    }
    
    /// LBCF inject delay length in samples
    void injectComb(float inputLength, float gc)
    {
        
        if (delayComb_->getDelay() == 0)
        {
            // first time setting delay
            delayComb_->setDelay(inputLength);
            params_->combL_ = inputLength;
            params_->gc_ = gc;
            delayComb_->reset();
        }
        else
        {
            // let popSample() handle delay length changes
            params_->combL_ = inputLength;
            params_->gc_ = gc;
        }
    }
    
    void injectModAmp(float inputModAmp)
    {
        combLMod_->lModAmpAdd_ = inputModAmp;
    }
    
    void injectModSpd(float inputModSpd)
    {
        combLMod_->lModSpd_ = inputModSpd;
    }
    
    /// Sample-wise tick function
    SignalType tick(SignalType iSamp)
    {
        params_->combLModADD_ = combLMod_->tick();  // Delay Modulation
        // push and pop!
        delayComb_->pushSample(0,iSamp);
        oSamp_ = delayComb_->popSample(0,params_->sumCombL()) * params_->gc_;
        oSamp_ = biquadFilter_->processSingleSampleRaw(oSamp_);
        return oSamp_;
    }
    
    /// Returns last output sample
    SignalType lastOut() const
    {
        return oSamp_;
    }
    
    /// clears all internal states
    void clearLBCF()
    {
        delayComb_->reset();
        biquadFilter_->reset();
    }
    
    /// int channel id
    unsigned int channel_id;
    
private:
    /// Sampleate information
    float fs_;
    
    /// AP(?) interpolative DelayLine for Comb in the channel - less LP artifact, for delay modulation
    std::unique_ptr< juce::dsp::DelayLine<SignalType,juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> > delayComb_;
    juce::dsp::ProcessSpec delayDefaultProcess_;
    
    /// Biquad Filter
    std::unique_ptr< juce::IIRFilter > biquadFilter_;
    juce::IIRCoefficients biquadCoeffs_;
    
    /// Internal Param Struct
    std::unique_ptr< LBCFParam > params_;
    std::unique_ptr< AmplitudeModulationCurve> combLMod_;
    
    /// last output sample
    SignalType oSamp_;
    
    /// initialize LBCF once when created
    void init()
    {
        delayComb_.reset( new juce::dsp::DelayLine<SignalType,juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> );
                
        biquadFilter_.reset( new juce::IIRFilter() );
        biquadFilter_->reset();
        
        params_.reset( new LBCFParam );
        
        combLMod_.reset( new AmplitudeModulationCurve );
    }
    
};





