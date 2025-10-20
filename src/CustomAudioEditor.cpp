#include "CustomAudioProcessor.h"
#include "CustomAudioEditor.h"

CustomAudioEditor::CustomAudioEditor (CustomAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), valueTreeState(vts), audioProcessor(p), faderBank (vts, "amp"), spectrumDisplay (p.getSpectrumPointer(), p.getSampleRate())
{

    addAndMakeVisible(spectrumDisplay);
    addAndMakeVisible(faderBank);

    addAndMakeVisible(dryWetSlider);
    dial1Attachment.reset (new SliderAttachment (valueTreeState, "dryWet", dryWetSlider));
    dryWetSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    dryWetSlider.setTextValueSuffix (" %");     
    dryWetSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, dryWetSlider.getTextBoxWidth(), dryWetSlider.getTextBoxHeight());
    dryWetSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    dryWetSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkgreen.withAlpha(0.75f));
    dryWetSlider.setColour(juce::Slider::thumbColourId , juce::Colours::darkgreen.brighter(1.5));
    dryWetSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    dryWetSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);

    addAndMakeVisible(dryWetLabel);
    dryWetLabel.setText ("dryWet", juce::dontSendNotification);
    dryWetLabel.setJustificationType(juce::Justification::centred);
    
    addAndMakeVisible(binSmoothSlider);
    dial2Attachment.reset (new SliderAttachment (valueTreeState, "binSmooth", binSmoothSlider));
    binSmoothSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    binSmoothSlider.setTextValueSuffix (" ");     
    binSmoothSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, binSmoothSlider.getTextBoxWidth(), binSmoothSlider.getTextBoxHeight());
    binSmoothSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    binSmoothSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkgreen.withAlpha(0.75f));
    binSmoothSlider.setColour(juce::Slider::thumbColourId , juce::Colours::darkgreen.brighter(1.5));
    binSmoothSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    binSmoothSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);

    addAndMakeVisible(binSmoothLabel);
    binSmoothLabel.setText ("binSmooth", juce::dontSendNotification);
    binSmoothLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(freqSmoothSlider);
    dial3Attachment.reset (new SliderAttachment (valueTreeState, "freqSmooth", freqSmoothSlider));
    freqSmoothSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    freqSmoothSlider.setTextValueSuffix (" ");     
    freqSmoothSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, freqSmoothSlider.getTextBoxWidth(), freqSmoothSlider.getTextBoxHeight());
    freqSmoothSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    freqSmoothSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkgreen.withAlpha(0.75f));
    freqSmoothSlider.setColour(juce::Slider::thumbColourId , juce::Colours::darkgreen.brighter(1.5));
    freqSmoothSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    freqSmoothSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);    

    addAndMakeVisible(freqSmoothLabel);
    freqSmoothLabel.setText ("freqSmooth", juce::dontSendNotification);
    freqSmoothLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(ampSmoothSlider);
    dial4Attachment.reset (new SliderAttachment (valueTreeState, "ampSmooth", ampSmoothSlider));
    ampSmoothSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    ampSmoothSlider.setTextValueSuffix (" ");     
    ampSmoothSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, ampSmoothSlider.getTextBoxWidth(), ampSmoothSlider.getTextBoxHeight());
    ampSmoothSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    ampSmoothSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkgreen.withAlpha(0.75f));
    ampSmoothSlider.setColour(juce::Slider::thumbColourId , juce::Colours::darkgreen.brighter(1.5));
    ampSmoothSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    ampSmoothSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);
    addAndMakeVisible(ampSmoothLabel);
    ampSmoothLabel.setText ("ampSmooth", juce::dontSendNotification);
    ampSmoothLabel.setJustificationType(juce::Justification::centred);  

    addAndMakeVisible(freqScaleSlider);
    dial5Attachment.reset (new SliderAttachment (valueTreeState, "freqScale", freqScaleSlider));
    freqScaleSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);   
    freqScaleSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, freqScaleSlider.getTextBoxWidth(), freqScaleSlider.getTextBoxHeight());
    freqScaleSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    freqScaleSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkgreen.withAlpha(0.75f));
    freqScaleSlider.setColour(juce::Slider::thumbColourId , juce::Colours::darkgreen.brighter(1.5));
    freqScaleSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    freqScaleSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);

    addAndMakeVisible(freqScaleLabel);
    freqScaleLabel.setText ("freqScale", juce::dontSendNotification);
    freqScaleLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(modFreqSlider);
    dial6Attachment.reset (new SliderAttachment (valueTreeState, "modFreq", modFreqSlider));
    modFreqSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    modFreqSlider.setTextValueSuffix (" Hz");     
    modFreqSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, modFreqSlider.getTextBoxWidth(), modFreqSlider.getTextBoxHeight());
    modFreqSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    modFreqSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkgreen.withAlpha(0.75f));
    modFreqSlider.setColour(juce::Slider::thumbColourId , juce::Colours::darkgreen.brighter(1.5));
    modFreqSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    modFreqSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);

    addAndMakeVisible(modFreqLabel);
    modFreqLabel.setText ("modFreq", juce::dontSendNotification);
    modFreqLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(modAmpSlider);
    dial7Attachment.reset (new SliderAttachment (valueTreeState, "modAmp", modAmpSlider));
    modAmpSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);    
    modAmpSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, modAmpSlider.getTextBoxWidth(), modAmpSlider.getTextBoxHeight());
    modAmpSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    modAmpSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkgreen.withAlpha(0.75f));
    modAmpSlider.setColour(juce::Slider::thumbColourId , juce::Colours::darkgreen.brighter(1.5));
    modAmpSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    modAmpSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);    

    addAndMakeVisible(modAmpLabel);
    modAmpLabel.setText ("modAmp", juce::dontSendNotification);
    modAmpLabel.setJustificationType(juce::Justification::centred);     

    addAndMakeVisible(stereoModeBox);
    stereoModeBox.addItem("Odd and even columns", 1);
    stereoModeBox.addItem("modulation", 2);
    stereoModeBox.setSelectedId(1);
    comboBoxAttachment.reset (new juce::AudioProcessorValueTreeState::ComboBoxAttachment (valueTreeState, "stereoMode", stereoModeBox));
    stereoModeBox.setColour(juce::ComboBox::backgroundColourId, juce::Colours::darkgreen.withAlpha(0.75f));
    stereoModeBox.setColour(juce::ComboBox::textColourId, juce::Colours::white);
    stereoModeBox.setColour(juce::ComboBox::outlineColourId, juce::Colours::white);
    stereoModeBox.setColour(juce::ComboBox::arrowColourId, juce::Colours::white);

    addAndMakeVisible(stereoModeLabel);
    stereoModeLabel.setText ("stereoMode", juce::dontSendNotification);
    stereoModeLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(modPanFreqSlider);
    dial8Attachment.reset (new SliderAttachment (valueTreeState, "modPanFreq", modPanFreqSlider));
    modPanFreqSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    modPanFreqSlider.setTextValueSuffix (" Hz");     
    modPanFreqSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, modPanFreqSlider.getTextBoxWidth(), modPanFreqSlider.getTextBoxHeight());
    modPanFreqSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    modPanFreqSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkgreen.withAlpha(0.75f));
    modPanFreqSlider.setColour(juce::Slider::thumbColourId , juce::Colours::darkgreen.brighter(1.5));
    modPanFreqSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    modPanFreqSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);    

    addAndMakeVisible(modPanFreqLabel);
    modPanFreqLabel.setText ("modPanFreq", juce::dontSendNotification);
    modPanFreqLabel.setJustificationType(juce::Justification::centred); 

    addAndMakeVisible(reverbLabel);
    reverbLabel.setText ("reverb", juce::dontSendNotification);
    reverbLabel.setJustificationType(juce::Justification::centred); 

    addAndMakeVisible(mixSlider);
    dial9Attachment.reset (new SliderAttachment (valueTreeState, "mix", mixSlider));
    mixSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    mixSlider.setTextValueSuffix (" %");     
    mixSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, mixSlider.getTextBoxWidth(), mixSlider.getTextBoxHeight());
    mixSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    mixSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkgreen.withAlpha(0.75f));
    mixSlider.setColour(juce::Slider::thumbColourId , juce::Colours::darkgreen.brighter(1.5));
    mixSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    mixSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);   

    addAndMakeVisible(mixLabel);
    mixLabel.setText ("mix", juce::dontSendNotification);
    mixLabel.setJustificationType(juce::Justification::centred);    

    addAndMakeVisible(delayAllSlider);
    dial10Attachment.reset (new SliderAttachment (valueTreeState, "delayAll", delayAllSlider));
    delayAllSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    delayAllSlider.setTextValueSuffix (" ms");     
    delayAllSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, delayAllSlider.getTextBoxWidth(), delayAllSlider.getTextBoxHeight());
    delayAllSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    delayAllSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkgreen.withAlpha(0.75f));
    delayAllSlider.setColour(juce::Slider::thumbColourId , juce::Colours::darkgreen.brighter(1.5));
    delayAllSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    delayAllSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);

    addAndMakeVisible(delayAllLabel);
    delayAllLabel.setText ("delayAll", juce::dontSendNotification);
    delayAllLabel.setJustificationType(juce::Justification::centred);   

    addAndMakeVisible(delayComSlider);
    dial11Attachment.reset (new SliderAttachment (valueTreeState, "delayCom", delayComSlider));
    delayComSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    delayComSlider.setTextValueSuffix (" ms");     
    delayComSlider.setTextBoxStyle (juce::Slider::TextBoxBelow, false, delayComSlider.getTextBoxWidth(), delayComSlider.getTextBoxHeight());
    delayComSlider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::white);
    delayComSlider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkgreen.withAlpha(0.75f));
    delayComSlider.setColour(juce::Slider::thumbColourId , juce::Colours::darkgreen.brighter(1.5));
    delayComSlider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
    delayComSlider.setColour(juce::Slider::textBoxOutlineColourId , juce::Colours::white);  

    addAndMakeVisible(delayComLabel);
    delayComLabel.setText ("delayCom", juce::dontSendNotification);
    delayComLabel.setJustificationType(juce::Justification::centred);   
    
    setSize(900, 600);
}

