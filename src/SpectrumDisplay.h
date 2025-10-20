// SpectrumDisplay.h
#pragma once
#include <JuceHeader.h>

class SpectrumDisplay : public juce::AnimatedAppComponent
{
public:
    SpectrumDisplay(const float* dataPtr, const float RNBOsampleRate)
        : spectrumData(dataPtr), sampleRate(RNBOsampleRate)
    {
        setFramesPerSecond (30);
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);

        if (spectrumData == nullptr) return;

        // Drawing area size
        const float width = (float)getWidth();
        const float height = (float)getHeight();

        const float minFreq = 20.0f;                   
        const float maxFreq = sampleRate / 2.0f;   

        const float minDB = -60.0f;
        const float maxDB = 0.0f;

        

        g.setColour(juce::Colours::darkgreen);

        for (int band = 0; band < numBands; band += 2)
        {
            const float freq = juce::jlimit(0.0f, sampleRate / 2, spectrumData[band]); // 0 〜 NyquistFreq
            //std::cout << "freq: " << freq << " band: " << band <<  std::endl;
    

            // Logarithmic scaling for frequency axis
            const float normalizedFreq = (std::log(freq / minFreq)) / (std::log(maxFreq / minFreq));
            const float x = normalizedFreq * width;


            const float amp = juce::jlimit(0.0f, 1.0f, spectrumData[band + 1]);

            float dB = 20.0f * std::log10(juce::jmax(amp, 0.0001f));
            
            //Logarithmic scaling for amplitude axis
            float normalizedAmp = juce::jlimit(0.0f, 1.0f, (dB - minDB) / (maxDB - minDB));

            //std::cout << "amp: " << amp << " band: " << band <<  std::endl;

            const float barHeight = normalizedAmp * height;
            const float y = height - barHeight;

            //std::cout << spectrumData[band] << " spectrumData[band] " <<  band <<  std::endl;

            g.fillRect(x, y, width / numBands * 0.1f, barHeight);
        }

 
       
    }

    void setDataPointer(const float* newData) { spectrumData = newData; }

    void update() override
    {
        // Here you would typically read from spectrumData and update the display accordingly.
        // For demonstration, we'll just repaint.
        repaint();
    }   

private:
    const float* spectrumData = nullptr; // Pointer to the spectrum data array
    const float sampleRate;
    int numBands = 15;
};
