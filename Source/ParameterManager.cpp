#include "ParameterManager.h"

ParameterManager::ParameterManager(juce::AudioProcessorValueTreeState& state,
                                   DWmixer<double>* dwmL, DWmixer<double>* dwmR,
                                   LateReverbManager<double>* lrL, LateReverbManager<double>* lrR,
                                   //FilterIntegration<double>* fiL, FilterIntegration<double>* fiR,
                                   ConvolutionManager<float>* conManager)
    : apvts(state), conMan(conManager)
{
    dwm[0] = dwmL; dwm[1] = dwmR;
    latRev[0] = lrL; latRev[1] = lrR;
    //fi[0] = fiL; fi[1] = fiR;

    // Cache raw parameter pointers
    pMix     = apvts.getRawParameterValue("00-allmix");
    pMenu    = apvts.getRawParameterValue("de-menuitem");
    //pPreFilt = apvts.getRawParameterValue("01-prefilt");
    pT60     = apvts.getRawParameterValue("02-t60");
    pModAmp  = apvts.getRawParameterValue("03-modamp");
    pModSpd  = apvts.getRawParameterValue("04-modspd");
    pDamp    = apvts.getRawParameterValue("05-damping");

    registerListeners();
}

ParameterManager::~ParameterManager()
{
    unregisterListeners();
}

void ParameterManager::registerListeners()
{
    apvts.addParameterListener("00-allmix",  this);
    apvts.addParameterListener("de-menuitem", this);
    //apvts.addParameterListener("01-prefilt", this);
    apvts.addParameterListener("02-t60",     this);
    apvts.addParameterListener("03-modamp",  this);
    apvts.addParameterListener("04-modspd",  this);
    apvts.addParameterListener("05-damping", this);
}

void ParameterManager::unregisterListeners()
{
    apvts.removeParameterListener("00-allmix",  this);
    apvts.removeParameterListener("de-menuitem", this);
    //apvts.removeParameterListener("01-prefilt", this);
    apvts.removeParameterListener("02-t60",     this);
    apvts.removeParameterListener("03-modamp",  this);
    apvts.removeParameterListener("04-modspd",  this);
    apvts.removeParameterListener("05-damping", this);
}

void ParameterManager::parameterChanged(const juce::String& parameterID, float newValue)
{
    // Called on audio thread; ensure DSP methods are RT-safe
    if (! dspReady.load())
        return;

    if (parameterID == "00-allmix")
    {
        if (dwm[0]) dwm[0]->injectProportion(newValue);
        if (dwm[1]) dwm[1]->injectProportion(newValue);
    }
    else if (parameterID == "02-t60")
    {
        if (latRev[0]) { latRev[0]->t60ToLBCFLength(newValue); latRev[0]->t60ToAPLength(newValue); }
        if (latRev[1]) { latRev[1]->t60ToLBCFLength(newValue); latRev[1]->t60ToAPLength(newValue); }
    }
    else if (parameterID == "03-modamp")
    {
        if (latRev[0]) latRev[0]->changeModulationAmp(newValue);
        if (latRev[1]) latRev[1]->changeModulationAmp(newValue);
    }
    else if (parameterID == "04-modspd")
    {
        if (latRev[0]) latRev[0]->changeModulationSpd(newValue);
        if (latRev[1]) latRev[1]->changeModulationSpd(newValue);
    }
    else if (parameterID == "05-damping")
    {
        if (latRev[0]) latRev[0]->changeDamping(newValue);
        if (latRev[1]) latRev[1]->changeDamping(newValue);
    }
    else if (parameterID == "01-prefilt")
    {
        /*????????????? gonna rewrite this
        
        if (fi[0]) fi[0]->changeCoeffs(1);
        if (fi[1]) fi[1]->changeCoeffs(1);
         */
    }
    else if (parameterID == "de-menuitem")
    {
        if (conMan) conMan->changeIR((int)newValue);
    }
}

void ParameterManager::applyAllParametersToDSP()
{
    if (! dspReady.load())
        return;

    const auto mix    = pMix    ? pMix->load()    : 1.0f;
    const auto t60    = pT60    ? pT60->load()    : 3.2f;
    const auto modAmp = pModAmp ? pModAmp->load() : 14.3f;
    const auto modSpd = pModSpd ? pModSpd->load() : 1.47f;
    const auto damp   = pDamp   ? pDamp->load()   : 8.5f;
    //const auto pre    = pPreFilt? pPreFilt->load(): 0.0f;
    const auto menu   = pMenu   ? (int)pMenu->load() : 1;

    for (int ch = 0; ch < 2; ch++)
    {
        if (dwm[ch])    dwm[ch]->injectProportion(mix);
        if (latRev[ch]) { latRev[ch]->t60ToLBCFLength(t60); latRev[ch]->t60ToAPLength(t60); latRev[ch]->changeModulationAmp(modAmp); latRev[ch]->changeModulationSpd(modSpd); latRev[ch]->changeDamping(damp); }
        //if (fi[ch])     fi[ch]->changeCoeffs(pre >= 0.5f);
    }

    if (conMan) conMan->changeIR(menu);
}

void ParameterManager::setDSPReady(bool shouldBeReady)
{
    dspReady.store(shouldBeReady);
}

bool ParameterManager::isDSPReady() const
{
    return dspReady.load();
}
