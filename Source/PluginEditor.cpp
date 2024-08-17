/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
HybridrevjoAudioProcessorEditor::HybridrevjoAudioProcessorEditor
    (HybridrevjoAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p)
    , audioProcessor (p)
    , valueTreeState(vts)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (660, 380);
    setResizable(false, false);
    
    // Add drop down menu
    // ** signal unimplemented
    menuSelectLabel.setText("Auditoria: ", juce::dontSendNotification);
    menuSelect.setBounds       (20, 20, 120, 40 );
    
    addAndMakeVisible(&menuSelect);
    addAndMakeVisible(&menuSelectLabel);
    menuSelect.addSectionHeading ("binaurals");
    menuSelect.addItem ("Aula Carolina", 1);
    menuSelect.addItem ("Lecture   ", 2);
    menuSelect.addItem ("Meeting   ", 3);
    menuSelect.addItem ("Office    ", 4);
    menuSelect.addItem ("Stairway  ", 5);
    
    
    
    menuSelect.addSeparator();
    menuSelect.addListener(this);
    menuAtt.reset (new ComboBoxAttachment (valueTreeState, "de-menuitem", menuSelect));
    
    // Add dry/wet mix slider object
    // ** mapped rt to DWmix object
    drywetMix.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    //drywetMix.setMouseDragSensitivity(80);
    addAndMakeVisible (&drywetMixLabel);
    addAndMakeVisible (&drywetMix);
    drywetMix.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 120, 20);
    drywetMixLabel.setText ("Dry/Wet Mix", juce::dontSendNotification);
    //true is to the left, false is above
    drywetMixLabel.attachToComponent (&drywetMix, false);
    drywetMix.setBounds     (20, 90, 120, 120 );
    drywetMix.addListener(this);
    drywetMixAtt.reset (new SliderAttachment (valueTreeState, "00-allmix", drywetMix));
    
    // Add DecayTime knob (t60)
    // ** mapped rt to LBCF and AP lengths
    decayTime.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    //decayTime.setMouseDragSensitivity(80);
    addAndMakeVisible(&decayTime);
    addAndMakeVisible(&decayTimeLabel);
    //decayTime.setSkewFactor(0.125f);//set skew in processor.cpp, normalized range
    decayTime.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 120, 20);
    decayTimeLabel.setText  ("Decay Time", juce::dontSendNotification);
    decayTimeLabel.attachToComponent(&decayTime, false);
    decayTime.setBounds     (160, 90, 120, 120 );
    decayTime.addListener(this);
    decayTimeAtt.reset (new SliderAttachment (valueTreeState, "02-t60", decayTime));
    
    // Add Modulation knobs (modamp and modspd)
    // ** mapped to LBCF only for now
    modAmp.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    addAndMakeVisible(&modAmp);
    addAndMakeVisible(&modAmpLabel);
    modAmp.setTextBoxStyle(juce::Slider::TextBoxRight, true, 120, 20);
    modAmpLabel.setText  ("Modulation Amount", juce::dontSendNotification);
    modAmpLabel.attachToComponent(&modAmp, true);
    modAmp.setBounds     (getWidth()-200, 90, 180, 60 );
    modAmp.addListener(this);
    modAmpAtt.reset (new SliderAttachment (valueTreeState, "03-modamp", modAmp));
    
    modSpd.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    addAndMakeVisible(&modSpd);
    addAndMakeVisible(&modSpdLabel);
    //modSpd.setSkewFactor(0.333f);//set skew in processor.cpp, normalized range
    modSpd.setTextBoxStyle(juce::Slider::TextBoxRight, true, 120, 20);
    modSpdLabel.setText  ("Modulation Rate", juce::dontSendNotification);
    modSpdLabel.attachToComponent(&modSpd, true);
    modSpd.setBounds     (getWidth()-200, 150, 180, 60 );
    modSpd.addListener(this);
    modSpdAtt.reset (new SliderAttachment (valueTreeState, "04-modspd", modSpd));
    
    // Add switch for LP/HP
    //addAndMakeVisible(&preFilterSwitch);
    //addAndMakeVisible(&preFilterSwitchLabel);
    
    preFilterSwitchLabel.setText ("preFiltering", juce::dontSendNotification);
    preFilterSwitchLabel.attachToComponent (&preFilterSwitch, false);
    
    preFilterSwitch.setClickingTogglesState(true);
    preFilterSwitch.setBounds(20, 290, 80, 40 );
