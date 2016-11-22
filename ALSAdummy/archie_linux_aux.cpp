#include "archie_linux_aux.h"

bool operator!=(const WAVEFORMATEX& l, const WAVEFORMATEX& r)
{
	return  0 != memcmp(&l, &r, sizeof(WAVEFORMATEX));
}

bool operator==(const WAVEFORMATEX& l, const WAVEFORMATEX& r)
{
	return 0 == memcmp(&l, &r, sizeof(WAVEFORMATEX));
}

bool operator!=(const WAVEFORMATEXTENSIBLE& l, const WAVEFORMATEXTENSIBLE& r)
{
	return 0 != memcmp(&l, &r, sizeof(WAVEFORMATEXTENSIBLE));
}

bool operator==(const WAVEFORMATEXTENSIBLE& l, const WAVEFORMATEXTENSIBLE& r)
{
	return 0 == memcmp(&l, &r, sizeof(WAVEFORMATEXTENSIBLE));
}

bool operator!=(const GUID& l, const GUID& r)
{
	return 0 != memcmp(&l, &r, sizeof(GUID));
}

bool operator==(const GUID& l, const GUID& r)
{
	return 0 == memcmp(&l, &r, sizeof(GUID));
}

WAVEFORMATEXTENSIBLE SetWaveformatextensible(WORD channels, DWORD sampleRate, WORD sampleSize, const GUID subFormat)
{
	if (KSDATAFORMAT_SUBTYPE_IEEE_FLOAT == subFormat) sampleSize = 4;

	WAVEFORMATEXTENSIBLE r;
	r.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	r.Format.nChannels = channels;
	r.Format.nSamplesPerSec = sampleRate;
	r.Format.nAvgBytesPerSec = sampleSize * r.Format.nSamplesPerSec;
	r.Format.nBlockAlign = r.Format.nChannels * sampleSize;
	r.Format.wBitsPerSample = sampleSize * 8;
	r.Format.cbSize = 22;
	r.Samples.wValidBitsPerSample = r.Format.wBitsPerSample;
	r.dwChannelMask = 0x3;
	r.SubFormat = subFormat;

	return r;
}