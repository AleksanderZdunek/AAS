#pragma once
#ifndef ARCHIE_LINUX_AUX_H
#define ARCHIE_LINUX_AUX_H
#include <cstdint>
#include <cstring>

typedef uint16_t WORD;
typedef uint32_t DWORD;
typedef uint32_t UINT32;
typedef unsigned char BYTE;

enum _AUDCLNT_BUFFERFLAGS
{
	AUDCLNT_BUFFERFLAGS_SILENT = 0x2
};

enum _WAVE_FORMAT_TAGS
{
	WAVE_FORMAT_UNKNOWN		= 0x0000,
	WAVE_FORMAT_PCM			= 0x0001,
	WAVE_FORMAT_IEEE_FLOAT	= 0x0003,
	WAVE_FORMAT_EXTENSIBLE	= 0xFFFE
};

struct GUID
{
	unsigned long Data1;
	unsigned short Data2;
	unsigned short Data3;
	unsigned char Data4[8];
};

const GUID KSDATAFORMAT_SUBTYPE_PCM { 0x1, 0, 0x10,{ 0x80, 0, 0, 0xaa, 0, 0x38, 0x9b, 0x71 } };
const GUID KSDATAFORMAT_SUBTYPE_IEEE_FLOAT{ 0x3, 0, 0x10,{ 0x80, 0, 0, 0xaa, 0, 0x38, 0x9b, 0x71 } };

struct WAVEFORMATEX
{
	WORD wFormatTag;
	WORD nChannels;
	DWORD nSamplesPerSec;
	DWORD nAvgBytesPerSec;
	WORD nBlockAlign;
	WORD wBitsPerSample;
	WORD cbSize;
};

struct WAVEFORMATEXTENSIBLE
{
	WAVEFORMATEX Format;
	union
	{
		WORD wValidBitsPerSample;
		WORD wSamplesPerBlock;
		WORD wReserved;
	} Samples;
	DWORD dwChannelMask;
	GUID SubFormat;
};

bool operator!=(const WAVEFORMATEX& l, const WAVEFORMATEX& r);
bool operator==(const WAVEFORMATEX& l, const WAVEFORMATEX& r);
bool operator!=(const WAVEFORMATEXTENSIBLE& l, const WAVEFORMATEXTENSIBLE& r);
bool operator==(const WAVEFORMATEXTENSIBLE& l, const WAVEFORMATEXTENSIBLE& r);
bool operator!=(const GUID& l, const GUID& r);
bool operator==(const GUID& l, const GUID& r);

/**
	If subFormat argument is KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, assumes the value of sampleSize is 4.

	@param channels Number of channels
	@param sampleRate Samples per second
	@param sampleSize Size of a sample, in bytes
	@param subFormat Sample encoding format

	@return Configured WAVEFORMATEXTENSIBLE object
*/
WAVEFORMATEXTENSIBLE SetWaveformatextensible(WORD channels, DWORD sampleRate, WORD sampleSize, const GUID subFormat);

#endif // !ARCHIE_LINUX_AUX_H