void CustomAudioEditor::paint (Graphics& g)
{
    g.fillAll(juce::Colours::darkgreen); 
    auto area = getLocalBounds();
    g.setColour(juce::Colours::white);
    g.drawRect(static_cast<int>(padding * 3.5) + (componentWidth2 * 3), (area.getHeight() / 4) * 3 - 10, 
              (componentWidth2 * 4) + (padding * 4), componentHeight + padding + 10, 2);

}

void CustomAudioEditor::resized()
{
  //  const int componentWidth2Diff = componentWidth2 / 2;

    auto area = getLocalBounds();
    padding = 20; 
    componentWidth1 = (area.getWidth() - 40) / 15;
    componentWidth2 = (area.getWidth() - 160) / 7; 
    componentHeight = (area.getHeight() - 100) / 4;

    //for (int i = 0; i < numSliders; ++i){
    //    ampSliders[i].setBounds(padding + (i * componentWidth1), 2 * padding + componentHeight, componentWidth1, componentHeight);
    //}
    spectrumDisplay.setBounds(padding, padding, area.getWidth() - 40, componentHeight);
    faderBank.setBounds(padding, spectrumDisplay.getBottom() + padding, area.getWidth() - 40, componentHeight);

    dryWetSlider.setBounds(padding, faderBank.getBottom() + padding,  componentWidth2 , componentHeight);
    binSmoothSlider.setBounds(dryWetSlider.getRight() + padding,  faderBank.getBottom() + padding,  componentWidth2 , componentHeight);
    freqSmoothSlider.setBounds(binSmoothSlider.getRight() + padding,  faderBank.getBottom() + padding,  componentWidth2 , componentHeight);  
    ampSmoothSlider.setBounds(freqSmoothSlider.getRight() + padding,  faderBank.getBottom() + padding,  componentWidth2 , componentHeight);
    freqScaleSlider.setBounds(ampSmoothSlider.getRight() + padding,  faderBank.getBottom() + padding,  componentWidth2 , componentHeight);
    modFreqSlider.setBounds(freqScaleSlider.getRight()+ padding, faderBank.getBottom() + padding,  componentWidth2 , componentHeight);
    modAmpSlider.setBounds(modFreqSlider.getRight() + padding, faderBank.getBottom() + padding,  componentWidth2 , componentHeight);

    stereoModeBox.setBounds(padding, dryWetSlider.getBottom() + padding + 40 ,  componentWidth2 , componentHeight / 4);
    modPanFreqSlider.setBounds(stereoModeBox.getRight() + padding, dryWetSlider.getBottom() + padding,  componentWidth2 , componentHeight);
    reverbLabel.setBounds(modPanFreqSlider.getRight()  + stereoModeBox.getRight() + padding, dryWetSlider.getBottom() + padding, componentWidth2, componentHeight);
    mixSlider.setBounds(reverbLabel.getRight() + padding, dryWetSlider.getBottom() + padding,  componentWidth2 , componentHeight);
    delayAllSlider.setBounds(mixSlider.getRight() + padding, dryWetSlider.getBottom() + padding,  componentWidth2 , componentHeight);
    delayComSlider.setBounds(delayAllSlider.getRight() + padding, dryWetSlider.getBottom() + padding,  componentWidth2 , componentHeight);
  

    dryWetLabel.setBounds(dryWetSlider.getX(), dryWetSlider.getY() - 10, dryWetSlider.getWidth(), dryWetSlider.getTextBoxHeight() );
    binSmoothLabel.setBounds(binSmoothSlider.getX(), binSmoothSlider.getY()- 10, binSmoothSlider.getWidth(), binSmoothSlider.getTextBoxHeight() );
    freqSmoothLabel.setBounds(freqSmoothSlider.getX(), freqSmoothSlider.getY()- 10, freqSmoothSlider.getWidth(), freqSmoothSlider.getTextBoxHeight() );
    ampSmoothLabel.setBounds(ampSmoothSlider.getX(), ampSmoothSlider.getY()- 10, ampSmoothSlider.getWidth(), ampSmoothSlider.getTextBoxHeight() );
    freqScaleLabel.setBounds(freqScaleSlider.getX(), freqScaleSlider.getY()- 10, freqScaleSlider.getWidth(), freqScaleSlider.getTextBoxHeight() );  
    modFreqLabel.setBounds(modFreqSlider.getX(), modFreqSlider.getY()- 10, modFreqSlider.getWidth(), modFreqSlider.getTextBoxHeight() );
    modAmpLabel.setBounds(modAmpSlider.getX(), modAmpSlider.getY()- 10, modAmpSlider.getWidth(), modAmpSlider.getTextBoxHeight() );
    stereoModeLabel.setBounds(stereoModeBox.getX(), stereoModeBox.getY()- 40, stereoModeBox.getWidth(), stereoModeBox.getHeight() );
    modPanFreqLabel.setBounds(modPanFreqSlider.getX(), modPanFreqSlider.getY()- 10, modPanFreqSlider.getWidth(), modPanFreqSlider.getTextBoxHeight() );  
    mixLabel.setBounds(mixSlider.getX(), mixSlider.getY()- 10, mixSlider.getWidth(), mixSlider.getTextBoxHeight() );
    delayAllLabel.setBounds(delayAllSlider.getX(), delayAllSlider.getY()- 10, delayAllSlider.getWidth(), delayAllSlider.getTextBoxHeight() );
    delayComLabel.setBounds(delayComSlider.getX(), delayComSlider.getY()- 10, delayComSlider.getWidth(), delayComSlider.getTextBoxHeight() );   
}