//    preFilterSwitch.setImages(false,true,true,
//                              img_LP,0.85f,juce::Colours::transparentWhite,         //up
//                              juce::Image(),0.85f,juce::Colours::transparentWhite,  //hover
//                              img_HP,0.85f,juce::Colours::transparentWhite          //down
//                              );
    
    preFilterSwitch.addListener(this);
    preFilterSwitchAtt.reset (new ButtonAttachment (valueTreeState, "01-prefilt", preFilterSwitch));
    
    // Add damping knob
    dampSet.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    addAndMakeVisible(&dampSet);
    addAndMakeVisible(&dampSetLabel);
    dampSet.setTextBoxStyle(juce::Slider::TextBoxRight, true, 120, 20);
    dampSetLabel.setText  ("Damping", juce::dontSendNotification);
    dampSetLabel.attachToComponent(&dampSet, true);
    dampSet.setBounds     (getWidth()-200, 210, 180, 60 );
    dampSet.addListener(this);
    dampSetAtt.reset (new SliderAttachment (valueTreeState, "05-damping", dampSet));
    
}

HybridrevjoAudioProcessorEditor::~HybridrevjoAudioProcessorEditor()
{
}

//==============================================================================
void HybridrevjoAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::darkgrey);

    g.setColour (juce::Colours::grey);
    g.setFont (15.0f);
    g.drawFittedText ("LouisHybridReverb " + juce::String(ProjectInfo::versionString), getLocalBounds(), juce::Justification::topLeft, 1);
    g.drawFittedText ("Advanced Parameters: ", getLocalBounds(), juce::Justification::topRight, 1);
    
}

void HybridrevjoAudioProcessorEditor::sliderValueChanged(juce::Slider* subject)
{
    if (subject == &drywetMix)
    {
        audioProcessor.mDWM[0]->injectProportion(drywetMix.getValue());
        audioProcessor.mDWM[1]->injectProportion(drywetMix.getValue());
    }
    
    if (subject == &decayTime)
    {
        // change t60-related in LBCF and AP
        audioProcessor.mLatRev[0]->t60ToLBCFLength(decayTime.getValue());
        audioProcessor.mLatRev[0]->t60ToAPLength(decayTime.getValue());
        audioProcessor.mLatRev[1]->t60ToLBCFLength(decayTime.getValue());
        audioProcessor.mLatRev[1]->t60ToAPLength(decayTime.getValue());
    }
    
    if (subject == &modAmp)
    {
        audioProcessor.mLatRev[0]->changeModulationAmp(modAmp.getValue());
        audioProcessor.mLatRev[1]->changeModulationAmp(modAmp.getValue());
    }
    
    if (subject == &modSpd)
    {
        audioProcessor.mLatRev[0]->changeModulationSpd(modSpd.getValue());
        audioProcessor.mLatRev[1]->changeModulationSpd(modSpd.getValue());
    }
    
    if (subject == &dampSet)
    {
        audioProcessor.mLatRev[0]->changeDamping(dampSet.getValue());
        audioProcessor.mLatRev[1]->changeDamping(dampSet.getValue());
    }
    
}

void HybridrevjoAudioProcessorEditor::comboBoxChanged(juce::ComboBox* subject)
{
    audioProcessor.mConMan->changeIR(subject->getSelectedId());
    //textLabel.setFont (textFont);
}

void HybridrevjoAudioProcessorEditor::buttonClicked(juce::Button* subject)
{
    if (subject == &preFilterSwitch){
        // change the filter coefficients in FilterIntegration
        // ...
        audioProcessor.mFI[0]->changeCoeffs(preFilterSwitch.getState());
        audioProcessor.mFI[1]->changeCoeffs(preFilterSwitch.getState());
        
    }
}

void HybridrevjoAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

