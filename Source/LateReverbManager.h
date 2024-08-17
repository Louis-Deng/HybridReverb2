/*
  ==============================================================================

    LateReverbManager.h
    Created: 4 Feb 2024 3:44:02pm
    Author:  Louis Deng

  ==============================================================================
*/

#pragma once

#include "BiquadCombChannel.h"
#include "SchroederAPLine.h"
#include "PrimeUtil.h"
#include "MatrixOps.h"

#define MATDIM 16
#define APCASC 4

template <typename SignalType>
class LateReverbManager
{
public:
    /// LateReverbManager constructor
    LateReverbManager()
    //more init params comma separated...
    {
        init();
    }
    
    /// LateReverbManager deconstructor
    ~LateReverbManager()
    {
    }
        
    void setSamplingRate(float samplingRate)    // this MUST be called before setting delay lengths (fs-related calc) etc. make sure sampling rate changes are done before. in plugin processor.
    {
        
        for (int i=0;i<MATDIM;i++)
        {
            lbcfCH[i]->setSamplingRate(samplingRate);
        }
        for (int i=0;i<APCASC;i++)
        {
            apCH[i]->setSamplingRate(samplingRate);
        }
        fs_ = samplingRate;
        //pArr.setFS(fs_);
    }
    
    /// process buffered input (R+W Permission): apply sample-wise tick calculation, and replace buffer with output.
    void processBuffer(float *buffer, int numSamps)
    {
        for (int i=0;i<numSamps;i++)
        {
            *(buffer+i) = tick(*(buffer+i));
        }
    }
    
    /// sets all 16 LBCF delay line lengths, call this only after prepareToPlay(); aka after fs has been set
    void t60ToLBCFLength(float t60)
    {
        // force brickwall t_60 values
        if (t60<0.17)  t60 = 0.17;
        if (t60>10.0)  t60 = 10.0;
        /**
         < code snippet from Louis's MA Thesis - Hybrid Reverberation Algorithm>
         % S&L says 1~1.5 range
         range = [1 1.5];
         channels = 16;
         dM = 0.15/4*channels;  %mode density push =====0.15 for 4 channels

         fract_increment = (range(2)-range(1))/(channels-1);
         ratios = [1];
         for ind = 2:channels
             ratios = [ratios ratios(end)+fract_increment];
         end

         % S suggested 0.15*fs*t60 for enough mode density
         suml = fs*t60*dM;
         thv = suml/sum(ratios).*ratios;    <ratios vector (1.0~1.5) implemented in init()>
         thv = round(thv);
         parr = primegen(19,suml/2);
         */
        // below algorithm finds N (FIXED #Ch) lengths based on fs_ and t60, Gcs and Gk
        float t60insamp = fs_*t60;
        float suml = t60insamp*(0.33/4*MATDIM);
        
        for (int i=0;i<MATDIM;i++)
        {
            // calculate length
            lbcfLengths_[i] = pArr.closestPrime( lbcfLRatios_[i] / lbcfRatioSum_ * suml, nullptr );
            if (lbcfLengths_[i] < 7.0) lbcfLengths_[i] = 7.0;  //shouldn't happen, just in case it crashes with length modulation.
            // calculate gc
            lbcfGcs_[i] = pow(10.0f,-3*lbcfLengths_[i]/t60insamp);
            if (abs(lbcfGcs_[i])>1.0f) lbcfGcs_[i] = lbcfGcs_[i]/abs(lbcfGcs_[i]);  //shouldn't happen, but just in case it feeds back over 1.
            // inject two parameters
            lbcfCH[i]->injectComb(lbcfLengths_[i],lbcfGcs_[i]);
            
            //DBG("LBCF ch." << juce::String(i) << " length set to " << juce::String(lbcfLengths_[i]));
            //DBG("LBCF ch." << juce::String(i) << " g_c set to " << juce::String(lbcfGcs_[i]));
        }
        //DBG("***");
        // implemented above
        /**
         < code snippet from Louis's MA Thesis - Hybrid Reverberation Algorithm>
         % tao and epsilon factors J&C 1991
         tao = lbcf./fs;
         epsi = t60_nyqu/t60_zero;  <ignore one-zero filter parameter in this implementation>
         
         % damping filter gain
         gc   =  10^(-3*tao/t60_zero);  <t60_zero is just t60, supposedly>
         
         % lp_d is -beta_i == damping filter param
         lp_d =  -(1-2/(1+gc^(1+1/epsi)));  <ignore one-zero filter parameter in this implementation>
         
         % correction filter param
         beta = (1-sqrt(epsi))/(1+sqrt(epsi));  <ignore one-zero filter parameter in this implementation>
         
         gk   = sqrt(sum(lbcf./fs)/t60_zero);   <t60_zero is just t60, supposedly>
         */
    }
    
    void t60ToAPLength(float t60)
    {
        // force brickwall t_60 values
        if (t60<0.17)  t60 = 0.17f;
        if (t60>22.0) t60 = 22.0f;
        /**
         < code snippet from Louis's MA Thesis - Hybrid Reverberation Algorithm>
         %% find allpass filter lengths
         function [ap_4] = apfind (t60_in,ga,fs)
         thv = fs*t60_in*log10(1/ga)/3;
         thv = thv.*(1/3).^([0; 1; 2; 3]);  <ratios vector (0~1 (8-step float)) implemented in init()>
         parr = primegen(0,2*max(thv));
         for ind=1:4
             [~,closestIndex] = min(abs(parr-thv(ind)));
             ap_4(ind) = parr(closestIndex);
         end

         end
         */
        // below algorithm finds 8 (FIXED #Pipe) lengths based on fs_ and t60
        float maxl = fs_*t60*log10(1/0.708)/3.0f;
        
        for (int i=0;i<APCASC;i++)
        {
            apLengths_[i] = pArr.closestPrime( apLRatios_[i] * maxl, nullptr );
            apCH[i]->injectLength(apLengths_[i]);
            //DBG("AP ch." << juce::String(i) << " set to " << juce::String(apLengths_[i]));
        }
        //DBG("************************************************");
    }
    
