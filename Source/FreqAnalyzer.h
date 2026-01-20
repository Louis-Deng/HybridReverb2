/*
  ==============================================================================

    FreqAnalyzer.h
    Created: 9 Sep 2024 4:13:54pm
    Author:  Louis Deng

    fft-based processing, includes dry and wet proportions, 2 channels
     
    audio signals being injected to the buffer in this instance
    the freq-domain points is passed to pluginEditor to be displayed
  ==============================================================================
*/

#pragma once
#include "SpectrumUtil.h"
// all FFT-related objects in this header has fixed order: fftsize = 2048 (2e11)
// fft :: 2^N sized fft -- 2^11 = 2048, ~23.4fps
const uint32_t FFTORDER = 11;

/// Sub-component of the freq-analyzer Component, fft-operation, data processing and lastly numerical normalization
class fftUnit
{
public:
    fftUnit()
    {
        // initialize juce fft object
        fftOp.reset(new juce::dsp::FFT(FFTORDER));
        fftBufferSize = (fftOp->getSize()) << 1;
        // make two buffers the size of fft buffersize
        iBuffer.resize(fftBufferSize);
        oBuffer.resize(fftBufferSize);
        // nyquist size is half of total fftsize
        iterNyquist = fftBufferSize>>1;
        //DBG("fft buffer size is " + juce::String(fftBufferSize));
    }
    
    ~fftUnit()
    {
    }
    
    /// inject a single sample to this fft unit, returns if this unit is ready to show its complete spectrum
    bool injectSample (float input)
    {
        iBuffer[iterBuffer] = input;
        iterBuffer++;
        bool ready = false;
        
        if ( !(iterBuffer<iterNyquist) )
        {
            // reset buffer iterator
            iterBuffer = 0;
            // copy i to o
            oBuffer = iBuffer;
            // calculate o
            fftOp->performFrequencyOnlyForwardTransform(&(oBuffer[0]));
            ready = true;
        }
        return ready;
    }
    
    std::vector<float> getBuffer() const { return oBuffer; }
    
    int getIterB() const { return iterBuffer; }
    
private:
    /// I/O Buffer
    std::vector<float> iBuffer;
    std::vector<float> oBuffer;
    
    /// iteration related parameter
    uint32_t iterNyquist;
    uint32_t iterBuffer = 0;
    
    uint32_t fftBufferSize;
    std::unique_ptr<juce::dsp::FFT> fftOp;
};

/// Channel component - includes dry and wet
class FreqAnalChannel : public juce::Component
{
public:
    FreqAnalChannel(uint32_t chan)
    {
        // init
        dryUnit.reset( new fftUnit );
        wetUnit.reset( new fftUnit );
        
        iterBuffer = dryUnit->getIterB();
        
        // should be 1/2 fftSize
        dryMags.resize(pow(2,iterBuffer));
        wetMags.resize(pow(2,iterBuffer));
        
        // set bounds for graphics
        /*
        int x0 = 15;
        int y0 = 295;
        int xi = 315;
        int yi = 135;
        for (int v=0; v<4; v++){
            bounds[v][0] = chan*xi+x0+(v%2)*xi;
            bounds[v][1] = y0+(v/2)*yi;
        }
         */
        xPos = 15+(int)chan*315;
        
    }
    ~FreqAnalChannel()
    {
    }
    
    void injectSampleTo (float inputSample, uint32_t drywet)
    {
        switch(drywet){
            case 0:
                dryReady = dryUnit->injectSample( inputSample );
                break;
            case 1:
                wetReady = wetUnit->injectSample( inputSample );
                break;
        }
        
        if (dryReady && wetReady)
        {
            graphGen();
            repaint();
            dryReady = false;
            wetReady = false;
        }
    }
    
    /// inherited from juce::component
    void paint(juce::Graphics& g) override
    {
        juce::Rectangle<int> rectArea (xPos, 295, 315, 270);
        g.setColour(juce::Colours::white);
        g.drawRect(rectArea);
    }
    
private:
    // iterB
    int iterBuffer;
    
    // dry and wet fft units
    std::unique_ptr<fftUnit> dryUnit;
    std::unique_ptr<fftUnit> wetUnit;
    
    // ready to show content
    bool dryReady;
    bool wetReady;
    
    // magnitude in dB graph
    std::vector<float> dryMags;
    std::vector<float> wetMags;
    
    /// Component bounds (0,0) (0,x) (0,y) (x,y)
    //std::vector<std::vector<int>> bounds = std::vector<std::vector<int>>(4,std::vector<int>(2));
    int xPos;
    
    void graphGen()
    {
        
    }
    
};

//#include <juce_FFT.h>
/// Component Freq Analyzer, data organizing
class FreqAnalyzer : public juce::Component
{
    
public:
    FreqAnalyzer()
    {
        init();
    }
    ~FreqAnalyzer()
    {
    }
    
    /// input a single sample to specified channel and dry/wet configuration
    void injectSampleToTo(float inputSample, uint32_t leftright, uint32_t drywet)
    {
        switch(leftright){
            case 0:
                LFAC.injectSampleTo(inputSample,drywet);
                break;
            case 1:
                RFAC.injectSampleTo(inputSample,drywet);
                break;
        }
    }
        
    
    
private:
    // bin number max
    uint32_t iterBLimit = pow(2,FFTORDER-1);
    
    /// leftright, drywet buffers
    FreqAnalChannel LFAC = FreqAnalChannel(0);
    FreqAnalChannel RFAC = FreqAnalChannel(1);
    
    void init()
    {
        addChildComponent(LFAC,0);
        addChildComponent(RFAC,0);
        //repaint();
    }
    /// common function call to all four buffer dupes
    /*
    void processAll4 (void (*f)(fftUnit b, uint32_t arg1), uint32_t arg1)
    {
        f(LD,arg1);
        f(RD,arg1);
        f(LW,arg1);
        f(RW,arg1);
    }
    */
    
};

