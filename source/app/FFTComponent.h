#pragma once

#include <JuceHeader.h>
#include <vector>
#include <random>

class FFTComponent : public juce::Component, private juce::Timer
{
public:
    FFTComponent()
        : forwardFFT(fftOrder), window(fftSize, juce::dsp::WindowingFunction<float>::hann)
    {
        setOpaque(true); 
        startTimerHz(30);  // Update at 30Hz. It is enough
        std::fill(fftData.begin(), fftData.end(), 0.0f);// 
        std::cout << "FFT Order: " << fftOrder << ", FFT Size: " << fftSize << std::endl;
        std::cout << "fftData size: " << fftData.size() << std::endl;
    }

    ~FFTComponent() override = default;

    void set_iter_value(float value, int index)
    {
        fftData[index] = value;
    }

    void setAudioData(int numSamples)
    {
        if (numSamples > fftSize)
            numSamples = fftSize;
        
        window.multiplyWithWindowingTable(fftData.data(), fftSize);
        forwardFFT.performFrequencyOnlyForwardTransform(fftData.data());
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);

        setAudioData(fftSize);
        g.setColour(juce::Colours::green);
        auto width = getLocalBounds().getWidth();
        auto height = getLocalBounds().getHeight();
        auto binWidth = width / (float)fftSize;

        for (int i = 0; i < fftSize / 2; ++i)
        {
            auto magnitude = juce::jmap(
                juce::Decibels::gainToDecibels(fftData[i], -100.0f), 
                -100.0f, 0.0f, 0.0f, (float)height);
            g.drawLine((float)i * binWidth, height, (float)i * binWidth, height - magnitude);
        }
    }

    void resized() override
    {
        repaint();
    }

private:
    void timerCallback() override
    {
        repaint();
    }
    static constexpr int fftOrder = 9;  // Adjustable FFT order
    static constexpr int fftSize = 1 << fftOrder;  // FFT size = 2^fftOrder and our array in leftBuffer is 512 and they are fftOrder is depends on it
    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;
    std::vector<float> fftData = std::vector<float>(1 << fftOrder, 0.0f);  
    // I use vector instead of array because it became less painfull rather than array
    // array usage leads memory error when setting order in forwardFFT as 9
    // however in the vector this error does not appear
    // actually do not know what happens but suggest somehting related to dynamic memory allocation ????))))
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FFTComponent)
};