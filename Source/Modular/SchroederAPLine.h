/*
  ==============================================================================

    SchroederAPLine.h
 
    a Schroeder allpass filter line
 
    Created: 14 Mar 2024 5:21:38pm
    Author:  Louis Deng

  ==============================================================================
*/

#pragma once

template <typename SignalType>
class SchroederAPLine
{
public:
    SchroederAPLine(float iSamplingRate = 48000.0)
    : fs_(iSamplingRate)
    {
        init();
    }
    ~SchroederAPLine()
    {
    }
    
    void setSamplingRate(float inputFs) // this MUST be called before setting delay lengths (fs-related calc) etc. make sure sampling rate changes are done before. in plugin processor.
    {
        fs_ = inputFs;
        
        delayDefaultProcess_.sampleRate = fs_;
        delayDefaultProcess_.maximumBlockSize = 2;   //2 samples long buffer due to only using sample-wise processing
        delayDefaultProcess_.numChannels = 1;
        delayAP_->prepare(delayDefaultProcess_);
        delayAP_->setMaximumDelayInSamples( ((int)fs_) << 1 );
        delayAP_->reset();
        apLMod_->setSR(fs_);
    }
    /// AP inject gain ga
    void injectGain(float ga)
    {
        ga_ = ga;
    }
    /// AP inject delay length in samples
    void injectLength(float inputLength)
    {
        
        if (delayAP_->getDelay() == 0)
        {
            // first time setting delay
            delayAP_->setDelay(inputLength);
            apL_ = inputLength;
            delayAP_->reset();
        }
        else
        {
            // let popSample() handle delay length changes
            apL_ = inputLength;
        }
    }
    
    SignalType tick(SignalType input)
    {/**
      < code snippet from Louis's MA thesis project >
      inline StkFloat APF :: tick( StkFloat input )
      {
          //TF: H(z) = [-ga+z^-n] / [1 - ga*z^-n]
          // tmp is v[n] -> the input right before the delay unit
          
          //StkFloat tmp = input + ga_ * apdelayline_.lastOut();
          //lastoutsample_ = apdelayline_.lastOut() - ga_ * tmp;
          
          lastoutsample_ = apdelayline_.lastOut() - ga_*input;
          StkFloat tmp = ga_*lastoutsample_ + input;
          
          apdelayline_.tick(tmp);
          return lastoutsample_;
      }
      */
        oSamp_ = delayAP_->popSample(0,apL_+apLMod_->tick()) - ga_*input;
        vn_ = ga_*oSamp_ + input;
        delayAP_->pushSample(0,vn_);
        
        return oSamp_;
    }
    
    /// Returns last output sample
    SignalType lastOut() const
    {
        return oSamp_;
    }
    
    unsigned int pipe_id;
    
private:
    /// Sampleate information
    float fs_;
    
    /// interpolative DelayLine for All-pass filter in the channel
    std::unique_ptr< juce::dsp::DelayLine<SignalType,juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> > delayAP_;
    juce::dsp::ProcessSpec delayDefaultProcess_;
    
    std::unique_ptr< AmplitudeModulationCurve> apLMod_;
    
    /// internal params
    float ga_;
    float apL_;
    
    /// register
    float vn_;
    
    /// lastout sample
    SignalType oSamp_ = 0.0f;
    
    /// initialize once when created
    void init()
    {
        delayAP_.reset( new juce::dsp::DelayLine<SignalType,juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> );
        
        apLMod_.reset( new AmplitudeModulationCurve );
        apLMod_->lModAmpAdd_ = 3.521f;
        apLMod_->lModSpd_ = 1.337f;
    }
};


