#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
	formatManager.registerBasicFormats();
}
PlayerAudio::~PlayerAudio()
{
	transportSource.setSource(nullptr);
}
void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	transportSource.getNextAudioBlock(bufferToFill);
}
void PlayerAudio::releaseResources()
{
	transportSource.releaseResources();
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
