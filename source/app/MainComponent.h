#pragma once

#include <JuceHeader.h>
#include "WaveComponent.h"
#include "SpectogramComponent.h"

class MainComponent : public juce::AudioAppComponent
{
public:
    MainComponent()
    {
        setSize(800, 1200);
        
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
        addAndMakeVisible(WaveComponent);
        sineWaveLabel.setText("Sine Wave", juce::dontSendNotification);
        sineWaveLabel.attachToComponent(&WaveComponent, false);
        
        // Create and add FFT visualization
        addAndMakeVisible(fftComponent);
        fftLabel.setText("FFT", juce::dontSendNotification);
        fftLabel.attachToComponent(&fftComponent, false);

        // Start the audio
        setAudioChannels(1, 2); 
        // 1 input channels(cause in this case i can properly load and produce the sound) (e.g., microphone), 2 output channels
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
    auto area = getLocalBounds().reduced(10); // Add padding around edges.

    // Reserve space for the Wave visualization
    auto waveArea = area.removeFromTop(area.getHeight() / 3); // Top 1/3 for WaveComponent
    WaveComponent.setBounds(waveArea.reduced(10));            // Add padding around WaveComponent

    // Reserve space for sliders
    auto sliderArea = area.removeFromTop(50);                 // 50px for slider
    amplitudeSlider.setBounds(sliderArea.reduced(10));        // Center amplitude slider in reserved area

    // Reserve remaining space for FFT visualization
    fftComponent.setBounds(area.reduced(10));                 // Remaining area for FFTComponent
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
        std::cout << sampleRate << " SAMPLE RATE UPDATED\n";        
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

        if (prevSampleRate != currentSampleRate)
        {
            std::cout << "Sample Rate is changed to " << currentSampleRate << std::endl;
            prevSampleRate = currentSampleRate;
            std::cout << "Number of Sample from the I/O Buffer " << bufferToFill.numSamples << std::endl;
        }
        // TODO : change dynamically the size of wave component due to the changes
        // in the dynamic changes in the bufferToFIll ?  
        auto* leftInputBuffer = bufferToFill.buffer->getReadPointer(0, bufferToFill.startSample);
        auto* rightInputBuffer = bufferToFill.buffer->getReadPointer(1, bufferToFill.startSample);
        auto* leftOutputBuffer = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
        auto* rightOutputBuffer = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

        for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
        {
            // Simple processing: amplify the input
            float processedSampleLeft = (leftInputBuffer[sample] * .238f) * amplitude;
        
            // Write processed audio to output
            leftOutputBuffer[sample] = processedSampleLeft;
            rightOutputBuffer[sample] = processedSampleLeft;

            // Optionally update visualizations
            fftComponent.pushNextSampleIntoFifo(processedSampleLeft);
            WaveComponent.set_iter_value(processedSampleLeft, sample);
            // TODO spectorgram pusging value FUNCTION call here !!! 
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
        WaveComponent.setFrequency(frequencySlider.getValue());
    }

    void updateAmplitude()
    {
        amplitude = amplitudeSlider.getValue();
        WaveComponent.setAmplitude(amplitude);
    }

    juce::Slider frequencySlider;
    juce::Slider amplitudeSlider;
    juce::Label frequencyLabel;
    juce::Label amplitudeLabel;
    juce::Label sineWaveLabel;
    juce::Label fftLabel;
    
    WaveComponent WaveComponent;
    FFTComponent fftComponent;
    // std::unique_ptr<juce::AudioDeviceSelectorComponent> audioSettingsComponent;

    double prevSampleRate = -1.0f;
    double currentSampleRate = 0.0f;
    double currentAngle = 0.0f;
    double angleDelta = 0.0f;
    double amplitude = 0.125f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};