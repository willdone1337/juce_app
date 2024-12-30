#pragma once

#include <JuceHeader.h>

class MainComponent : public juce::AudioAppComponent
{
public:
    MainComponent()
    {
        setSize(800, 400);
        
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


        // Add Amplitude slider
        addAndMakeVisible(amplitudeSlider);
        amplitudeSlider.setRange(0.0, 3.0, .1f);
        amplitudeSlider.setValue(1.0);
        amplitudeSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 90, 20);
        amplitudeSlider.onValueChange = [this] { updateAmplitude(); };


        // Add frequency label
        addAndMakeVisible(amplitudeLabel);
        amplitudeLabel.setText("\nAmplitude", juce::dontSendNotification);
        amplitudeLabel.attachToComponent(&amplitudeSlider, false);

        // Start the audio
        setAudioChannels(0, 2); // no inputs, 2 outputs
        std::cout << angleDelta << "-> Angel Data" << std::endl;
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
    }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override
    {
        std::cout << sampleRate << "-> Sample Rate" << std::endl;
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
            // auto currentSample = (float)(currentAngle);
            currentAngle += angleDelta;
            leftBuffer[sample] = currentSample * this->level * this->amplitude;
            rightBuffer[sample] = currentSample * this->level * this->amplitude;
            std::cout << "Current Angle: " << currentAngle << std::endl;
            std::cout << "Current Sample: " << currentSample << std::endl;
            std::cout << "Current Angle Delta : " << angleDelta << std::endl;
            std::cout << "-------------" << std::endl;

        }
    }

    void releaseResources() override
    {
    }
    
private:
    void updateFrequency() 
    {
        auto cyclesPerSample = frequencySlider.getValue() / currentSampleRate;
        angleDelta = cyclesPerSample ;//* 2.0 * juce::MathConstants<double>::pi;
    }

    void updateAmplitude(){
        this->amplitude = amplitudeSlider.getValue();
    }

    juce::Slider frequencySlider;
    juce::Slider amplitudeSlider;
    juce::Label frequencyLabel;
    juce::Label amplitudeLabel;
    
    double currentSampleRate = 0.0;
    double currentAngle = 0.0;
    double angleDelta = 0.0;
    double amplitude = 1.0;
    float level = 0.125f;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

