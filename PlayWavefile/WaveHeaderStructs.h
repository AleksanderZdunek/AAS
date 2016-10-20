#pragma once
#ifndef WAVEHEADERSTRUCTS_H
#define WAVEHEADERSTRUCTS_H

//#include <mmdeviceapi.h>
#include <cstdint>
//#include <mmreg.h>

struct ChunkHeader
{
	uint32_t IDtag; //"RIFF"
	uint32_t chunkSize; //does not include the size of the riff header nor the padding byte at the end if data size in bytes is uneven
};

using RiffHeader = ChunkHeader;

struct WaveHeader
{
	uint32_t IDtag; //"WAVE"
};

struct FormatChunk
{
	//common fields
	uint16_t formatTag;
	uint16_t channels; //number of channels
	uint32_t samplesPerSecond; //a 'sample' is a waveform sample defined per channel
	uint32_t avgBytesPerSecond; //(sizeof sample) * samplesPerSecond * channels
	uint16_t blockAlign; // (sizeof sample) * channels

	//format specific fields
		//WAVE_FORMAT_PCM
	uint16_t bitsPerSample;
	//...
};

struct DataChunkHeader
{

};

#endif // !WAVEHEADERSTRUCTS_H
