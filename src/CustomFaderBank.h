#pragma once
#include <JuceHeader.h>

class CustomFaderBank : public juce::Component
{
public:
    static constexpr int numBands = 15;

    CustomFaderBank(juce::AudioProcessorValueTreeState& vts, const juce::String& paramBaseID)
        : valueTreeState (vts), baseID (paramBaseID)
    {
        for (int i = 0; i < numBands; ++i){
            auto id = baseID + juce::String(i + 1);
            bandValues[i] = *valueTreeState.getRawParameterValue (id);

        }
        setInterceptsMouseClicks(true, true);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::darkgrey);
        auto area = getLocalBounds();
        auto barWidth = (int)area.getWidth() / numBands;

        for (int i = 0; i < numBands; ++i)
        {
            int x = i * barWidth;
            int height = static_cast<int>(area.getHeight() * bandValues[i]);
            int y = area.getBottom() - height;

            // background
            g.setColour(juce::Colours::white);
            g.fillRect(x, 0, barWidth, area.getHeight());

            // filled bar
            g.setColour(juce::Colours::darkgreen.withAlpha(0.75f));
            g.fillRect(x, y, barWidth, height);

            // grid-line
            g.setColour(juce::Colours::black);
            g.drawRect(x, 0, barWidth, area.getHeight(), 1);

        }
    }

    void mouseDown(const juce::MouseEvent& e) override
    {
        isDragging = true;
        updateHoveredBar(e.position);
    }

    void mouseDrag(const juce::MouseEvent& e) override
    {
        if (isDragging)
            updateHoveredBar(e.position);
    }

    void mouseUp(const juce::MouseEvent&) override
    {
        isDragging = false;
    }

    // 外部からバンド値にアクセス可能に
    float getValue(int index) const { return bandValues[index]; }
    
    void setValue(int index, float newValue)
    {
        if (index >= 0 && index < numBands)
        {
            bandValues[index] = juce::jlimit(0.0f, 1.0f, newValue);
            repaint();
        }
    }

private:
    juce::AudioProcessorValueTreeState& valueTreeState;
    const juce::String baseID;
    float bandValues[numBands];
    bool isDragging = false;

    void updateHoveredBar(juce::Point<float> pos)
    {
        auto area = getLocalBounds();
        auto barWidth = (float)area.getWidth() / numBands;

        int index = (int)(pos.x / barWidth);
        if (index >= 0 && index < numBands)
        {
            float value = juce::jlimit(0.0f, 1.0f, 1.0f - (pos.y / (float)area.getHeight()));

            auto* param = valueTreeState.getParameter (baseID + juce::String(index + 1));
            if (param)
            {
                param->beginChangeGesture();
                param->setValueNotifyingHost (value);
                param->endChangeGesture();
            }
            bandValues[index] = value;
            repaint();
        }
    }
};
