#include "CustomAudioProcessor.h"
#include "CustomAudioEditor.h"
#include "CustomFaderBank.h"

CustomAudioProcessor::CustomAudioProcessor() 
: AudioProcessor (BusesProperties()
                  #if ! JucePlugin_IsMidiEffect
                  #if ! JucePlugin_IsSynth
                  .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                  #endif
                  .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                  #endif
                  ),
parameters(*this, nullptr, juce::Identifier("PARAMETERS"),
    juce::AudioProcessorValueTreeState::ParameterLayout {
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "dryWet",  1}, "dryWet",
      juce::NormalisableRange<float>(0.f, 100.f, 0.01f),0.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "binSmooth",  1}, "binSmooth",
      juce::NormalisableRange<float>(0.f, 0.999f, 0.001f),0.5f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "freqSmooth",  1}, "freqSmooth",
      juce::NormalisableRange<float>(0.f, 0.999f, 0.001f),0.97f), 
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "ampSmooth",  1}, "ampSmooth",
      juce::NormalisableRange<float>(0.f, 0.999f, 0.001f),0.97f),       
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "freqScale",  1}, "freqScale",
      juce::NormalisableRange<float>(0.1f, 5.f, 0.01f),1.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "modFreq",  1}, "modFreq",
      juce::NormalisableRange<float>(0.1f, 5.f, 0.01f),1.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "modAmp",  1}, "modAmp",
      juce::NormalisableRange<float>(0.f, 100.f, 0.01f),0.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "stereoMode",  1}, "stereoMode",
      juce::NormalisableRange<float>(0, 1, 1), 1),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "modPanFreq",  1}, "modPanFreq",
      juce::NormalisableRange<float>(0.1f, 10.f, 0.01f),1.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "equalTemperamentMode",  1}, "equalTemperamentMode",
      juce::NormalisableRange<float>(0, 1, 1), 0),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "equalTemperament",  1}, "equalTemperament",
      juce::NormalisableRange<float>(2, 53, 1), 12),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "tuning",  1}, "tuning",
      juce::NormalisableRange<float>(220.f, 880.f, 0.01f), 440.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "mix",  1}, "mix",
      juce::NormalisableRange<float>(0.f, 100.f, 0.01f),0.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "delayAll",  1}, "delayAll",
      juce::NormalisableRange<float>(0.f, 1000.f, 0.01f),5.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "delayCom",  1}, "delayCom",
      juce::NormalisableRange<float>(0.f, 1000.f, 0.01f),39.85f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "amp1",  1}, "amp1",
      juce::NormalisableRange<float>(0.f, 1.f, 0.01f),1.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "amp2",  1}, "amp2",
      juce::NormalisableRange<float>(0.f, 1.f, 0.01f),1.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "amp3",  1}, "amp3",
      juce::NormalisableRange<float>(0.f, 1.f, 0.01f),1.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "amp4",  1}, "amp4",
      juce::NormalisableRange<float>(0.f, 1.f, 0.01f),1.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "amp5",  1}, "amp5",
      juce::NormalisableRange<float>(0.f, 1.f, 0.01f),1.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "amp6",  1}, "amp6",
      juce::NormalisableRange<float>(0.f, 1.f, 0.01f),1.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "amp7",  1}, "amp7",
      juce::NormalisableRange<float>(0.f, 1.f, 0.01f),1.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "amp8",  1}, "amp8",
      juce::NormalisableRange<float>(0.f, 1.f, 0.01f),1.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "amp9",  1}, "amp9",
      juce::NormalisableRange<float>(0.f, 1.f, 0.01f),1.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "amp10",  1}, "amp1",
      juce::NormalisableRange<float>(0.f, 1.f, 0.01f),1.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "amp11",  1}, "amp11",
      juce::NormalisableRange<float>(0.f, 1.f, 0.01f),1.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "amp12",  1}, "amp12",
      juce::NormalisableRange<float>(0.f, 1.f, 0.01f),1.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "amp13",  1}, "amp13",
      juce::NormalisableRange<float>(0.f, 1.f, 0.01f),1.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "amp14",  1}, "amp14",
      juce::NormalisableRange<float>(0.f, 1.f, 0.01f),1.f),
      std::make_unique<juce::AudioParameterFloat>(ParameterID { "amp15",  1}, "amp15",
      juce::NormalisableRange<float>(0.f, 1.f, 0.01f),1.f),
    }
  )
{
 
  for (RNBO::ParameterIndex i = 0; i < rnboObject.getNumParameters(); ++i){
    RNBO::ParameterInfo info;
    rnboObject.getParameterInfo (i, &info);

    if (info.visible){
      auto paramID = juce::String (rnboObject.getParameterId (i));
      std::cout << "Parameter Index: " << i << std::endl;
      std::cout << "Min Value: " << info.min << std::endl;
      std::cout << "Max Value: " << info.max << std::endl;

      // Each apvts parameter id and range must be the same as the rnbo param object's.
      // If you hit this assertion then you need to fix the incorrect id in ParamIDs.h.
      jassert (parameters.getParameter (paramID) != nullptr);

      // If you hit these assertions then you need to fix the incorrect apvts
      // parameter range in createParameterLayout().
      jassert (info.min == parameters.getParameterRange (paramID).start);
      jassert (info.max == parameters.getParameterRange (paramID).end);

      apvtsParamIdToRnboParamIndex[paramID] = i;
    
      parameters.addParameterListener(paramID, this);
      rnboObject.setParameterValue(i, parameters.getRawParameterValue(paramID)->load());

    } 
  }

}

void CustomAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    rnboObject.prepareToProcess (sampleRate, static_cast<size_t> (samplesPerBlock));
    setBufferData();
}
 
void CustomAudioProcessor::releaseResources()
{
}
 


void CustomAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{ 
    juce::ignoreUnused (midiMessages);
    rnboObject.process(
                      buffer.getArrayOfWritePointers(), static_cast<RNBO::Index>(buffer.getNumChannels()),
                      buffer.getArrayOfWritePointers(), static_cast<RNBO::Index>(buffer.getNumChannels()),
                      static_cast<RNBO::Index> (buffer.getNumSamples())
     ); 
     
    updateSpectrumData(RNBObuffer.get());
}

void CustomAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
  std::cout << "Parameter Changed: " << parameterID << " New Value: " << newValue << std::endl;
  rnboObject.setParameterValue (apvtsParamIdToRnboParamIndex[parameterID], newValue);
}

juce::AudioProcessorEditor* CustomAudioProcessor::createEditor()
{
   return new CustomAudioEditor (*this,  parameters);
}

bool CustomAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}


const juce::String CustomAudioProcessor::getName() const{
    return "RNBO_SinusoidalResynthesis";
}

bool CustomAudioProcessor::acceptsMidi() const
{
    return false;
}

bool CustomAudioProcessor::producesMidi() const 
{
    return false;
}   
 
bool CustomAudioProcessor::isMidiEffect() const
{
    return false;
}

double CustomAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}
 
int CustomAudioProcessor::getNumPrograms()
{
    return 1;   
}
 
int CustomAudioProcessor::getCurrentProgram()
{
    return 0;
}
 
void CustomAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}
 
const juce::String CustomAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}
 
void CustomAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}


void CustomAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void CustomAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

void CustomAudioProcessor::setBufferData()
{
    constexpr int numBands = 15;
    constexpr int numChannels = 2;

    RNBObuffer = std::make_unique<float[]>(numBands * numChannels);

    std::fill(RNBObuffer.get(), RNBObuffer.get() + numBands * numChannels, 0.0f);

    //RNBO::Float32AudioBuffer scopeBufferType(1, this->rnboObject.getSampleRate());
    RNBO::Float32AudioBuffer bufferType(numChannels, this->rnboObject.getSampleRate());

    this->rnboObject.setExternalData(
        "ExDetectionData",
        reinterpret_cast<char*>(RNBObuffer.get()),
        numBands * numChannels * sizeof(float),
        bufferType
    );
}

void CustomAudioProcessor::updateSpectrumData(float* rnboBuffer)
{
    for (int band = 0; band < 15; ++band){
        for (int ch = 0; ch < 2; ++ch){
            spectrumData[band][ch] = rnboBuffer[ch + (2 * band)];
        }
    }
    //std::cout << "freq1: " << spectrumData[0][0] << " amp1: "  << spectrumData[0][1] << std::endl;
    //std::cout << "freq2: " << spectrumData[1][0] << " amp2: "  << spectrumData[1][1] << std::endl;

}



