/*
  ==============================================================================

    ConvolutionManager.h
    Created: 16 Aug 2024 11:54:14am
    Author:  Louis Deng

  ==============================================================================
*/

#pragma once

template <typename SignalType>
class ConvolutionManager
{
public:
    ConvolutionManager()
    {
        init();
    }
    
    ~ConvolutionManager()
    {
    }
    
    void processBlock(juce::dsp::AudioBlock<SignalType>& inputBlock)
    {
        // update context
        juce::dsp::ProcessContextReplacing<SignalType> pContext = juce::dsp::ProcessContextReplacing<SignalType>(inputBlock);
        // process
        conUnit->process(pContext);
    }
    
    void setSamplingRate(float fs)
    {
        pSpec_.sampleRate = fs;
        conUnit->prepare(pSpec_);
    }
    
    void changeIR(int inputArg)
    {
        switch(inputArg){
            case 1: 
                conUnit->loadImpulseResponse(BinaryData::aula_wav,BinaryData::aula_wavSize,
                                             juce::dsp::Convolution::Stereo::yes,
                                             juce::dsp::Convolution::Trim::no, 0,
                                             juce::dsp::Convolution::Normalise::yes
                                             );
                break;
            case 2:
                conUnit->loadImpulseResponse(BinaryData::lecture_wav,BinaryData::lecture_wavSize,
                                             juce::dsp::Convolution::Stereo::yes,
                                             juce::dsp::Convolution::Trim::no, 0,
                                             juce::dsp::Convolution::Normalise::yes
                                             );
                break;
            case 3:
                conUnit->loadImpulseResponse(BinaryData::meeting_wav,BinaryData::meeting_wavSize,
                                             juce::dsp::Convolution::Stereo::yes,
                                             juce::dsp::Convolution::Trim::no, 0,
                                             juce::dsp::Convolution::Normalise::yes
                                             );
                break;
            case 4:
                conUnit->loadImpulseResponse(BinaryData::office_wav,BinaryData::office_wavSize,
                                             juce::dsp::Convolution::Stereo::yes,
                                             juce::dsp::Convolution::Trim::no, 0,
                                             juce::dsp::Convolution::Normalise::yes
                                             );
                break;
            case 5:
                conUnit->loadImpulseResponse(BinaryData::stair_wav,BinaryData::stair_wavSize,
                                             juce::dsp::Convolution::Stereo::yes,
                                             juce::dsp::Convolution::Trim::no, 0,
                                             juce::dsp::Convolution::Normalise::yes
                                             );
                break;
            default:
                break;
        }
        conUnit->reset();
    }
    
private:
    juce::dsp::Convolution::NonUniform nuStruct_;
    std::unique_ptr< juce::dsp::Convolution > conUnit;
    
    juce::dsp::ProcessSpec pSpec_;
    
    void init()
    {
        nuStruct_.headSizeInSamples = 256;
        conUnit.reset(new juce::dsp::Convolution(nuStruct_));
        pSpec_.numChannels = 2;
        //default max size
        pSpec_.maximumBlockSize = 8192;
    }
    
};
