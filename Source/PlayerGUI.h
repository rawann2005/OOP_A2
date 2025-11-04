#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer,
    public juce::ListBoxModel
{
public:
    PlayerGUI();
    ~PlayerGUI() override;

    void resized() override;
    void paint(juce::Graphics& g) override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    void timerCallback() override;

    juce::AudioSource* getAudioSource();

private:
    PlayerAudio playerAudio;


    juce::TextButton loadButton{ "Load Files" };
    juce::TextButton playButton{ "Play" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton loopButton{ "Loop" };
    juce::TextButton pauseButton{ "||" };
    juce::TextButton goToStartButton{ "<|" };
    juce::TextButton goToEndButton{ "|>" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton controlButton{ "Show Controls" };
    juce::TextButton BackwardButton{ "<< 10s" };
    juce::TextButton ForwardButton{ ">> 10s" };
    juce::TextButton setAButton{ "Set A" };
    juce::TextButton setBButton{ "Set B" };
    juce::TextButton clearLoopButton{ "clear loop" };

    juce::Slider volumeSlider;
    juce::Slider speedSlider;

    bool isMuted = false;
    float lastVolume = 0.5f;

    juce::Label infolabel;


    juce::ListBox playlistBox{ "PlayList", this };
    std::vector<juce::File> playlist;

    std::unique_ptr<juce::FileChooser> fileChooser;


    juce::AudioThumbnailCache thumbnailCache{ 5 };
    juce::AudioThumbnail thumbnail{ 512, formatManager, thumbnailCache };
    juce::AudioFormatManager formatManager;

    juce::File currentFile;
    void paintWaveform(juce::Graphics& g);
    void loadFileForWaveform(const juce::File& file);


    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void selectedRowsChanged(int lastRowSelected) override;


    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;


    void loadMultipleFiles();
    void updateInfoLabel(const juce::File& file);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)
};