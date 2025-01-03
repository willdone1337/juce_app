#pragma once

#include <JuceHeader.h>

// Forward declaration of FFTComponent
class FFTComponent;

class FFTThread : public juce::Thread
{
public:
    FFTThread(FFTComponent& fftComp)
        : juce::Thread("FFTThread"), fftComponent(fftComp)
    {
    }

    void run() override
    {
        while (!threadShouldExit())
        {
            if (fftComponent.isDataAvailable())
            {
                fftComponent.processFFT();
            }

            // Sleep for a bit to allow other threads to run (use a suitable delay)
            wait(20);
        }
    }

private:
    FFTComponent& fftComponent;
};