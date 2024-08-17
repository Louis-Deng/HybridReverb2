/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
HybridrevjoAudioProcessor::HybridrevjoAudioProcessor()
    : mBufferSize(0)
    , mSampleRate(0.0)
    , parameters (*this, nullptr, juce::Identifier ("PVT"),
      {
        std::make_unique<juce::AudioParameterFloat>     (juce::ParameterID{"00-allmix",1},// {parameterID,parameterVersionHint>0: for AU}
                                                         "Dry/Wet Mix",       // parameter name
                                                         //min,max,increment,*skew
                                                         juce::NormalisableRange(0.000f,1.000f,0.001f),
                                                         1.000f,              // default value
                                                         juce::AudioParameterFloatAttributes().withStringFromValueFunction ([] (auto x, auto) { return juce::String(x*100.0f)+" %";
                                                        })  //lambda function parenthesis
                                                         ), //parameter float parenthesis
        
        std::make_unique<juce::AudioParameterInt>       (juce::ParameterID{"de-menuitem",1},
                                                         "Menu Item",
                                                         1, 5, 1),
        std::make_unique<juce::AudioParameterBool>      (juce::ParameterID{"01-prefilt",2},
                                                         "PreFiltering",
                                                         false),
        std::make_unique<juce::AudioParameterFloat>     (juce::ParameterID{"02-t60",3},
                                                         "Decay Time t60",
                                                         juce::NormalisableRange(0.170f,10.0f,0.0001f,0.425f),
                                                         3.200f,
                                                         juce::AudioParameterFloatAttributes().withStringFromValueFunction ([] (auto x, auto) { 
                                                            if(x<1.0f) {return juce::String(x*1000.0f)+" ms";}
                                                            else {return juce::String(x)+" s";}
                                                        })
                                                         ),
        std::make_unique<juce::AudioParameterFloat>     (juce::ParameterID{"03-modamp",4},
                                                         "Modulation Amt",
                                                         juce::NormalisableRange(0.000f,20.000f,0.0001f,0.432f),
                                                         14.30f,
                                                         juce::AudioParameterFloatAttributes().withStringFromValueFunction ([] (auto x, auto) { return juce::String(x)+" samps";
                                                        })
                                                         ),
        std::make_unique<juce::AudioParameterFloat>     (juce::ParameterID{"04-modspd",4},
                                                         "Modulation Rate",
                                                         juce::NormalisableRange(0.000f,2.000f,0.0001f,0.5f),
                                                         1.470f,
                                                         juce::AudioParameterFloatAttributes().withStringFromValueFunction ([] (auto x, auto) { return juce::String(x)+" Hz";
                                                        })
                                                         ),
        std::make_unique<juce::AudioParameterFloat>     (juce::ParameterID{"05-damping",5},
                                                         "Damping",
                                                         juce::NormalisableRange(0.0f,10.0f,0.01f),
                                                         8.5f,
                                                         juce::AudioParameterFloatAttributes().withStringFromValueFunction ([] (auto x, auto) { return juce::String(x)+" atten";
                                                        })
                                                         )
      }//4th item parenthesis
                  )//parameters parenthesis
/*#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif*/
{
    // init late reverb manager
    mFI[0].reset(new FilterIntegration<double>);
    mFI[1].reset(new FilterIntegration<double>);
    mConMan.reset(new ConvolutionManager<float>);
    mLatRev[0].reset(new LateReverbManager<double>);
    mLatRev[1].reset(new LateReverbManager<double>);
    // init dry/wet mixer
    mDWM[0].reset(new DWmixer<double>);
    mDWM[1].reset(new DWmixer<double>);
    
}

HybridrevjoAudioProcessor::~HybridrevjoAudioProcessor()
{
}

//==============================================================================
const juce::String HybridrevjoAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool HybridrevjoAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool HybridrevjoAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool HybridrevjoAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double HybridrevjoAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int HybridrevjoAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int HybridrevjoAudioProcessor::getCurrentProgram()
{
    return 0;
}

void HybridrevjoAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String HybridrevjoAudioProcessor::getProgramName (int index)
{
    return {};
}

void HybridrevjoAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void HybridrevjoAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback initialisation that you need..
    // DBG("PROCESSOR: PREPARE TO PLAY");
    
    // set sampleRates + preparetoplay
    mConMan->setSamplingRate(sampleRate);
    mLatRev[0]->setSamplingRate(sampleRate);
    mLatRev[1]->setSamplingRate(sampleRate);
    
}

void HybridrevjoAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool HybridrevjoAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void HybridrevjoAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    
//    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; i++)
//        buffer.clear (i, 0, buffer.getNumSamples());
     
    
    
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    
    // copy dry input all channels
    juce::AudioBuffer<float> drySamples;
    drySamples.makeCopyOf(buffer);
    
    // stereo convolution
    // wrap in audioblock, same pointer contained
    juce::dsp::AudioBlock<float> pBlock = juce::dsp::AudioBlock<float>(buffer);
    mConMan->processBlock(pBlock);
    
    // for each output channel
    for (int channel = 0; channel < totalNumOutputChannels; channel++)
    {
        int dryReadFromChan = 0;
        if (totalNumInputChannels == 1 && totalNumOutputChannels == 1)
        {
            // mono -> mono
            dryReadFromChan = 0;
        }
        else if (totalNumInputChannels == 1 && totalNumOutputChannels == 2)
        {
            // mono -> stereo
            dryReadFromChan = 0;
            // copy inL to inR in buffer
            buffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples());
        }
        else
        {
            // stereo -> stereo
            dryReadFromChan = channel;
        }
        
        auto* drySamplesPtr = drySamples.getReadPointer(dryReadFromChan);
        auto* channelDSP = buffer.getWritePointer(channel);
        
        // late reverberation
        //mFI[channel]->processBuffer(channelDSP,buffer.getNumSamples());
        mLatRev[channel]->processBuffer(channelDSP,buffer.getNumSamples());
        
        // dry wet mixer
        mDWM[channel]->processBuffer(drySamplesPtr,channelDSP,buffer.getNumSamples());
        
    }
}

//==============================================================================
bool HybridrevjoAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* HybridrevjoAudioProcessor::createEditor()
{
    return new HybridrevjoAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void HybridrevjoAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
    //DBG("SAVED STATE INFO IN" << "..." << "! ");
}

void HybridrevjoAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
     
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName (parameters.state.getType()))
        {
            parameters.replaceState (juce::ValueTree::fromXml (*xmlState));
            //DBG("GOT STATE INFO FROM " << "..." << "! ");
        }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HybridrevjoAudioProcessor();
}
