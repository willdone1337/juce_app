#pragma once

#include <JuceHeader.h>

class WaveComponent : public juce::Component, private juce::Timer
{
public:
    WaveComponent(){
        startTimerHz(30);  // 25hz
    }

    ~WaveComponent() override = default;

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
    // calls in the WaveComponent class and remove from the MainComponent.h.

    void paint(juce::Graphics& g) override
    {
        // juce::Path WavePath;
        
        g.fillAll(juce::Colours::black);  
        g.setColour(juce::Colours::green);
        generateWave();
        g.strokePath(WavePath, juce::PathStrokeType(2.0f)); // It pain the filled WavePath
    }

    void resized() override
    {
        repaint();
    }

    void set_iter_value(const float value, const int index)
    {
        sineData[index] = value;
    }

    void set_ftsize(size_t size)
    {
    }

private:
    void generateWave()
    {
        WavePath.clear();  // Clear the path to avoid overlapping
        int width = getWidth();    // Current width of the component
        int height = getHeight();  // Current height of the component
        double sampleRate = 48100.0;  // Standard sample rate for audio
        double timeStep = 1.0 / sampleRate;  // Time step for each sample
        WavePath.startNewSubPath(0, height / 2);  // Start at the middle of the height
        for (int x = 0; x < width; ++x)  // Iterate over the width of the component
        {
            double y = sineData[x % sineData.size()] * amplitude;  // Scale amplitude and use sineData
            WavePath.lineTo(x, height / 2 - y * (height / 2));  // Map y to the component's height
        }
    }
    
    void timerCallback() override
    {
        currentPhase += 1.0;  // Increment the phase
        if (std::isinf(currentPhase)){
            currentPhase = 0.0f;
        }
        repaint();  // Trigger a repaint to update the sine wave
    }
    
    std::vector<float> sineData = std::vector<float>(1 << 9, 0.0f);  
    juce::Path WavePath;
    double currentPhase = 0.0f; // X-line(time) increase value
    double frequency = 500.0; // Default frequencyg
    double amplitude = .125f;   // Default amplitude

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveComponent)
};

// With Spectogram
// PID   COMMAND      %CPU TIME     #TH  #WQ  #POR MEM   PURG CMPR PGRP PPID STATE    BOOSTS     %CPU_ME %CPU_OTHRS UID  FAULTS COW  MSGSEN MSGRE SYSBSD SYSMAC CSW    PAGE IDLE POWE INSTRS    CYCLES    JETP
// 6256  Audio App    15.1 00:03.38 13   3    414  356M+ 13M  0B   6254 6254 sleeping *0[10]     0.77518 0.00000    501  10875+ 339  25418+ 5835+ 30768+ 66075+ 32187+ 6    2+   15.1 265128731 346503119 180


// Without Spectogram
// PID   COMMAND      %CPU TIME     #TH  #WQ  #POR MEM  PURG CMPR PGRP PPID STATE    BOOSTS     %CPU_ME %CPU_OTHRS UID  FAUL COW  MSGSE MSGRE SYSBSD SYSMAC CSW    PAGE IDLE POWE INSTRS    CYCLES    JETP USER
// 6991  Audio App    6.8  00:01.02 13   3    415  471M 13M  0B   6990 6990 sleeping *0[13]     0.41167 0.00000    501  8074 332  10024 1640+ 11808+ 29717+ 12981+ 2    2+   6.8  121816648 160003631 180  vildan
