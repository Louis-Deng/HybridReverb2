/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ConvolutionManager.h"
#include "LateReverbManager.h"
#include "FilterIntegration.h"
#include "DWmixer.h"

//==============================================================================
/**
*/
class HybridrevjoAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    HybridrevjoAudioProcessor();
    ~HybridrevjoAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    std::unique_ptr< FilterIntegration<double> > mFI[2];
    std::unique_ptr< ConvolutionManager<float> > mConMan;
    std::unique_ptr< LateReverbManager<double> > mLatRev[2];
    
    std::unique_ptr< DWmixer<double> > mDWM[2];
    //==============================================================================
    
private:
    //==============================================================================
    
    //==============================================================================
    int mBufferSize;
    float mSampleRate;
    juce::AudioProcessorValueTreeState parameters;
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HybridrevjoAudioProcessor)
};

