
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

void PlayerGUI::resized()
{
    auto area = getLocalBounds();
    area.removeFromTop(40); 

    int y = 10; 
    int x = 10;
    int spacing = 10;
    int buttonWidth = 80;
    int buttonHeight = 30;
    int rowHeight = buttonHeight + spacing;
    int fullWidth = getWidth() - 20;

  
    loadButton.setBounds(x, y, buttonWidth, buttonHeight);
    playButton.setBounds(x + 90, y, buttonWidth, buttonHeight);
    pauseButton.setBounds(x + 180, y, buttonWidth, buttonHeight);
    stopButton.setBounds(x + 270, y, buttonWidth, buttonHeight);
    y += rowHeight; 

    goToStartButton.setBounds(x, y, buttonWidth, buttonHeight);
    BackwardButton.setBounds(x + 90, y, 70, buttonHeight); 
    ForwardButton.setBounds(x + 170, y, 70, buttonHeight); 
    goToEndButton.setBounds(x + 250, y, buttonWidth, buttonHeight);
    y += rowHeight;

  
    loopButton.setBounds(x, y, buttonWidth, buttonHeight);
    setAButton.setBounds(x + 90, y, buttonWidth, buttonHeight);
    setBButton.setBounds(x + 180, y, buttonWidth, buttonHeight);
    clearLoopButton.setBounds(x + 270, y, 100, buttonHeight);
    y += rowHeight;


    muteButton.setBounds(x, y, buttonWidth, buttonHeight);
    controlButton.setBounds(x + 90, y, 110, buttonHeight);
    y += rowHeight; 

    playlistBox.setBounds(x, y, fullWidth, 120);
    y += 125; 

   
    volumeSlider.setBounds(x, y, fullWidth, 25);
    y += 30; 

    playerAudio.getPositionSlider()->setBounds(x, y, fullWidth, 40);
    y += 45; 

  
    infolabel.setBounds(x, y, fullWidth, 30);
    y += 30; 

    
    playerAudio.getCurrentTimeLabel()->setBounds(x, y, 80, 30);
    playerAudio.getTotalTimeLabel()->setBounds(getWidth() - 90, y, 80, 30);


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
        updateInfoLabel(playlist[lastRowSelected]);
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
