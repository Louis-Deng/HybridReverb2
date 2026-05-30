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
    setSize (660, 580); //y+200 to fit graphic window
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
    
    // Add graphic sub-section
    //???.setBounds         (15, getHeight()-285, 630, 270);    // 15px margin on L,R,Bottom
    
}

HybridrevjoAudioProcessorEditor::~HybridrevjoAudioProcessorEditor()
{
    // Remove listeners you added in the constructor
    drywetMix.removeListener(this);
    decayTime.removeListener(this);
    modAmp.removeListener(this);
    modSpd.removeListener(this);
    dampSet.removeListener(this);
    menuSelect.removeListener(this);
    preFilterSwitch.removeListener(this);

    // Explicitly reset attachments so they drop parameter listeners
    drywetMixAtt.reset();
    decayTimeAtt.reset();
    modAmpAtt.reset();
    modSpdAtt.reset();
    dampSetAtt.reset();
    preFilterSwitchAtt.reset();
    menuAtt.reset();
}
//==============================================================================
void HybridrevjoAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (juce::Colours::darkgrey);

    g.setColour (juce::Colours::grey);
    g.setFont (15.0f);
    g.drawFittedText ("LouisHybridReverb " + juce::String(ProjectInfo::versionString), getLocalBounds(), juce::Justification::topLeft, 1);
    g.drawFittedText ("Advanced Parameters:               ", getLocalBounds(), juce::Justification::topRight, 1);
    
    // graphic sub-section
    juce::Rectangle<int> rectArea (15, getHeight()-285, 630, 270);
    g.setColour(juce::Colours::black);
    g.fillRect(rectArea);
    
}

void HybridrevjoAudioProcessorEditor::sliderValueChanged(juce::Slider* subject)
{
    juce::ignoreUnused(subject);
    // UI controls are attached to parameters; DSP reacts via ParameterManager listeners.
}

void HybridrevjoAudioProcessorEditor::comboBoxChanged(juce::ComboBox* subject)
{
    juce::ignoreUnused(subject);
    // ComboBoxAttachment updates the parameter; DSP reacts via ParameterManager.
}

void HybridrevjoAudioProcessorEditor::buttonClicked(juce::Button* subject)
{
    juce::ignoreUnused(subject);
    // ButtonAttachment updates the parameter; DSP reacts via ParameterManager.
}

void HybridrevjoAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
