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

#endif // !WAVEHEADERSTRUCTS_H