    // maybe put this in t60ToAPLengths?
    std::vector<float> getExclusionVector () const
    {
        // send a copy of lbcfLengths
        std::vector<float> exclVec = lbcfLengths_;
        return exclVec;
    }
    
    void changeModulationAmp(float modAmp)
    {
        for (int i=0;i<MATDIM;i++)
        {
            lbcfCH[i]->injectModAmp(modAmp);
        }
    }
    
    void changeModulationSpd(float modSpd)
    {
        for (int i=0;i<MATDIM;i++)
        {
            lbcfCH[i]->injectModSpd(modSpd);
        }
    }
    
    void changeDamping(float dampFactor)
    {
        //dF = 0~10, damping = 2000~800, logN scale
        //y = -8332.31 log(0.0906911 (x+1))
        baseLPfreq_ = -500.438869709*log(dampFactor+1.0)+2000;
        for (int i=0;i<MATDIM;i++)
        {
            //DBG("set LBCF channel with LP = " + juce::String(baseLPfreq_*pow(10,biasLPexponents_[i])));
            //if (i%2==0) lbcfCH[i]->injectBiquadFreq(baseLPfreq_,1);
            //if (i%2==1) lbcfCH[i]->injectBiquadFreq(baseLPfreq_,2);
            lbcfCH[i]->injectBiquadFreq(baseLPfreq_*pow(10,biasLPexponents_[i]),1);
        }
    }
    
private:
    // basics
    float fs_;
    SignalType outSamp_ = 0.0f;
    
    // util
    primeArray pArr;
    
    // sub compenent params
    std::vector<float> lbcfLengths_ = std::vector<float>(MATDIM);
    std::vector<float> lbcfLRatios_ = std::vector<float>(MATDIM);
    float lbcfRatioSum_ = 0.0f;
    std::vector<float> lbcfGcs_ = std::vector<float>(MATDIM);
    float lbcfGk_ = 1.0f;
    float baseLPfreq_ = 0.0f;
    std::vector<float> biasLPexponents_ = std::vector<float>(MATDIM);
    
    std::unique_ptr< matrixOperation<SignalType> > fdnMat;
    
    std::vector<float> apLengths_ = std::vector<float>(APCASC);
    std::vector<float> apLRatios_ = std::vector<float>(APCASC);
    
    // sub components to be called
    std::unique_ptr< BiquadCombChannel<SignalType> > lbcfCH[MATDIM];
    std::unique_ptr< SchroederAPLine<SignalType> > apCH[APCASC];
    
    // internal variables
    std::vector<SignalType> x_matIn_ = std::vector<SignalType>(MATDIM);    // 16 channel temp numbers
    SignalType x_pipethru_ = 0.0f;
    
    // init function for constructor
    void init()
    {
        // initialize all unique_ptr objects
        for (int i=0;i<MATDIM;i++)
        {
            lbcfCH[i].reset( new BiquadCombChannel<SignalType> );
            lbcfCH[i]->channel_id = i;
        }
        fdnMat.reset( new matrixOperation<SignalType> );
        for (int i=0;i<APCASC;i++)
        {
            apCH[i].reset( new SchroederAPLine<SignalType> );
            apCH[i]->pipe_id = i;
            apCH[i]->injectGain(0.708f);
        }
                
        // precalculate the ratios
        lbcfLRatios_[0] = 1.0f;
        lbcfRatioSum_ += lbcfLRatios_[0];
        for (int i=1;i<MATDIM;i++)
        {
            lbcfLRatios_[i] = lbcfLRatios_[i-1] + 0.67f/(MATDIM-1);
            lbcfRatioSum_ += lbcfLRatios_[i];
        }
        for (int i=0;i<APCASC;i++)
        {
            apLRatios_[i] = pow( 1.0f/(APCASC-1), i );
        }
        
        // precalculate frequency bias exponent ratios
        
        biasLPexponents_[MATDIM-1] = 1.0;
        for (int i=MATDIM-2;i>=0;i--)
        {
            biasLPexponents_[i] = biasLPexponents_[i+1] - 0.5f/(MATDIM-1);
        }
        
        //DBG("LATE REV CH INIT DONE. ");
    }
    // sample-wise tick calculation funciton
    SignalType tick(SignalType inSamp)
    {
        // input to LBCF
        
        
        // LBCF to matrix
        for (int i=0;i<MATDIM;i++)
        {
            x_matIn_[i] = lbcfCH[i]->tick( inSamp + fdnMat->lastOutAtChan(i) );
        }
        
        // matrix to correction
        x_pipethru_ = fdnMat->tick(x_matIn_);
        
        // correction to AP
        
        for (int i=0;i<APCASC;i++)
        {
            x_pipethru_ = apCH[i]->tick(x_pipethru_);
        }
        
        outSamp_ = x_pipethru_;
        
        // output
        return outSamp_;
    }
        
};  //class bracket

