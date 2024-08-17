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
    preFilterSwitch.setImages(false,true,true,
                              img_LP,0.85f,juce::Colours::transparentWhite,         //up
                              juce::Image(),0.85f,juce::Colours::transparentWhite,  //hover
                              img_HP,0.85f,juce::Colours::transparentWhite          //down
                              );
    
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
    
    // Add LBCF sub components
    /*
    int xPosNow = 340;  //starting array of lbcf componenets
    int xWidthP = 40;   //increment each next array lbcf componenets
    for (int i=0;i<16;i++)
    {
        addAndMakeVisible(&lbcfFreq[i]);
        lbcfFreq[i].setSliderStyle(juce::Slider::LinearBarVertical);
        lbcfFreq[i].setBounds(xPosNow, 80, xWidthP, 120);
        lbcfFreq[i].setTextBoxStyle(juce::Slider::TextBoxAbove, true, 0, 0);
        lbcfFreq[i].setRange(20.0f, 20000.0f);
        lbcfFreq[i].setNumDecimalPlacesToDisplay(0);
        
        lbcfFreq[i].addListener(this);
        //default: depends
        
        
        addAndMakeVisible(&lbcfReso[i]);
        lbcfReso[i].setSliderStyle(juce::Slider::RotaryVerticalDrag);
        lbcfReso[i].setBounds(xPosNow, 220, xWidthP, xWidthP+36);
        lbcfReso[i].setTextBoxStyle(juce::Slider::TextBoxAbove, true, xWidthP, 16);
        lbcfReso[i].setNumDecimalPlacesToDisplay(2);
        lbcfReso[i].setRange(0.01f,10.0f);
        lbcfReso[i].setSkewFactor(0.333f);
        
        lbcfReso[i].addListener(this);
        //default: 1.0
        
        
        addAndMakeVisible(&lbcfFtyp[i]);
        lbcfFtyp[i].setSliderStyle(juce::Slider::LinearHorizontal);
        lbcfFtyp[i].setBounds(xPosNow, 320, xWidthP, xWidthP);
        lbcfFtyp[i].setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);
        lbcfFtyp[i].setRange(0.0f,3.f,1.0f);    //third arg is step-size
        
        lbcfFtyp[i].addListener(this);
        //default: 0.0
        
        addAndMakeVisible(&lbcfFtypInd[i]);
        lbcfFtypInd[i].setImage(img_FiltSwitch);
        lbcfFtypInd[i].setBoundingBox(juce::Rectangle<float>((float)xPosNow, 320.0f, (float)xWidthP, 20.0f));
        addAndMakeVisible(&lbcfFtypImg[i]);
        lbcfFtypImg[i].setBoundingBox(juce::Rectangle<float>((float)xPosNow, 360.0f, (float)xWidthP, 30.0f));
        ////////////////////////////////// WTF THIS DOESNT WORK
        // increment xpos with width
        xPosNow += xWidthP;
        // increment xpos with gap of x?
        xPosNow += 8;
    }
     */
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
    
    /*
    /// lbcf loop
    for (int i=0;i<16;i++)
    {
        if (subject == &lbcfFreq[i]){
            
        }else if(subject == &lbcfReso[i]){
            
        }else if(subject == &lbcfFtyp[i]){
            //since setImage() overrides the boundingbox for drawableImage, retrieve a temporary boundingBox here and apply later
            auto tmpBox = lbcfFtypImg[i].getBoundingBox();
            if (lbcfFtyp[i].getValue() == 0.0f){
                lbcfFtypImg[i].setImage(img_XF);
            }else if(lbcfFtyp[i].getValue() == 1.0f){
                lbcfFtypImg[i].setImage(img_LP);
            }else if(lbcfFtyp[i].getValue() == 2.0f){
                lbcfFtypImg[i].setImage(img_BP);
            }else if(lbcfFtyp[i].getValue() == 3.0f){
                lbcfFtypImg[i].setImage(img_HP);
            }
            //apply the same bounding box that was set in initialization
            lbcfFtypImg[i].setBoundingBox(tmpBox);
        }
    }
     */
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

