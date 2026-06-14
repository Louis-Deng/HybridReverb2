#pragma once

#include <JuceHeader.h>
#include "ConvolutionManager.h"
#include "LateReverbManager.h"
#include "FilterIntegration.h"
#include "DWmixer.h"

// ParameterManager isolates parameter->DSP routing so DSP initializes and updates
// even when no UI is open. It listens to APVTS changes and applies them to DSP.
class ParameterManager : public juce::AudioProcessorValueTreeState::Listener
{
public:
    ParameterManager(juce::AudioProcessorValueTreeState& state,
                     DWmixer<double>* dwmL, DWmixer<double>* dwmR,
                     LateReverbManager<double>* lrL, LateReverbManager<double>* lrR,
                     //FilterIntegration<double>* fiL, FilterIntegration<double>* fiR,
                     ConvolutionManager<float>* conMan);
    ~ParameterManager() override;

    // APVTS listener
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    // Apply current values to DSP (call on startup, after SR changes, after state load)
    void applyAllParametersToDSP();
    void setDSPReady(bool shouldBeReady);
    bool isDSPReady() const;

private:
    juce::AudioProcessorValueTreeState& apvts;

    DWmixer<double>* dwm[2] { nullptr, nullptr };
    LateReverbManager<double>* latRev[2] { nullptr, nullptr };
    //FilterIntegration<double>* fi[2] { nullptr, nullptr };
    ConvolutionManager<float>* conMan { nullptr };

    // Cached raw parameter pointers (denormalised floats)
    std::atomic<float>* pMix    = nullptr;   // "00-allmix"
    std::atomic<float>* pMenu   = nullptr;   // "de-menuitem"
    //std::atomic<float>* pPreFilt= nullptr;   // "01-prefilt"
    std::atomic<float>* pT60    = nullptr;   // "02-t60"
    std::atomic<float>* pModAmp = nullptr;   // "03-modamp"
    std::atomic<float>* pModSpd = nullptr;   // "04-modspd"
    std::atomic<float>* pDamp   = nullptr;   // "05-damping"
    std::atomic<bool> dspReady { false };

    void registerListeners();
    void unregisterListeners();
};
