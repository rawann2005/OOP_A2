#include "PlayerGUI.h"

PlayerGUI::PlayerGUI()
    : formatManager()
{
  
    formatManager.registerBasicFormats();

    for (auto* btn : { &loadButton, &playButton, &stopButton, &loopButton, &controlButton,
                       &setAButton, &setBButton, &clearLoopButton,
                       &BackwardButton, &ForwardButton,
                       &pauseButton, &goToStartButton, &goToEndButton, &muteButton })
    {
        addAndMakeVisible(btn);
        btn->addListener(this);
        btn->setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
        btn->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
    }

    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    addAndMakeVisible(volumeSlider);

    speedSlider.setRange(0.5, 2.0, 0.1);
    speedSlider.setValue(1.0);
    speedSlider.addListener(this);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    speedSlider.setTextValueSuffix("x");
    addAndMakeVisible(speedSlider);

    addAndMakeVisible(playlistBox);
    playlistBox.setRowHeight(25);
    playlistBox.setColour(juce::ListBox::backgroundColourId, juce::Colours::darkgrey);

    addAndMakeVisible(infolabel);
    infolabel.setText("No file loaded", juce::dontSendNotification);
    infolabel.setJustificationType(juce::Justification::centredLeft);

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

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::darkgrey);

   
    g.setColour(juce::Colours::black);
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    g.drawText("Audio Player", getLocalBounds().removeFromTop(40),
        juce::Justification::centred, true);

  
    paintWaveform(g);

    g.setColour(juce::Colour(0xff26c6da));
    g.drawRect(getLocalBounds(), 1);
}

void PlayerGUI::paintWaveform(juce::Graphics& g)
{
    int waveformHeight = 150;
    auto waveformArea = getLocalBounds().removeFromBottom(waveformHeight);

    g.setColour(juce::Colours::black);
    g.fillRect(waveformArea);

   
    if (thumbnail.getTotalLength() > 0.0 && currentFile.existsAsFile())
    {
        auto waveformDrawingArea = waveformArea.withTrimmedTop(25);

        g.setColour(juce::Colours::greenyellow);
        thumbnail.drawChannels(g, waveformDrawingArea, 0.0, thumbnail.getTotalLength(), 1.0f);

        double audioPosition = playerAudio.getPosition();
        double audioLength = playerAudio.getLength();

        if (audioLength > 0.0)
        {
            float playheadPosition = (float)(audioPosition / audioLength) * waveformDrawingArea.getWidth();

            g.setColour(juce::Colours::red);
            g.drawLine(waveformDrawingArea.getX() + playheadPosition,
                waveformDrawingArea.getY(),
                waveformDrawingArea.getX() + playheadPosition,
                waveformDrawingArea.getBottom(), 3.0f);


        }
    }
   

 
    g.setColour(juce::Colours::white);
    g.drawRect(waveformArea, 1);

    auto titleArea = waveformArea.withHeight(25);
    g.setColour(juce::Colours::white);
    g.setFont(juce::Font(14.0f, juce::Font::bold));
    g.drawText("Waveform", titleArea, juce::Justification::centred);

    
    g.setColour(juce::Colours::darkgrey);
    g.fillRect(titleArea);

}
void PlayerGUI::loadFileForWaveform(const juce::File& file)
{
    currentFile = file;

   
    thumbnail.reset(2, 44100);

 
    auto* inputSource = new juce::FileInputSource(file);
    thumbnail.setSource(inputSource);

  
    repaint();
}

