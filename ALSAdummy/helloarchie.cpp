#include <iostream>
//#include "archie.h"
#include "archie_linux.h"
#include <cmath>

const float twoPI = (float)atan(1)*8;
const float toneFrequency = 261.626f;
UINT32 sampleRate(0); //This should be configurable, really.
int channels(0);	//This should be configurable, really.
int sampleSize(0); //bytes. //Because IEEE_FLOAT //This should be configurable, really.
int frameSize(0);
const float phaseDelta = toneFrequency * twoPI / sampleRate;

void GetConfigValues()
{
	sampleRate = Archie::configFormat.Format.nSamplesPerSec;
	channels = Archie::configFormat.Format.nChannels;
	sampleSize = Archie::configFormat.Format.wBitsPerSample;
	frameSize = channels*sampleSize;
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

	Archie::Init();
	GetConfigValues();
	Archie::LoadData = Tone;
	Archie::Play();
	Archie::UnInit(); //This will never be reached, because the program does not handle user input to terminate gracefully.

	return 0;
}
