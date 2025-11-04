#pragma once
#include <JuceHeader.h>

class PlayerAudio : public juce::AudioAppComponent,
    public juce::Slider::Listener,
    public juce::Timer
{
public:
    PlayerAudio();
    ~PlayerAudio();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    bool loadFile(const juce::File& file);
    void play();
    void stop();
    void setGain(float gain);
    void setPosition(double pos);
    double getPosition() const;
    double getLength() const;

    void setLooping(bool shouldLoop);
    bool isLooping() const;
    void checkForLoop();

    void pause();
    void goToStart();
    void goToEnd();

    void setLoopPointA();
    void setLoopPointB();
    void clearABloop();

    juce::Slider* getPositionSlider() { return &positionSlider; }
    juce::Label* getCurrentTimeLabel() { return &currentTimeLabel; }
    juce::Label* getTotalTimeLabel() { return &totalTimeLabel; }
    juce::TextButton* getShowControlsButton() { return &showControlsButton; }

    juce::AudioTransportSource* getAudioSource() { return &transportSource; }
    void seekforward();
    void seekbackward();

    juce::String getTitle( ) const;
    juce::String getArtist() const;
    juce::String getFilename() const;

private:
    juce::AudioFormatManager formatManager;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    juce::AudioTransportSource transportSource;

    juce::String title;
    juce::String artist;
    juce::String filename;

    bool looping = false;

    juce::Slider positionSlider;
    juce::Label currentTimeLabel;
    juce::Label totalTimeLabel;
    juce::TextButton showControlsButton;
    bool controlsVisible = false;

    double loopPointA = -1.0;
    double loopPointB = -1.0;
    bool isAbLoopingActive = false;

    void sliderValueChanged(juce::Slider* slider) override;

    void timerCallback() override;
    juce::String formatTime(double seconds);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};
