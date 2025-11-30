# RNBO_SinusoidalResynthesis
Sinusoidal Resynthesis System by Rnbo (Max Msp)

The input signal is analyzed using FFT, and the frequency data with the largest amplitude is estimated in order, and they are assigned to the frequency and amplitude of individual sine waves.


I have also arranged the code to use the [AudioProcessorValueTreeState](https://docs.juce.com/master/classAudioProcessorValueTreeState.html).
Built from the [rnbo.example.juce](https://github.com/Cycling74/rnbo.example.juce).  

For *Getting Started*, please refer to the original repository. Also included in this repository is the C++ program output from RNBO in `export/`. And Max patch is included in `patches/`.

## Demonstration
[Youtube<img width="1206" height="690" alt="Screenshot 2025-10-21 at 11 20 53" src="https://github.com/user-attachments/assets/5c61d85f-191c-4717-b473-e1d191705f2e" />](https://youtu.be/dINIlqqssl8)

## UI and parameter explanation
- The graph shows the top 15 estimated peak frequencies.
- **15 sliders** : The amplitude of each of the 15 sine waves can be individually controlled. It looks like a graphic equalizer.
- **dryWet** : You can mix the input signal with the sinusoidal rsynthesis sound.
- **binSmooth** : When detecting the input signal using FFT, it smooths the detected values ​​of each bin in the frame domain (time domain). Same function as [vectral~](https://docs.cycling74.com/legacy/max5/refpages/msp-ref/vectral~.html).
- **freqSmooth** : In addition to **binSmooth**, the detected peak frequencies are smoothed in the time domain. (Low values ​​may cause click noise.)
- **ampSmooth** : As with freqSmooth, the amplitude values ​​corresponding to the detected peak frequencies are smoothed in the time domain.
- **freqScale** : You can scale the frequency domain.
- **modFreq** : You can apply frequency modulation to individual sine waves. modFreq control the modulator frequency.
- **modAmp** : You can control the amplitude of the modulator.
- **stereoMode** : **Odd and even columns** is a mode where the first sine wave, second sine wave, and so on are assigned alternately to the left and right channels. "modulation" is a mode that performs panning modulation on the left and right, but it is performed by a modulator with 15 individual delay times, so it can produce a smooth sound.
- **modPanFreq** : This parameter is only available when **stereoMode** is **modulation**. It controls the frequency of the panning modulator.
- **equalTemperamentMode** : this mode maps each of the 15 input frequencies to the nearest pitch in an N-tone equal temperament scale. Both the reference frequency (e.g., A4 = 440 Hz) and the number of divisions per octave (N) can be freely specified. This mode takes each input frequency 
𝑓 and converts it to the closest equal-tempered frequency defined by those parameters.
- **equalTemperament** : You can specify the number of divisions in the equal temperament.
- **tuning** : You can specify the equal temperament reference frequency.

### Reverb parameters
The reverb effect used in this system is [RNBO_SchroederReverb](https://github.com/yuki-sato-0402/RNBO_SchroederReverb). Please check it out.

## Reference
- [lulu](https://github.com/xinisnot/lulu)
- [jr-granular](https://github.com/szkkng/jr-granular)  
- [Tutorial:Saving and loading your plug-in state](https://juce.com/tutorials/tutorial_audio_processor_value_tree_state/)

