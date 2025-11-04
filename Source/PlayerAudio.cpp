#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
    : resamplingSource(&transportSource, false) 
{
    formatManager.registerBasicFormats();
    positionSlider.setRange(0.0, 100.0);
    positionSlider.addListener(this);

    positionSlider.setColour(juce::Slider::trackColourId, juce::Colours::black);
    positionSlider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
    positionSlider.setColour(juce::Slider::backgroundColourId, juce::Colours::darkgrey);

    currentTimeLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    totalTimeLabel.setColour(juce::Label::textColourId, juce::Colours::black);

    currentTimeLabel.setText("00:00", juce::dontSendNotification);
    totalTimeLabel.setText("00:00", juce::dontSendNotification);

   
    resamplingSource.setResamplingRatio(1.0);

    startTimer(100);
}

PlayerAudio::~PlayerAudio()
{
    transportSource.setSource(nullptr);
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    resamplingSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    resamplingSource.getNextAudioBlock(bufferToFill);
}

void PlayerAudio::releaseResources()
{
    resamplingSource.releaseResources();
}

bool PlayerAudio::loadFile(const juce::File& file)
{
    if (auto* reader = formatManager.createReaderFor(file))
    {
        transportSource.stop();
        transportSource.setSource(nullptr);
        readerSource.reset();

        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);

        // إعادة تعيين السرعة عند تحميل ملف جديد
        setSpeed(1.0f);

        auto meta = reader->metadataValues;
        title = meta.getValue("title", {});
        artist = meta.getValue("artist", {});
        filename = file.getFileName();

        positionSlider.setRange(0.0, getLength());
        totalTimeLabel.setText(formatTime(getLength()), juce::dontSendNotification);

        return true;
    }
    return false;
}

void PlayerAudio::play()
{
    transportSource.start();
}

void PlayerAudio::pause()
{
    transportSource.stop();
}

void PlayerAudio::stop()
{
    transportSource.stop();
    transportSource.setPosition(0.0);
}

void PlayerAudio::goToStart()
{
    transportSource.setPosition(0.0);
}

void PlayerAudio::goToEnd() {
    double length = transportSource.getLengthInSeconds();
    transportSource.setPosition(length - 0.01);
}

void PlayerAudio::setGain(float gain)
{
    transportSource.setGain(gain);
}

void PlayerAudio::setPosition(double pos)
{
    transportSource.setPosition(pos);
}

double PlayerAudio::getPosition() const
{
    return transportSource.getCurrentPosition();
}

double PlayerAudio::getLength() const
{
    return transportSource.getLengthInSeconds();
}

juce::String PlayerAudio::getTitle() const
{
    return title;
}

juce::String PlayerAudio::getArtist() const
{
    return artist;
}

juce::String PlayerAudio::getFilename() const
{
    return filename;
}

void PlayerAudio::setLooping(bool shouldLoop)
{
    looping = shouldLoop;
    if (readerSource != nullptr)
        readerSource->setLooping(shouldLoop);
}

bool PlayerAudio::isLooping() const
{
    return looping;
}

void PlayerAudio::checkForLoop()
{
    if (looping && transportSource.hasStreamFinished())
    {
        transportSource.setPosition(0.0);
        transportSource.start();
    }
}

void PlayerAudio::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &positionSlider)
    {
        setPosition(slider->getValue());
    }
}

void PlayerAudio::timerCallback()
{
    if (isAbLoopingActive == true)
    {
        if (getPosition() >= loopPointB)
        {
            setPosition(loopPointA);
        }
    }
    if (getLength() > 0.0)
    {
        positionSlider.setValue(getPosition(), juce::dontSendNotification);
        currentTimeLabel.setText(formatTime(getPosition()), juce::dontSendNotification);
    }
}

juce::String PlayerAudio::formatTime(double seconds)
{
    int time = static_cast<int>(seconds);
    int minutes = time / 60;
    int second = time % 60;
    return juce::String::formatted("%02d:%02d", minutes, second);
}

void PlayerAudio::setLoopPointA()
{
    loopPointA = getPosition();

    if (loopPointB != -1.0 && loopPointA >= loopPointB) {
        loopPointB = -1.0;
        isAbLoopingActive = false;
    }
}

void PlayerAudio::setLoopPointB()
{
    double potentialB = getPosition();

    if (loopPointA != -1.0 && potentialB > loopPointA) {
        loopPointB = getPosition();
        isAbLoopingActive = true;
    }
}

void PlayerAudio::clearABloop()
{
    loopPointA = -1.0;
    loopPointB = -1.0;
    isAbLoopingActive = false;
}

void PlayerAudio::seekbackward()
{
    double current_time = getPosition();
    double new_time = current_time - 10.00;
    if (new_time < 0.0)
        setPosition(0.0);
    else
        setPosition(new_time);
}

void PlayerAudio::seekforward()
{
    double current_time = getPosition();
    double lenght = getLength();
    double new_time = current_time + 10.00;
    if (new_time > lenght)
        setPosition(lenght);
    else
        setPosition(new_time);
}

void PlayerAudio::setSpeed(float newSpeed)
{
    currentSpeed = newSpeed;
    resamplingSource.setResamplingRatio(currentSpeed); 
}