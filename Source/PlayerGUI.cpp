
#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
{

    for (auto* btn : { &loadButton, &playButton, &stopButton, &loopButton, &controlButton,
                       &setAButton, &setBButton, &clearLoopButton,
                       &BackwardButton, &ForwardButton,
                       &pauseButton, &goToStartButton, &goToEndButton, &muteButton })
    {
        addAndMakeVisible(btn);
        btn->addListener(this);


        btn->setColour(juce::TextButton::buttonColourId, juce::Colours::lightgrey);
        btn->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    }

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    addAndMakeVisible(volumeSlider);

    addAndMakeVisible(playerAudio.getPositionSlider());
    addAndMakeVisible(playerAudio.getCurrentTimeLabel());
    addAndMakeVisible(playerAudio.getTotalTimeLabel());

    controlButton.setButtonText("Hide Controls");
    startTimer(100);
}

PlayerGUI::~PlayerGUI() {}

void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);
}

void PlayerGUI::releaseResources()
{
    playerAudio.releaseResources();
}

void PlayerGUI::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(40);

    // Row 1: 
    loadButton.setBounds(10, 50, 80, 30);
    playButton.setBounds(100, 50, 80, 30);
    pauseButton.setBounds(190, 50, 80, 30);
    stopButton.setBounds(280, 50, 80, 30);

    // Row 2:
    goToStartButton.setBounds(10, 90, 80, 30);
    BackwardButton.setBounds(100, 90, 70, 30);
    ForwardButton.setBounds(180, 90, 70, 30);
    goToEndButton.setBounds(260, 90, 80, 30);

    // Row 3:
    loopButton.setBounds(10, 130, 80, 30);
    setAButton.setBounds(100, 130, 80, 30);
    setBButton.setBounds(190, 130, 80, 30);
    clearLoopButton.setBounds(280, 130, 100, 30);

    // Row 4:
    muteButton.setBounds(10, 170, 80, 30);
    controlButton.setBounds(100, 170, 110, 30);


    volumeSlider.setBounds(10, 210, getWidth() - 20, 25);

    playerAudio.getPositionSlider()->setBounds(10, 245, getWidth() - 20, 40);


    playerAudio.getCurrentTimeLabel()->setBounds(10, 290, 80, 30);
    playerAudio.getTotalTimeLabel()->setBounds(getWidth() - 90, 290, 80, 30);
}



void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select an audio file...", juce::File{}, ".wav;.mp3;.aiff;.flac");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                    playerAudio.loadFile(file);
            });
    }
    else if (button == &playButton)
    {
        playerAudio.play();
    }
    else if (button == &stopButton)
    {
        playerAudio.stop();
    }
    else if (button == &loopButton)
    {
        bool currentLooping = playerAudio.isLooping();
        playerAudio.setLooping(!currentLooping);
        loopButton.setButtonText(playerAudio.isLooping() ? "Loop ON" : "Loop OFF");
    }

    else if (button == &pauseButton)
    {
        playerAudio.pause();
    }
    else if (button == &goToStartButton)
    {
        playerAudio.goToStart();
    }
    else if (button == &goToEndButton)
    {
        playerAudio.goToEnd();
    }
    else if (button == &muteButton)
    {
        if (!isMuted)
        {
            lastVolume = (float)volumeSlider.getValue();
            playerAudio.setGain(0.0f);
            isMuted = true;
            muteButton.setButtonText("Unmute");
        }
        else
        {
            playerAudio.setGain(lastVolume);
            isMuted = false;
            muteButton.setButtonText("Mute");
        }
        volumeSlider.setValue(isMuted ? 0.0f : lastVolume, juce::dontSendNotification);
    }

    else if (button == &controlButton)
    {
        bool shouldShow = !playerAudio.getPositionSlider()->isVisible();
        playerAudio.getPositionSlider()->setVisible(shouldShow);
        playerAudio.getCurrentTimeLabel()->setVisible(shouldShow);
        playerAudio.getTotalTimeLabel()->setVisible(shouldShow);
        controlButton.setButtonText(shouldShow ? "Hide Controls" : "Show Controls");
    }
    else if (button == &setAButton)
    {
        playerAudio.setLoopPointA();
    }
    else if (button == &setBButton)
    {
        playerAudio.setLoopPointB();
    }
    else if (button == &clearLoopButton)
    {
        playerAudio.clearABloop();
    }
    else if (button == &BackwardButton)
    {
        playerAudio.seekbackward();
    }
    else if (button == &ForwardButton)
    {
        playerAudio.seekforward();
    }
}


void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
    {
        playerAudio.setGain((float)volumeSlider.getValue());
        if (isMuted && volumeSlider.getValue() > 0.0f)
        {
            isMuted = false;
            muteButton.setButtonText("Mute");
            lastVolume = (float)volumeSlider.getValue();
        }
    }
}

void PlayerGUI::timerCallback()
{
    playerAudio.checkForLoop();
}


void PlayerGUI::paint(juce::Graphics& g)
{

    g.fillAll(juce::Colours::darkgrey);

    g.setColour(juce::Colours::black);
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    g.drawText("Audio Player", getLocalBounds().removeFromTop(40),
        juce::Justification::centred, true);

    g.setColour(juce::Colour(0xff26c6da));
    g.drawRect(getLocalBounds(), 1);
}

juce::AudioTransportSource* PlayerGUI::getAudioSource()
{
    return playerAudio.getAudioSource();
}