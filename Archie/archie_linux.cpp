#include "archie_linux_aux.h"
#include <alsa/asoundlib.h>
#include <functional>
#include <stdexcept>
#include <string>

//#include <cstdio>
//#include <iostream>
//#include "ALSAconfigspaceToString.h"

namespace Archie{
enum { BLOCKING_MODE = 0 };
const char defaultDeviceName[]{"default"};

snd_pcm_t* device = nullptr;
snd_pcm_hw_params_t* hwConfig = nullptr;

unsigned int desiredPeriodTime = 500000; //i THINK this is in micro seconds
unsigned int periodTime;
WORD sampleSize;
snd_pcm_uframes_t periodSize_frames;
UINT32 periodSize_bytes;
BYTE* intermediateBuffer = nullptr; //This shouldn't be needed. A better solution can be found.

WAVEFORMATEXTENSIBLE DefaultConfig();
WAVEFORMATEXTENSIBLE configFormat(DefaultConfig());
std::function<DWORD(UINT32, BYTE*)> LoadData = [](UINT32, BYTE*){return AUDCLNT_BUFFERFLAGS_SILENT;};

void CleanUp()
{
	snd_pcm_hw_free(device);
	device = nullptr;
	snd_pcm_hw_params_free(hwConfig);
	hwConfig = nullptr;
	if (nullptr != intermediateBuffer) delete(intermediateBuffer);
}

struct throwOnError_struct
{
	const char* message;
};
inline throwOnError_struct ThrowOnError(const char* msg)
{
	return {msg};
}
inline int operator>>(int errorcode, throwOnError_struct s)
{
	if (0 > errorcode)
	{
		CleanUp();
		throw std::runtime_error("Archie: " + std::string(s.message) + "\nErrorcode: " + std::to_string(errorcode));
	}
	return errorcode;
}

WAVEFORMATEXTENSIBLE DefaultConfig()
{
	//WAVEFORMATEXTENSIBLE r;

	//r.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	//r.Format.nChannels = 2;
	//r.Format.nSamplesPerSec = 48000;
	//r.Format.nAvgBytesPerSec = 0; //placeholder value
	//r.Format.nBlockAlign = 0; //placeholder value
	//r.Format.wBitsPerSample = 32;
	//r.Format.cbSize = 22;
	//r.Samples.wValidBitsPerSample = r.Format.wBitsPerSample;
	//r.dwChannelMask = 0x3;
	//r.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
	//
	//r.Format.nBlockAlign = r.Format.nChannels * (r.Format.wBitsPerSample/8);
	//r.Format.nAvgBytesPerSec = r.Format.nAvgBytesPerSec * (r.Format.wBitsPerSample/8);
	//
	//return r;

	return SetWaveformatextensible(2, 48000, 4, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT);
}

void Configure()
{
	snd_pcm_format_t format(SND_PCM_FORMAT_UNKNOWN);

	WORD formatTag(configFormat.Format.wFormatTag);
	WORD nChannels(configFormat.Format.nChannels);
	DWORD sampleRate(configFormat.Format.nSamplesPerSec);
	int rateDir(0);
	WORD bitsPerSample(configFormat.Format.wBitsPerSample);

	if (WAVE_FORMAT_PCM == formatTag || (WAVE_FORMAT_EXTENSIBLE == formatTag && KSDATAFORMAT_SUBTYPE_PCM == configFormat.SubFormat))
	{
		switch(bitsPerSample)
		{
		case 8:
			format = SND_PCM_FORMAT_U8;
			break;
		case 16:
			format = SND_PCM_FORMAT_S16_LE;
			break;
		case 24:
			format = SND_PCM_FORMAT_S24_LE;
			break;
		case 32:
			format = SND_PCM_FORMAT_S32_LE;
			break;
		default:
			break;
		}
	}
	else if(WAVE_FORMAT_IEEE_FLOAT == formatTag || (WAVE_FORMAT_EXTENSIBLE == formatTag && KSDATAFORMAT_SUBTYPE_IEEE_FLOAT == configFormat.SubFormat) )
	{
		format = SND_PCM_FORMAT_FLOAT_LE;
	}

	snd_pcm_hw_params_set_access(device, hwConfig, SND_PCM_ACCESS_RW_INTERLEAVED) >> ThrowOnError("snd_pcm_hw_params_set_access");
	snd_pcm_hw_params_set_format(device, hwConfig, SND_PCM_FORMAT_FLOAT_LE) >> ThrowOnError("snd_pcm_hw_params_set_format");
	snd_pcm_hw_params_set_channels(device, hwConfig, nChannels) >> ThrowOnError("snd_pcm_hw_params_set_channels");
	snd_pcm_hw_params_set_rate_near(device, hwConfig, &sampleRate, &rateDir) >> ThrowOnError("snd_pcm_hw_params_set_rate");
	if (0 != rateDir) -1 >> ThrowOnError("snd_pcm_hw_params_set_rate did not return exact rate");

	periodTime = desiredPeriodTime;
	int dir;
	snd_pcm_hw_params_set_period_time_near(device, hwConfig, &periodTime, &dir) >> ThrowOnError("snd_pcm_hw_params_set_period_time_near");
	//snd_pcm_hw_params_set_buffer_time_near(device, hwConfig, &periodTime, &dir) >> ThrowOnError("snd_pcm_hw_params_set_period_time_near");
}

void GetConfiguration()
{
	snd_pcm_format_t format(SND_PCM_FORMAT_UNKNOWN);
	unsigned int channels;
	unsigned int sampleRate;
	int rateDir;
	GUID guidFormatTag;

	snd_pcm_hw_params_get_format(hwConfig, &format) >> ThrowOnError("snd_pcm_hw_params_get_format");
	switch (format)
	{
	case SND_PCM_FORMAT_U8:
		sampleSize = 1;
		guidFormatTag = KSDATAFORMAT_SUBTYPE_PCM;
		break;
	case SND_PCM_FORMAT_S16_LE:
		sampleSize = 2;
		guidFormatTag = KSDATAFORMAT_SUBTYPE_PCM;
		break;
	case SND_PCM_FORMAT_S24_LE:
		sampleSize = 3;
		guidFormatTag = KSDATAFORMAT_SUBTYPE_PCM;
		break;
	case SND_PCM_FORMAT_S32_LE:
		sampleSize = 4;
		guidFormatTag = KSDATAFORMAT_SUBTYPE_PCM;
		break;
	case SND_PCM_FORMAT_FLOAT_LE:
		sampleSize = 4;
		guidFormatTag = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
		break;
	default:
		-1 >> ThrowOnError("GetConfiguration unhandled format tag: ");
		break;
	}

	snd_pcm_hw_params_get_channels(hwConfig, &channels) >> ThrowOnError("snd_pcm_get_channels");
	snd_pcm_hw_params_get_rate(hwConfig, &sampleRate, &rateDir) >> ThrowOnError("snd_pcm_hw_params_get_rate");
	if (0 != rateDir) - 1 >> ThrowOnError("snd_pcm_hw_params_get_rate did not return exact rate");

	configFormat = SetWaveformatextensible(channels, sampleRate, sampleRate, guidFormatTag);

	int dir(0);
	snd_pcm_hw_params_get_period_size(hwConfig, &periodSize_frames, &dir) >> ThrowOnError("snd_pcm_hw_params_get_period_size");
	//snd_pcm_hw_params_get_buffer_size(hwConfig, &periodSize_frames) >> ThrowOnError("snd_pcm_hw_params_get_period_size");
	if (0 < dir) periodSize_frames -= 1;
	periodSize_bytes = periodSize_frames * sampleSize * configFormat.Format.nChannels;
	intermediateBuffer = new BYTE[2 * periodSize_bytes];
}

bool Init()
{
	snd_pcm_hw_params_malloc(&hwConfig) >> ThrowOnError("snd_pcm_hw_params_malloc");
	snd_pcm_open(&device, defaultDeviceName, SND_PCM_STREAM_PLAYBACK, BLOCKING_MODE) >> ThrowOnError("snd_pcm_open");
	snd_pcm_hw_params_any(device, hwConfig) >> ThrowOnError("snd_pcm_hw_params_any");
	Configure();
	snd_pcm_hw_params(device, hwConfig) >> ThrowOnError("snd_pcm_hw_params");
	GetConfiguration();
	//std::cerr << config2string(hwConfig);
	return true;
}

void UnInit()
{
	CleanUp();
}

void Play()
{
	DWORD flag = LoadData(2*periodSize_bytes, intermediateBuffer);
	snd_pcm_writei(device, intermediateBuffer, 2*periodSize_frames) >> ThrowOnError("snd_pcm_writei");
	while (AUDCLNT_BUFFERFLAGS_SILENT != flag)
	{	
		flag = LoadData(periodSize_bytes, intermediateBuffer);
		snd_pcm_writei(device, intermediateBuffer, periodSize_frames) >> ThrowOnError("snd_pcm_writei in loop");
	}
}

}//!Archie
