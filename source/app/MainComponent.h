#pragma once

#include <JuceHeader.h>
#include "SineWaveComponent.h"
#include "FFTComponent.h"

class MainComponent : public juce::AudioAppComponent
{
public:
    MainComponent()
    {
        setSize(800, 1200);

        // Add frequency slider
        // addAndMakeVisible(frequencySlider);
        // frequencySlider.setRange(50.0, 5000.0, 1.0);
        // frequencySlider.setValue(500.0);
        // frequencySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 20);
        // frequencySlider.onValueChange = [this] { updateFrequency(); };

        // Add frequency label
        // addAndMakeVisible(frequencyLabel);
        // frequencyLabel.setText("\nFrequency (Hz)", juce::dontSendNotification);
        // frequencyLabel.attachToComponent(&frequencySlider, false);

        // Add amplitude slider
        addAndMakeVisible(amplitudeSlider);
        amplitudeSlider.setRange(0.0, 10.0, .01f);
        amplitudeSlider.setValue(0.125f);
        amplitudeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 20);
        amplitudeSlider.onValueChange = [this] { updateAmplitude(); };

        // Add amplitude label
        addAndMakeVisible(amplitudeLabel);
        amplitudeLabel.setText("Amplitude", juce::dontSendNotification);
        amplitudeLabel.attachToComponent(&amplitudeSlider, false);

        // Create and add sine wave visualization
        addAndMakeVisible(sineWaveComponent);
        sineWaveLabel.setText("Sine Wave", juce::dontSendNotification);
        sineWaveLabel.attachToComponent(&sineWaveComponent, false);
        
        // Create and add FFT visualization
        addAndMakeVisible(fftComponent);
        fftLabel.setText("FFT", juce::dontSendNotification);
        fftLabel.attachToComponent(&fftComponent, false);

        audioSettingsComponent.reset(new juce::AudioDeviceSelectorComponent(
            deviceManager,
            0, 2,  // Min input channels, Max input channels
            0, 2,  // Min output channels, Max output channels
            true,  // Show audio input selector
            true,  // Show audio output selector
            false,  // Show sample rate selector
            false  // Show buffer size selector
        ));
        addAndMakeVisible(audioSettingsComponent.get());
        audioSettingsComponent->setBounds(10, 650, 200, 600);

        // Start the audio
        setAudioChannels(1, 2); // 2 input channels (e.g., microphone), 2 output channels
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
        fftComponent.setBounds(area.removeFromTop(200).reduced(10));
        // audioSettingsComponent->setBounds(area.removeFromTop(200).reduced(10));
    }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
    {
        if (sampleRate > 0)
        {
            currentSampleRate = sampleRate;
        }
        else
        {
            // Fallback to a default sample rate if not set properly
            currentSampleRate = 48000.0;  // Default to 48 kHz
            std::cerr << "Warning: Invalid sampleRate received, using default: 48000 Hz" << std::endl;
        }
        
        updateFrequency();
        updateAmplitude();
    }

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        if (currentSampleRate <= 0)
        {
            std::cerr << "Error: Sample rate is not properly initialized!" << std::endl;
            bufferToFill.clearActiveBufferRegion();
            return;
        }

        auto* leftInputBuffer = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);
        auto* rightInputBuffer = bufferToFill.buffer->getReadPointer(1, bufferToFill.startSample);
        auto* leftOutputBuffer = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
        auto* rightOutputBuffer = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

        for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            // Simple processing: amplify the input
            float processedSampleLeft = leftInputBuffer[sample] * amplitude;
            // float processedSampleRight = rightInputBuffer[sample] * amplitude;
        
            // Write processed audio to output
            leftOutputBuffer[sample] = processedSampleLeft;
            rightOutputBuffer[sample] = processedSampleLeft;

            // Optionally update visualizations
            fftComponent.set_iter_value(processedSampleLeft, sample);
            sineWaveComponent.set_iter_value(processedSampleLeft, sample);
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
    juce::Label sineWaveLabel;
    juce::Label fftLabel;
    
    SineWaveComponent sineWaveComponent;
    FFTComponent fftComponent;
    std::unique_ptr<juce::AudioDeviceSelectorComponent> audioSettingsComponent;


    double currentSampleRate = 0.0;
    double currentAngle = 0.0;
    double angleDelta = 0.0;
    double amplitude = 0.125f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};