#include "MainComponent.h"

MainComponent::MainComponent()
{

    addAndMakeVisible(player1);
    addAndMakeVisible(addPlayerButton);

    addPlayerButton.addListener(this);
    addPlayerButton.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    addPlayerButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);


    setSize(800, 500);
    setAudioChannels(0, 2);


}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{

    mixerSource.prepareToPlay(samplesPerBlockExpected, sampleRate);


    mixerSource.addInputSource(player1.getAudioSource(), false);
    mixerSource.addInputSource(player2.getAudioSource(), false);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{

    mixerSource.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    mixerSource.releaseResources();
}

void MainComponent::buttonClicked(juce::Button* button)
{

    if (button == &addPlayerButton)
    {
        showSecondPlayer = true;
        addAndMakeVisible(player2);
        addPlayerButton.setVisible(false);
        resized();
    }
}

void MainComponent::resized()
{

    auto area = getLocalBounds();

    if (showSecondPlayer)
    {

        player1.setBounds(area.removeFromLeft(getWidth() / 2));
        player2.setBounds(area);
    }
    else
    {

        auto buttonArea = area.removeFromBottom(60);
        player1.setBounds(area);
        addPlayerButton.setBounds(buttonArea.withSizeKeepingCentre(200, 40));
    }
}