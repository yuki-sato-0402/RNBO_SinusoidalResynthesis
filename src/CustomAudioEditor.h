#include "JuceHeader.h"
#include "RNBO.h"
#include "CustomFaderBank.h"
#include "SpectrumDisplay.h"

class CustomAudioEditor : public juce::AudioProcessorEditor
{
public:
    CustomAudioEditor(CustomAudioProcessor& p, juce::AudioProcessorValueTreeState& vts);
    ~CustomAudioEditor() override = default;
    void paint (Graphics& g) override;
    void resized() override; 
    typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

private:
    juce::AudioProcessorValueTreeState& valueTreeState; // ✅ Hold by reference
    CustomAudioProcessor& audioProcessor; // ✅ Hold by reference
    juce::Slider dryWetSlider;
    juce::Slider binSmoothSlider;
    juce::Slider freqSmoothSlider;
    juce::Slider ampSmoothSlider;
    juce::Slider freqScaleSlider;
    juce::Slider modFreqSlider;
    juce::Slider modAmpSlider;
    juce::Slider modPanFreqSlider;

    //reverb
    juce::Slider mixSlider;
    juce::Slider delayAllSlider;
    juce::Slider delayComSlider;

    juce::ComboBox stereoModeBox;

    juce::Label  dryWetLabel;
    juce::Label  binSmoothLabel;
    juce::Label  freqSmoothLabel;
    juce::Label  ampSmoothLabel;
    juce::Label  freqScaleLabel;
    juce::Label  modFreqLabel;
    juce::Label  modAmpLabel;
    juce::Label  stereoModeLabel;
    juce::Label  modPanFreqLabel;
    juce::Label  reverbLabel;
    juce::Label  mixLabel;
    juce::Label  delayAllLabel;
    juce::Label  delayComLabel;

    std::unique_ptr<SliderAttachment> ampSliderAttachments[15];
    std::unique_ptr<SliderAttachment> dial1Attachment;
    std::unique_ptr<SliderAttachment> dial2Attachment;
    std::unique_ptr<SliderAttachment> dial3Attachment;
    std::unique_ptr<SliderAttachment> dial4Attachment;
    std::unique_ptr<SliderAttachment> dial5Attachment;
    std::unique_ptr<SliderAttachment> dial6Attachment;
    std::unique_ptr<SliderAttachment> dial7Attachment;
    std::unique_ptr<SliderAttachment> dial8Attachment;
    std::unique_ptr<SliderAttachment> dial9Attachment;
    std::unique_ptr<SliderAttachment> dial10Attachment;
    std::unique_ptr<SliderAttachment> dial11Attachment;

    CustomFaderBank faderBank;
    bool isDragging = false;

    SpectrumDisplay spectrumDisplay;
 


    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> comboBoxAttachment;


    int padding; 
    int componentWidth1;
    int componentWidth2;
    int componentHeight;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (CustomAudioEditor)
};
