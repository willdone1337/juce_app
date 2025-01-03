#pragma once

#include <JuceHeader.h>

class FFTComponent : public juce::Component, private juce::Timer
{
public:
    FFTComponent()
        : forwardFFT(fftOrder), spectrogramImage(juce::Image::RGB, 512, 512, true)   
    {
        setOpaque(true); 
        startTimerHz(60);  // Update at 30Hz. It is enough
        std::fill(fftData.begin(), fftData.end(), 0.0f);// 
        std::cout << "FFT Order: " << fftOrder << ", FFT Size: " << fftSize << std::endl;
        std::cout << "fftData size: " << fftData.size() << std::endl;
    }

    ~FFTComponent() override = default;

    void pushNextSampleIntoFifo (float sample) noexcept
    {
        // if the fifo contains enough data, set a flag to say
        // that the next line should now be rendered..
        if (fifoIndex == fftSize)       
        {
            if (! nextFFTBlockReady)
            {
                std::fill (fftData.begin(), fftData.end(), 0.0f);
                std::copy (fifo.begin(), fifo.end(), fftData.begin());
                nextFFTBlockReady = true;
            }
            fifoIndex = 0;
        }
 
        fifo[(size_t) fifoIndex++] = sample; // used this function from official documentation
    }

    void drawNextLineOfSpectrogram()
    {
        auto rightHandEdge = spectrogramImage.getWidth() - 1; //511
        auto imageHeight   = spectrogramImage.getHeight(); // 512
        // first, shuffle our image leftwards by 1 pixel..
        spectrogramImage.moveImageSection (0, 0, 1, 0, rightHandEdge, imageHeight);         
        
        // then render our FFT data..
        forwardFFT.performFrequencyOnlyForwardTransform (fftData.data());                   
        // find the range of values produced, so we can scale our rendering to
        // show up the detail clearly
        auto maxLevel = juce::FloatVectorOperations::findMinAndMax (fftData.data(), fftSize / 2); 

        juce::Image::BitmapData bitmap { spectrogramImage, rightHandEdge, 0, 1, imageHeight, juce::Image::BitmapData::writeOnly };
 
        for (auto y = 1; y < imageHeight; ++y)                                              
        {
            auto skewedProportionY = 1.0f - std::exp (std::log ((float) y / (float) imageHeight) * 0.2f);
            auto fftDataIndex = (size_t) juce::jlimit (0, fftSize / 2, (int) (skewedProportionY * fftSize / 2));
            auto level = juce::jmap (fftData[fftDataIndex], 0.0f, juce::jmax (maxLevel.getEnd(), 1e-5f), 0.0f, 1.0f);
    
            bitmap.setPixelColour (0, y, juce::Colour::fromHSV (level, 1.0f, level, 1.0f)); 
        }
    }

    void paint(juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::whitesmoke);
        g.setOpacity (1.0f);
        g.drawImage (spectrogramImage, getLocalBounds().toFloat());
    }

    void resized() override
    {
        repaint();
    }
    static constexpr int fftOrder = 9;  // Adjustable FFT order
    static constexpr int fftSize = 1 << fftOrder;  // FFT size = 2^fftOrder and our array in leftBuffer is 512 and they are fftOrder is depends on it

private:
     void timerCallback() override
    {
        if (nextFFTBlockReady)
        {
            drawNextLineOfSpectrogram();
            nextFFTBlockReady = false;
            repaint();
        }
    }
    
    juce::dsp::FFT forwardFFT;
    std::array<float, fftSize * 2> fftData;
    std::array<float, fftSize> fifo;
    int fifoIndex = 0;
    juce::Image spectrogramImage;
    bool nextFFTBlockReady = false;

    // std::vector<float> fftData = std::vector<float>(1 << fftOrder, 0.0f);  
    
    // Update below: I have to init array with the 2x size and it works then. Shit mistake !
    // I use vector instead of array because it became less painfull rather than array
    // array usage leads memory error when setting order in forwardFFT as 9
    // however in the vector this error does not appear
    // actually do not know what happens but suggest somehting related to dynamic memory allocation ????))))
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FFTComponent)
};