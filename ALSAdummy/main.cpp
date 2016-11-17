#include <iostream>
#include <cstdio>
//#include <alsa/asoundlib.h>
#include "ALSAconfigspaceToString.cpp"

int main(int argc, char* argv[])
{
	const int BLOCKING_MODE = 0;
	char defaultName[] = "default";

	std::cout << "ALSA configspace explorer!" << std::endl;

	snd_pcm_t* device;
	snd_pcm_hw_params_t* hwparams = nullptr;
	char* deviceName = defaultName;
	if (1 < argc) deviceName = argv[1];
	snd_pcm_hw_params_alloca(&hwparams);

	if (0 > snd_pcm_open(&device, deviceName, SND_PCM_STREAM_PLAYBACK, BLOCKING_MODE))
	{
		fprintf(stderr, "Error opening PCM device %s\n", deviceName);
		return -1;
	}

	if (0 > snd_pcm_hw_params_any(device, hwparams))
	{
		fprintf(stderr, "Can not configure this PCM device.\n");
		return -1;
	}

	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
	std::cout << "hwparams any config space" << std::endl;
	std::cout << configspace2string(hwparams);

	if (0 > snd_pcm_hw_params(device, hwparams))
	{
		fprintf(stderr, "Error setting HW params.\n");
		return(-1);
	}

	std::cout << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";
	std::cout << "hwparams single config" << std::endl;
	std::cout << configspace2string(hwparams);

	return 0;
}