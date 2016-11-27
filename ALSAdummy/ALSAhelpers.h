#pragma once
#include <alsa/asoundlib.h>
#include <string>
#include <sstream>

std::string config2string(snd_pcm_hw_params_t* conf)
{
	std::ostringstream oss;
	p_oss = &oss;

	oss << std::endl << "Access" << std::endl;
	snd_pcm_access_t tmpAccess;
	snd_pcm_hw_params_get_access(conf, &tmpAccess) >> OnError("get_access error");
	oss << "access: " << tmpAccess << std::endl;

	oss << std::endl << "Format" << std::endl;
	snd_pcm_format_t tmpFormat;
	snd_pcm_hw_params_get_format(conf, &tmpFormat) >> OnError("get_format error");
	oss << "format: " << tmpFormat << std::endl;

	snd_pcm_subformat_t tmpSubFormat;
	snd_pcm_hw_params_get_subformat(conf, &tmpSubFormat) >> OnError("get_subformat error");
	oss << "subformat: " << tmpSubFormat << std::endl;

	oss << std::endl << "Channels" << std::endl;
	unsigned int tmpVal;
	snd_pcm_hw_params_get_channels(conf, &tmpVal) >> OnError("get_channels error");
	oss << "channels: " << tmpVal << std::endl;

	oss << std::endl << "SampleRate" << std::endl;
	int tmpDir;
	snd_pcm_hw_params_get_rate(conf, &tmpVal, &tmpDir) >> OnError("get_rate error");
	oss << "Samplerate: " << tmpVal << " dir: " << tmpDir << std::endl;

	unsigned int tmpNumerator, tmpDenominator;
	snd_pcm_hw_params_get_rate_numden(conf, &tmpNumerator, &tmpDenominator) >> OnError("get_rate_numden error");
	oss << "Samplerate fractional: " << tmpNumerator << "/" << tmpDenominator << std::endl;

	oss << std::endl << "Period" << std::endl;
	snd_pcm_hw_params_get_period_time(conf, &tmpVal, &tmpDir) >> OnError("get_period_time error");
	oss << "Period time: " << tmpVal << " dir: " << tmpDir << std::endl;

	snd_pcm_uframes_t tmpFrames;
	snd_pcm_hw_params_get_period_size(conf, &tmpFrames, &tmpDir) >> OnError("get_period_size error");
	oss << "Period frames: " << tmpFrames << " dir: " << tmpDir << std::endl;

	snd_pcm_hw_params_get_periods(conf, &tmpVal, &tmpDir) >> OnError("get_periods error");
	oss << "Periods count: " << tmpVal << " dir: " << tmpDir << std::endl;

	oss << std::endl << "Buffer" << std::endl;
	snd_pcm_hw_params_get_buffer_time(conf, &tmpVal, &tmpDir) >> OnError("get_buffer_time error");
	oss << "Buffer time: " << tmpVal << " dir: " << tmpDir << std::endl;

	snd_pcm_hw_params_get_buffer_size(conf, &tmpFrames) >> OnError("get_buffer_size error");
	oss << "Buffer frames: " << tmpFrames << std::endl;

	p_oss = nullptr;
	return oss.str();
}