void PlayerGUI::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(40);

    int y = 10;
    int x = 10;
    int spacing = 8;
    int buttonWidth = 80;
    int buttonHeight = 28;
    int rowHeight = buttonHeight + spacing;
    int fullWidth = getWidth() - 20;


    loadButton.setBounds(x, y, buttonWidth, buttonHeight);
    y += rowHeight;

  
    playButton.setBounds(x, y, buttonWidth, buttonHeight);
    stopButton.setBounds(x + buttonWidth + spacing, y, buttonWidth, buttonHeight);
    pauseButton.setBounds(x + (buttonWidth + spacing) * 2, y, buttonWidth, buttonHeight);
    y += rowHeight;

   
    int seekButtonsWidth = 65;
    goToStartButton.setBounds(x, y, buttonWidth, buttonHeight);
    BackwardButton.setBounds(x + buttonWidth + spacing, y, seekButtonsWidth, buttonHeight);
    ForwardButton.setBounds(x + buttonWidth + spacing * 2 + seekButtonsWidth, y, seekButtonsWidth, buttonHeight);
    goToEndButton.setBounds(x + buttonWidth + spacing * 3 + seekButtonsWidth * 2, y, buttonWidth, buttonHeight);
    y += rowHeight;

   
    int sliderWidth = 150;
    muteButton.setBounds(x, y, buttonWidth, buttonHeight);
    loopButton.setBounds(x + buttonWidth + spacing, y, buttonWidth, buttonHeight);
    speedSlider.setBounds(x + (buttonWidth + spacing) * 2, y, sliderWidth, buttonHeight);
    y += rowHeight;

  
    setAButton.setBounds(x, y, buttonWidth, buttonHeight);
    setBButton.setBounds(x + buttonWidth + spacing, y, buttonWidth, buttonHeight);
    clearLoopButton.setBounds(x + (buttonWidth + spacing) * 2, y, 90, buttonHeight);
    y += rowHeight;


    controlButton.setBounds(x, y, 110, buttonHeight);
    y += rowHeight + 10;

 
    playlistBox.setBounds(x, y, fullWidth, 80);
    y += 85;

    volumeSlider.setBounds(x, y, fullWidth, 25);
    y += 30;

    playerAudio.getPositionSlider()->setBounds(x, y, fullWidth, 35);
    y += 40;

    infolabel.setBounds(x, y, fullWidth, 25);
    y += 30;

   
    playerAudio.getCurrentTimeLabel()->setBounds(x, y, 80, 20);
    playerAudio.getTotalTimeLabel()->setBounds(getWidth() - x - 80, y, 80, 20);
    y += 25;

   
    int waveformHeight = 150; 
    auto waveformArea = juce::Rectangle<int>(x, y, fullWidth, waveformHeight);
}
void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        loadMultipleFiles();
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
    else if (slider == &speedSlider)
    {
        playerAudio.setSpeed((float)speedSlider.getValue());
    }
}

void PlayerGUI::loadMultipleFiles()
{
    fileChooser = std::make_unique<juce::FileChooser>(
        "Select audio files...", juce::File{}, "*.wav;*.mp3;*.aiff;*.flac");

    fileChooser->launchAsync(
        juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectMultipleItems,
        [this](const juce::FileChooser& fc)
        {
            juce::Array<juce::File> files = fc.getResults();
            if (files.isEmpty()) return;


            for (auto f : files)
                playlist.push_back(f);

            playlistBox.updateContent();
            playlistBox.repaint();

            if (!playlist.empty())
            {
                playerAudio.loadFile(playlist[0]);
                updateInfoLabel(playlist[0]);
            }
        });
}

int PlayerGUI::getNumRows()
{
    return (int)playlist.size();
}

void PlayerGUI::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowNumber < 0 || rowNumber >= (int)playlist.size()) return;

    g.fillAll(rowIsSelected ? juce::Colours::orange : juce::Colours::darkgrey);
    g.setColour(juce::Colours::white);
    g.drawText(playlist[rowNumber].getFileName(), 5, 0, width - 10, height, juce::Justification::centredLeft);
}

void PlayerGUI::selectedRowsChanged(int lastRowSelected)
{
    if (lastRowSelected >= 0 && lastRowSelected < (int)playlist.size())
    {
        playerAudio.loadFile(playlist[lastRowSelected]);
        loadFileForWaveform(playlist[lastRowSelected]);
        updateInfoLabel(playlist[lastRowSelected]);
        repaint();
    }
}

void PlayerGUI::updateInfoLabel(const juce::File& file)
{
    juce::String title = playerAudio.getTitle();
    juce::String artist = playerAudio.getArtist();
    juce::String name = file.getFileName();
    double dur = playerAudio.getLength();

    juce::String text;
    if (title.isNotEmpty() || artist.isNotEmpty())
        text = "Title: " + title + " | Artist: " + artist + " | Duration: " + juce::String(dur, 2) + " s";
    else
        text = name + " (" + juce::String(dur, 2) + " s)";

    infolabel.setText(text, juce::dontSendNotification);
}

void PlayerGUI::timerCallback()
{
    playerAudio.checkForLoop();
    repaint();
}

juce::AudioSource* PlayerGUI::getAudioSource()
{
    return playerAudio.getAudioSource();
}