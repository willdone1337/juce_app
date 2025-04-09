#pragma once

#include <JuceHeader.h>
#include <cassert>

class WaveComponent : public juce::Component, private juce::Timer
{
public:
    WaveComponent(){
        this->sampleSampleRate = 4;//static_cast<int>(this->sampleRate / this->sampleChunk); // 48000 / 2048 = 23....s
        this->sineStartIdx = 0;
        assert(this->sineWindowSize <= this->sineData.size());
        
        // this->sampleSampleRate = sampleSampleRate
        startTimerHz(25);  // 25hz
    }

    ~WaveComponent() override = default;
    
    void setSampleRate(double samplerate)
    {
        this->sampleRate = samplerate;
    }

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

    void set_iter_value(const float value)
    {
        // std::cout << this->sampleSampleRate << " SAMPLE RAAA\n";
        if ((this->globalSineDataIdx % this->sampleSampleRate) == 0)
        {
            sineData[this->sineDataIdx] = value;
            // std::cout << " HERE ? ? ?  ? " << this->sineData[this->sineDataIdx] << std::endl;
            this->sineDataIdx++;
            if (this->sineDataIdx == this->sineData.size()){
            // if (this->sineDataIdx == 0){
                // this->sineDataIdx = this->sineData.size() - 1;
                this->sineDataIdx = 0;;
            };
        }

        this->globalSineDataIdx++;
        if (this->globalSineDataIdx == this->sampleRate)
        {
            this->globalSineDataIdx = 0;
        }
        // std::cout << this->globalSineDataIdx << " GLOBAL " << this->sineDataIdx << std::endl;
    }

    void set_ftsize(size_t size)
    {
    }

private:
    void generateWave()
    {
        // we have to keep start and end point of the sine data
        // and iterate over this indexies to be sure that we are in the right
        // window for the updating the wave image 
        WavePath.clear();  // Clear the path to avoid overlapping
        int width = getWidth();    // Current width of the component
        int height = getHeight();  // Current height of the component
        double timeStep = 1.0 / this->sampleRate;  // Time step for each sample
        WavePath.startNewSubPath(0, height / 2);  // Start at the middle of the height
        // for (int x = 0; x < width; ++x)  // Iterate over the width of the component
        int last_start_val = this->sineStartIdx;
        // for (int x = width - 1; x >= 0; --x)  // Iterate over the width of the component
        // {
        //     double y = sineData[x % sineData.size()] * amplitude;  // Scale amplitude and use sineData
        //     WavePath.lineTo(x, height / 2 - y * (height / 2));  // Map y to the component's height
        // }
        
        // for (int x = this->sineStartIdx; x <= (this->sineStartIdx + this->sineWindowSize); ++x)
        for (int x = 0; x < width; ++x)
        {
            // double y = sineData[x % sineData.size()] * amplitude;  // Scale amplitude and use sineData
            double y = sineData[this->sineStartIdx % sineData.size()] * amplitude;  // Scale amplitude and use sineData
            WavePath.lineTo(x, height / 2 - y * (height / 2));  // Map y to the component's height
            this->sineStartIdx = (this->sineStartIdx + 1) % sineData.size();
        }
        this->sineStartIdx = (this->sineStartIdx + this->sineStepSize) % sineData.size();
    }
    
    void timerCallback() override
    {
        currentPhase += 1.0;  // Increment the phase
        if (std::isinf(currentPhase)){
            currentPhase = 0.0f;
        } 
        repaint();  // Trigger a repaint to update the sine wave
    }
    
    std::vector<float> sineData = std::vector<float>(1 << 11, 0.0f);  
    juce::Path WavePath;
    double currentPhase = 0.0f; // X-line(time) increase value
    double frequency = 500.0; // Default frequencyg
    double amplitude = .125f;   // Default amplitude
    int sineDataIdx = 0;//(1 << 20) - 1; //slice for the sine data filling
    int globalSineDataIdx = 0;
    bool paintSineData = false;
    double sampleRate = 48000.0f;
    double sampleChunk = 1024.0f;
    int sampleSampleRate;
    int sineStepSize = 4;
    int sineWindowSize = 512;
    int sineStartIdx;
    // JUCE_DECLARE_NO N_COPYABLE_WITH_LEAK_DETECTOR(WaveComponent);
};

// With Spectogram
// PID   COMMAND      %CPU TIME     #TH  #WQ  #POR MEM   PURG CMPR PGRP PPID STATE    BOOSTS     %CPU_ME %CPU_OTHRS UID  FAULTS COW  MSGSEN MSGRE SYSBSD SYSMAC CSW    PAGE IDLE POWE INSTRS    CYCLES    JETP
// 6256  Audio App    15.1 00:03.38 13   3    414  356M+ 13M  0B   6254 6254 sleeping *0[10]     0.77518 0.00000    501  10875+ 339  25418+ 5835+ 30768+ 66075+ 32187+ 6    2+   15.1 265128731 346503119 180


// Without Spectogram
// PID   COMMAND      %CPU TIME     #TH  #WQ  #POR MEM  PURG CMPR PGRP PPID STATE    BOOSTS     %CPU_ME %CPU_OTHRS UID  FAUL COW  MSGSE MSGRE SYSBSD SYSMAC CSW    PAGE IDLE POWE INSTRS    CYCLES    JETP USER
// 6991  Audio App    6.8  00:01.02 13   3    415  471M 13M  0B   6990 6990 sleeping *0[13]     0.41167 0.00000    501  8074 332  10024 1640+ 11808+ 29717+ 12981+ 2    2+   6.8  121816648 160003631 180  vildan
