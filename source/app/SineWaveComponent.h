#pragma once

#include <JuceHeader.h>

class SineWaveComponent : public juce::Component
{
public:
    SineWaveComponent() = default;
    ~SineWaveComponent() override = default;

    void setFrequency(double frequency)
    {
        this->frequency = frequency;
        repaint();
    }

    void setAmplitude(double amplitude)
    {
        this->amplitude = amplitude;
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::black);  // Background color
        
        g.setColour(juce::Colours::green);  // Sine wave color
        g.strokePath(sineWavePath, juce::PathStrokeType(2.0f));
    }

    void resized() override
    {
        generateSineWave();
    }

private:
    void generateSineWave()
    {
        sineWavePath.clear();

        int width = getWidth();
        int height = getHeight();
        
        // Calculate the sine wave data based on frequency and amplitude
        double phase = 0.0;
        double sampleRate = 44100.0; // Standard sample rate
        
        // Generate the sine wave points
        for (int x = 0; x < width; ++x)
        {
            double time = x / sampleRate;
            double y = amplitude * std::sin(2.0 * juce::MathConstants<double>::pi * frequency * time);
            sineWavePath.lineTo(x, height / 2 - y * height / 2);  // Scale to fit in the window
        }
    }

    juce::Path sineWavePath;
    double frequency = 500.0; // Default frequency
    double amplitude = 1.0;   // Default amplitude

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SineWaveComponent)
};