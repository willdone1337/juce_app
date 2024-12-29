// App.cpp
#include <JuceHeader.h>
#include "MainWindow.h"

class SineSynthApp : public juce::JUCEApplication
{
public:
    SineSynthApp() = default;

    const juce::String getApplicationName() override { return "SineSynthApp"; }
    const juce::String getApplicationVersion() override { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override { return true; }

    void initialise(const juce::String& commandLine) override
    {
        mainWindow = std::make_unique<MainWindow>(getApplicationName());
        mainWindow->setVisible(true);
    }

    void shutdown() override
    {
        mainWindow = nullptr;
    }

    void systemRequestedQuit() override
    {
        quit();
    }

    void anotherInstanceStarted(const juce::String& commandLine) override {}

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(SineSynthApp)