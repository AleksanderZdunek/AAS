#include <iostream>
//#include "archie.h"
#include "archie_linux.h"
#include <cmath>

const float twoPI = (float)atan(1)*8;
const float toneFrequency = 261.626f;
UINT32 sampleRate(0);
int channels(0);
int sampleSize(0); //bytes
int frameSize(0);
float phaseDelta(0);

void GetConfigValues()
{
	sampleRate = Archie::configFormat.Format.nSamplesPerSec;
	channels = Archie::configFormat.Format.nChannels;
	sampleSize = Archie::configFormat.Format.wBitsPerSample/8;
	frameSize = channels*sampleSize;
	phaseDelta = toneFrequency * twoPI / sampleRate;
}

DWORD Tone(UINT32 bufferSize, BYTE* pBuffer)
{
	static float phase = 0;
	UINT32 numberOfFrames(bufferSize/frameSize); //we are assuming that bufferSize is a multiple of frameSize
	for (UINT32 i = 0; i < numberOfFrames; i++)
	{
		float waveSample = sin(phase);

		phase += phaseDelta;
		if (phase >= twoPI) phase -= twoPI;

		*((float*)pBuffer + 2 * i + 0) = waveSample; //left channel
		*((float*)pBuffer + 2 * i + 1) = waveSample; //right channel
	}
	return 0;
}

int main(int argc, char* argv[])
{
	std::cout << "Hello Archie!" << std::endl;
	std::cout << "A tinnitus simulator is the Hello World of audio programming." << std::endl;

	try
	{
		Archie::Init();
		GetConfigValues();
		Archie::LoadData = Tone;
		Archie::Play();
		Archie::UnInit(); //This will never be reached, because the program does not handle user input to terminate gracefully.
	}
	catch (std::runtime_error e)
	{
		std::cout << "Runtime error. what: " << e.what() << std::endl;
	}

	return 0;
}
