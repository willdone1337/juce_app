
#pragma once

#include <JuceHeader.h>

class SineWaveComponent : public juce::Component, private juce::Timer
{
public:
    SineWaveComponent(){
        startTimerHz(25);  // 25hz
    }

    ~SineWaveComponent() override = default;

    void setFrequency(double frequency)
    {
        this->frequency = frequency;
        repaint();  // Trigger a repaint after updating the frequency
    }

    void setAmplitude(double amplitude)
    {
        this->amplitude = amplitude;
        repaint();  // Trigger a repaint after updating the amplitude
    }
    // All the repaint() calls in the MainComponent class are now replaced with repaint() 
    // calls in the SineWaveComponent class and remove from the MainComponent.h.

    void paint(juce::Graphics& g) override
    {
        juce::Path sineWavePath;
        
        g.fillAll(juce::Colours::black);  // Background color
        g.setColour(juce::Colours::green);  // Sine wave color
        generateSineWave(sineWavePath);
        g.strokePath(sineWavePath, juce::PathStrokeType(2.0f)); // It pain the filled sineWavePath
    }

    void resized() override
    {
        repaint();
    }

private:
    void generateSineWave(juce::Path& sineWavePath)
    {

        int width = getWidth();
        int height = getHeight();
        
        double phase = 0.0;
        double sampleRate = 44100.0; // Standard sample rate for audio in the Mac M3 Max
        for (int x = 0; x < width; ++x)
        {
            double time = (x + currentPhase) / sampleRate;
            double y = amplitude * std::sin(2.0 * juce::MathConstants<double>::pi * frequency * time);
            sineWavePath.lineTo(x, height / 2 - y * height / 2);  // Scale to fit in the window and it uses array data structure and size is constant(16).
        }
        // sineWavePath.clear();
    }
    void timerCallback() override
    {
        currentPhase += 1.0;  // Increment the phase
        if (std::isinf(currentPhase)){
            currentPhase = 0.0f;
        }
        repaint();  // Trigger a repaint to update the sine wave
    }
    double currentPhase = 0.0f; // X-line(time) increase value
    double frequency = 500.0; // Default frequencyg
    double amplitude = .125f;   // Default amplitude

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SineWaveComponent)
};