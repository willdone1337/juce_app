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
        generateSineWave();  // Regenerate the sine wave with the new frequency
        repaint();  // Trigger a repaint after updating the frequency
    }

    void setAmplitude(double amplitude)
    {
        this->amplitude = amplitude;
        generateSineWave();  // Regenerate the sine wave with the new amplitude
        repaint();  // Trigger a repaint after updating the amplitude
    }
    // All the repaint() calls in the MainComponent class are now replaced with repaint() 
    // calls in the SineWaveComponent class and remove from the MainComponent.h.

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
        
        double phase = 0.0;
        double sampleRate = 44100.0; // Standard sample rate for audio in the Mac M3 Max
        
        // Generate the sine wave points
        for (int x = 0; x < width; ++x)
        {
            double time = x / sampleRate;
            double y = amplitude * std::sin(2.0 * juce::MathConstants<double>::pi * frequency * time);
            sineWavePath.lineTo(x, height / 2 - y * height / 2);  // Scale to fit in the window and it uses array data structure and size is constant(16).
            // release the heap memory if lineto is not used.
            // sineWavePath.clear();
        }
    }

    juce::Path sineWavePath;
    double frequency = 500.0; // Default frequency
    double amplitude = 1.0;   // Default amplitude

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SineWaveComponent)
};