#pragma once

#include <JuceHeader.h>

class MainComponent : public juce::AudioAppComponent
{
public:
    MainComponent()
    {
        setSize(400, 300);
        
        // Add frequency slider
        addAndMakeVisible(frequencySlider);
        frequencySlider.setRange(50.0, 5000.0, 1.0);
        frequencySlider.setValue(500.0);
        frequencySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 20);
        frequencySlider.onValueChange = [this] { updateFrequency(); };
        
        // Add frequency label
        addAndMakeVisible(frequencyLabel);
        frequencyLabel.setText("Frequency (Hz)", juce::dontSendNotification);
        frequencyLabel.attachToComponent(&frequencySlider, false);

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
        frequencySlider.setBounds(area.removeFromTop(100).reduced(10));
    }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
    {
        currentSampleRate = sampleRate;
        updateFrequency();
    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        auto level = 0.125f;
        auto* leftBuffer = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
        auto* rightBuffer = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

        for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            auto currentSample = (float)std::sin(currentAngle);
            currentAngle += angleDelta;
            leftBuffer[sample] = currentSample * level;
            rightBuffer[sample] = currentSample * level;
        }
    }

    void releaseResources() override
    {
    }

private:
    void updateFrequency()
    {
        auto cyclesPerSample = frequencySlider.getValue() / currentSampleRate;
        angleDelta = cyclesPerSample * 2.0 * juce::MathConstants<double>::pi;
    }

    juce::Slider frequencySlider;
    juce::Label frequencyLabel;
    
    double currentSampleRate = 0.0;
    double currentAngle = 0.0;
    double angleDelta = 0.0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};