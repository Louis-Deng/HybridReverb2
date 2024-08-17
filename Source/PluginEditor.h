/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class HybridrevjoAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Slider::Listener, public juce::ComboBox::Listener, public juce::ImageButton::Listener
{
public:
    HybridrevjoAudioProcessorEditor (HybridrevjoAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~HybridrevjoAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider*) override;
    void comboBoxChanged(juce::ComboBox*) override;
    void buttonClicked(juce::Button*) override;
    //void juce::ImageButton::paintButton(juce::Graphics& g, bool drawHL, bool drawDN) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    
    // memory of VTS
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment ComboBoxAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;
    
    // main
    HybridrevjoAudioProcessor& audioProcessor;
    
    // memory of VTS
    juce::AudioProcessorValueTreeState& valueTreeState;
    
    /// Dry/Wet Mix knob
    juce::Slider drywetMix;
    juce::Label drywetMixLabel;
    std::unique_ptr<SliderAttachment> drywetMixAtt;
    
    /// IR selection menu
    juce::Label menuSelectLabel;
    juce::ComboBox menuSelect;
    std::unique_ptr<ComboBoxAttachment> menuAtt;
    
    /// Decay Time knob
    juce::Slider decayTime;
    juce::Label decayTimeLabel;
    std::unique_ptr<SliderAttachment> decayTimeAtt;
    
    /// Modulation knobs
    juce::Slider modAmp;
    juce::Label modAmpLabel;
    std::unique_ptr<SliderAttachment> modAmpAtt;
    juce::Slider modSpd;
    juce::Label modSpdLabel;
    std::unique_ptr<SliderAttachment> modSpdAtt;
    
    //image resources to be loaded and kept
    const juce::Image img_XF = juce::ImageCache::getFromMemory(BinaryData::OFFFilter_png, BinaryData::OFFFilter_pngSize);
    const juce::Image img_LP = juce::ImageCache::getFromMemory(BinaryData::LPFilter_png, BinaryData::LPFilter_pngSize);
    const juce::Image img_HP = juce::ImageCache::getFromMemory(BinaryData::HPFilter_png, BinaryData::HPFilter_pngSize);
    const juce::Image img_BP = juce::ImageCache::getFromMemory(BinaryData::BPFilter_png, BinaryData::BPFilter_pngSize);
    
    const juce::Image img_FiltSwitch = juce::ImageCache::getFromMemory(BinaryData::FilterSwitch_png, BinaryData::FilterSwitch_pngSize);
    
    /// preFiltering toggle switch
    juce::ImageButton preFilterSwitch;
    juce::Label preFilterSwitchLabel;
    std::unique_ptr<ButtonAttachment> preFilterSwitchAtt;
    
    /// damping knob
    juce::Slider dampSet;
    juce::Label dampSetLabel;
    std::unique_ptr<SliderAttachment> dampSetAtt;
    
    /// LBCF 16-channel
    /*
    juce::Slider lbcfFreq[16];
    std::unique_ptr<SliderAttachment> lbcfFreqAtt[16];
    juce::Slider lbcfReso[16];
    std::unique_ptr<SliderAttachment> lbcfResoAtt[16];
    juce::Slider lbcfFtyp[16];
    std::unique_ptr<SliderAttachment> lbcfFtypAtt[16];
    juce::DrawableImage lbcfFtypInd[16];
    juce::DrawableImage lbcfFtypImg[16];
    */

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HybridrevjoAudioProcessorEditor)
};
