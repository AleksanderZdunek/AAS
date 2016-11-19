#include <iostream>
#include "archie.h"
#include <cmath>

const float twoPI = (float)atan(1)*8;
const float toneFrequency = 261.626f;
const UINT32 sampleRate = 48000; //This should be configurable, really.
const int channels = 2;	//This should be configurable, really.
const int sampleSize = 4; //bytes. //Because IEEE_FLOAT //This should be configurable, really.
const int frameSize = channels*sampleSize;
const float phaseDelta = toneFrequency * twoPI / sampleRate;

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
	Archie::LoadData = Tone;
	Archie::Play();
	Archie::UnInit(); //This will never be reached, because the program does not handle user input to terminate gracefully.

	return 0;
}