#include <alsa/asoundlib.h>
#include <string>
#include <sstream>

std::ostringstream* p_oss = nullptr; //Not thread safe. Doesn't matter for my use case.
struct errStruct
{
	const char* msg;
};
errStruct OnError(const char* msg)
{
	return errStruct{ msg };
}
int operator>>(int errorcode, errStruct err)
{
	if (0 > errorcode)
	{
		(*p_oss) << err.msg << "\n";
	}
	return errorcode;
}

std::string configspace2string(snd_pcm_hw_params_t* conf)
{
	std::ostringstream oss;
	p_oss = &oss;

	oss << std::endl << "Access" << std::endl;
	snd_pcm_access_t tmpAccess;
	snd_pcm_hw_params_get_access(conf, &tmpAccess) >> OnError("get_access error");
	oss << "access: " << tmpAccess << std::endl;

	snd_pcm_access_mask_t* ptmpAccessMask;
	snd_pcm_access_mask_alloca(&ptmpAccessMask);
	snd_pcm_hw_params_get_access_mask(conf, ptmpAccessMask) >> OnError("get_access_mask error");
	oss << "acces mask: " << "TODO!"<< std::endl;

	oss << std::endl << "Format" << std::endl;
	snd_pcm_format_t tmpFormat;
	snd_pcm_hw_params_get_format(conf, &tmpFormat) >> OnError("get_format error");
	oss << "format: " << tmpFormat << std::endl;

	snd_pcm_format_mask_t* ptmpFormatMask;
	snd_pcm_format_mask_alloca(&ptmpFormatMask);
	snd_pcm_hw_params_get_format_mask(conf, ptmpFormatMask);// >> OnError("get_format_mask error");
	oss << "format mask: " << "TODO!" << std::endl;

	snd_pcm_subformat_t tmpSubFormat;
	snd_pcm_hw_params_get_subformat(conf, &tmpSubFormat) >> OnError("get_subformat error");
	oss << "subformat: " << tmpSubFormat << std::endl;

	snd_pcm_subformat_mask_t* ptmpSubFormatMask;
	snd_pcm_subformat_mask_alloca(&ptmpSubFormatMask);
	snd_pcm_hw_params_get_subformat_mask(conf, ptmpSubFormatMask);// >> OnError("get_subformat_mask error");
	oss << "subformat mask: " << "TODO!" << std::endl;

	oss << std::endl << "Channels" << std::endl;
	unsigned int tmpVal;
	snd_pcm_hw_params_get_channels(conf, &tmpVal) >> OnError("get_channels error");
	oss << "channels: " << tmpVal << std::endl;

	snd_pcm_hw_params_get_channels_min(conf, &tmpVal) >> OnError("get_channels_min error");
	oss << "channels min: " << tmpVal << std::endl;

	snd_pcm_hw_params_get_channels_max(conf, &tmpVal) >> OnError("get_channels_max error");
	oss << "channels max: " << tmpVal << std::endl;

	oss << std::endl << "SampleRate" << std::endl;
	int tmpDir;
	snd_pcm_hw_params_get_rate(conf, &tmpVal, &tmpDir) >> OnError("get_rate error");
	oss << "Samplerate: " << tmpVal << " dir: " << tmpDir << std::endl;

	snd_pcm_hw_params_get_rate_min(conf, &tmpVal, &tmpDir) >> OnError("get_rate_min error");
	oss << "Samplerate min: " << tmpVal << " dir: " << tmpDir << std::endl;

	snd_pcm_hw_params_get_rate_max(conf, &tmpVal, &tmpDir) >> OnError("get_rate_max error");
	oss << "Samplerate max: " << tmpVal << " dir: " << tmpDir << std::endl;

	unsigned int tmpNumerator, tmpDenominator;
	snd_pcm_hw_params_get_rate_numden(conf, &tmpNumerator, &tmpDenominator) >> OnError("get_rate_numden error");
	oss << "Samplerate fractional: " << tmpNumerator << "/" << tmpDenominator << std::endl;

	oss << std::endl << "Period" << std::endl;
	snd_pcm_hw_params_get_period_time(conf, &tmpVal, &tmpDir) >> OnError("get_period_time error");
	oss << "Period time: " << tmpVal << " dir: " << tmpDir << std::endl;

	snd_pcm_hw_params_get_period_time_min(conf, &tmpVal, &tmpDir) >> OnError("get_period_time_min error");
	oss << "Period time min: " << tmpVal << " dir: " << tmpDir << std::endl;

	snd_pcm_hw_params_get_period_time_max(conf, &tmpVal, &tmpDir) >> OnError("get_period_time_max error");
	oss << "Period time max: " << tmpVal << " dir: " << tmpDir << std::endl;

	snd_pcm_uframes_t tmpFrames;
	snd_pcm_hw_params_get_period_size(conf, &tmpFrames, &tmpDir) >> OnError("get_period_size error");
	oss << "Period frames: " << tmpFrames << " dir: " << tmpDir << std::endl;

	snd_pcm_hw_params_get_period_size_min(conf, &tmpFrames, &tmpDir) >> OnError("get_period_size_min error");
	oss << "Period frames min: " << tmpFrames << " dir: " << tmpDir << std::endl;

	snd_pcm_hw_params_get_period_size_max(conf, &tmpFrames, &tmpDir) >> OnError("get_period_size_max error");
	oss << "Period frames max: " << tmpFrames << " dir: " << tmpDir << std::endl;

	snd_pcm_hw_params_get_periods(conf, &tmpVal, &tmpDir) >> OnError("get_periods error");
	oss << "Periods count: " << tmpVal << " dir: " << tmpDir << std::endl;

	snd_pcm_hw_params_get_periods_min(conf, &tmpVal, &tmpDir) >> OnError("get_periods_min error");
	oss << "Periods count min: " << tmpVal << " dir: " << tmpDir << std::endl;

	snd_pcm_hw_params_get_periods_max(conf, &tmpVal, &tmpDir) >> OnError("get_periods_max error");
	oss << "Periods count max: " << tmpVal << " dir: " << tmpDir << std::endl;

	oss << std::endl << "Buffer" << std::endl;
	snd_pcm_hw_params_get_buffer_time(conf, &tmpVal, &tmpDir) >> OnError("get_buffer_time error");
	oss << "Buffer time: " << tmpVal << " dir: " << tmpDir << std::endl;

	snd_pcm_hw_params_get_buffer_time_min(conf, &tmpVal, &tmpDir) >> OnError("get_buffer_time_min error");
	oss << "Buffer time min: " << tmpVal << " dir: " << tmpDir << std::endl;

	snd_pcm_hw_params_get_buffer_time_max(conf, &tmpVal, &tmpDir) >> OnError("get_buffer_time_max error");
	oss << "Buffer time max: " << tmpVal << " dir: " << tmpDir << std::endl;

	snd_pcm_hw_params_get_buffer_size(conf, &tmpFrames) >> OnError("get_buffer_size error");
	oss << "Buffer frames: " << tmpFrames << std::endl;

	snd_pcm_hw_params_get_buffer_size_min(conf, &tmpFrames) >> OnError("get_buffer_size_min error");
	oss << "Buffer frames min: " << tmpFrames << std::endl;

	snd_pcm_hw_params_get_buffer_size_max(conf, &tmpFrames) >> OnError("get_buffer_size_max error");
	oss << "Buffer frames max: " << tmpFrames << std::endl;

	p_oss = nullptr;
	return oss.str();
}

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
