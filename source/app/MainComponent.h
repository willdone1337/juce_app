#pragma once

#include <JuceHeader.h>
#include "SineWaveComponent.h"

class MainComponent : public juce::AudioAppComponent
{
public:
    MainComponent()
    {
        setSize(800, 800);

        // Add frequency slider
        addAndMakeVisible(frequencySlider);
        frequencySlider.setRange(50.0, 5000.0, 1.0);
        frequencySlider.setValue(500.0);
        frequencySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 20);
        frequencySlider.onValueChange = [this] { updateFrequency(); };

        // Add frequency label
        addAndMakeVisible(frequencyLabel);
        frequencyLabel.setText("\nFrequency (Hz)", juce::dontSendNotification);
        frequencyLabel.attachToComponent(&frequencySlider, false);

        // Add amplitude slider
        addAndMakeVisible(amplitudeSlider);
        amplitudeSlider.setRange(0.0, 3.0, .1f);
        amplitudeSlider.setValue(1.0);
        amplitudeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 20);
        amplitudeSlider.onValueChange = [this] { updateAmplitude(); };

        // Add amplitude label
        addAndMakeVisible(amplitudeLabel);
        amplitudeLabel.setText("\nAmplitude", juce::dontSendNotification);
        amplitudeLabel.attachToComponent(&amplitudeSlider, false);

        // Create and add sine wave visualization
        addAndMakeVisible(sineWaveComponent);
        
        // Start the audio
        setAudioChannels(0, 2); // no inputs, 2 outputs
    }

    ~MainComponent() override
    {
        shutdownAudio();
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    }

    void resized() override
    {
        auto area = getLocalBounds();
        frequencySlider.setBounds(area.removeFromTop(95).reduced(10));
        amplitudeSlider.setBounds(area.removeFromTop(80).reduced(10));
        sineWaveComponent.setBounds(area.removeFromTop(200).reduced(10));
    }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
    {
        currentSampleRate = sampleRate;
        updateFrequency();
        updateAmplitude();
    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        auto* leftBuffer = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
        auto* rightBuffer = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);
        for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            auto currentSample = (float)std::sin(currentAngle);
            currentAngle += angleDelta;
            leftBuffer[sample] = currentSample *  this->amplitude;
            rightBuffer[sample] = currentSample * this->amplitude;
        }
    }

    void releaseResources() override
    {
    }

private:
    void updateFrequency()
    {
        auto cyclesPerSample = frequencySlider.getValue() / currentSampleRate;
        angleDelta = cyclesPerSample;
        sineWaveComponent.setFrequency(frequencySlider.getValue());
    }

    void updateAmplitude()
    {
        amplitude = amplitudeSlider.getValue();
        sineWaveComponent.setAmplitude(amplitude);
    }

    juce::Slider frequencySlider;
    juce::Slider amplitudeSlider;
    juce::Label frequencyLabel;
    juce::Label amplitudeLabel;

    SineWaveComponent sineWaveComponent;
    
    double currentSampleRate = 0.0;
    double currentAngle = 0.0;
    double angleDelta = 0.0;
    double amplitude = .125f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};