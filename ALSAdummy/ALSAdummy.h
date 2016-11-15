#pragma once
#ifndef ALSADUMMY_H
#define ALSADUMMY_H

#define snd_pcm_hw_params_alloca(ptr)
#define strdup(lit) lit

typedef long snd_pcm_sframes_t;
typedef unsigned long snd_pcm_uframes_t;

typedef struct _snd_pcm {} snd_pcm_t;
typedef struct _snd_pcm_hw_params {} snd_pcm_hw_params_t;

enum snd_pcm_stream_t
{
	SND_PCM_STREAM_PLAYBACK,
	SND_PCM_STREAM_CAPTURE
};

enum snd_pcm_format_t
{
	SND_PCM_FORMAT_UNKNOWN,
	SND_PCM_FORMAT_S8,
	SND_PCM_FORMAT_U8,
	SND_PCM_FORMAT_S16_LE,
	SND_PCM_FORMAT_S16_BE,
	SND_PCM_FORMAT_U16_LE,
	SND_PCM_FORMAT_U16_BE,
	SND_PCM_FORMAT_S24_LE,
	SND_PCM_FORMAT_S24_BE,
	SND_PCM_FORMAT_U24_LE,
	SND_PCM_FORMAT_U24_BE,
	SND_PCM_FORMAT_S32_LE,
	SND_PCM_FORMAT_S32_BE,
	SND_PCM_FORMAT_U32_LE,
	SND_PCM_FORMAT_U32_BE,
	SND_PCM_FORMAT_FLOAT_LE,
	SND_PCM_FORMAT_FLOAT_BE,
	SND_PCM_FORMAT_FLOAT64_LE,
	SND_PCM_FORMAT_FLOAT64_BE,
	SND_PCM_FORMAT_IEC958_SUBFRAME_LE,
	SND_PCM_FORMAT_IEC958_SUBFRAME_BE,
	SND_PCM_FORMAT_MU_LAW,
	SND_PCM_FORMAT_A_LAW,
	SND_PCM_FORMAT_IMA_ADPCM,
	SND_PCM_FORMAT_MPEG,
	SND_PCM_FORMAT_GSM,
	SND_PCM_FORMAT_SPECIAL,
	SND_PCM_FORMAT_S24_3LE,
	SND_PCM_FORMAT_S24_3BE,
	SND_PCM_FORMAT_U24_3LE,
	SND_PCM_FORMAT_U24_3BE,
	SND_PCM_FORMAT_S20_3LE,
	SND_PCM_FORMAT_S20_3BE,
	SND_PCM_FORMAT_U20_3LE,
	SND_PCM_FORMAT_U20_3BE,
	SND_PCM_FORMAT_S18_3LE,
	SND_PCM_FORMAT_S18_3BE,
	SND_PCM_FORMAT_U18_3LE,
	SND_PCM_FORMAT_U18_3BE,
	SND_PCM_FORMAT_S16,
	SND_PCM_FORMAT_U16,
	SND_PCM_FORMAT_S24,
	SND_PCM_FORMAT_U24,
	SND_PCM_FORMAT_S32,
	SND_PCM_FORMAT_U32,
	SND_PCM_FORMAT_FLOAT,
	SND_PCM_FORMAT_FLOAT64,
	SND_PCM_FORMAT_IEC958_SUBFRAME
};

enum snd_pcm_access_t
{
	SND_PCM_ACCESS_MMAP_INTERLEAVED,
	SND_PCM_ACCESS_MMAP_NONINTERLEAVED,
	SND_PCM_ACCESS_MMAP_COMPLEX,
	SND_PCM_ACCESS_RW_INTERLEAVED,
	SND_PCM_ACCESS_RW_NONINTERLEAVED
};

/**
	Write interleaved franes to a PCM. (What's a 'PCM'?)

	@param pcm PCM handle
	@param buffer frames containing buffer
	@param size frames to be written

	@return a positive number of frames actually written otherwise a negative error code 
	
	@retval -EBADFD PCM is not in the right state (SND_PCM_STATE_PREPARED or SND_PCM_STATE_RUNNING) 
	@retval -EPIPE	an underrun occurred
	@retval -ESTRPIPE a suspend event occurred (stream is suspended and waiting for an application recovery)

	If the blocking behaviour is selected and it is running, then routine waits until all requested frames are played or put to the playback ring buffer. The returned number of frames can be less only if a signal or underrun occurred.

	If the non-blocking behaviour is selected, then routine doesn't wait at all.
*/
snd_pcm_sframes_t snd_pcm_writei(snd_pcm_t* pcm, const void* buffer, snd_pcm_uframes_t size) {}

/**
	Prepare PCM for use.
	
	@param pcm PCM handle

	@return 0 on success, otherwise a negative error code
*/
int snd_pcm_prepare(snd_pcm_t* pcm) {}

/**
	Install one PCM hardware configuration chosen from a configuration space, and snd_pcm_prepare it.

	@param pcm PCM handle
	@param params Configuration space definition container

	@return 0 on success, otherwise a negative error code

	The configuration is chosen fixing single parameters in this order: first access, first format, first subformat, min channels, min rate, min period time, max buffer size, min tick time. If no mutually compatible set of parameters can be chosen, a negative error code will be returned.

	After this call, snd_pcm_prepare() is called automatically and the stream is brought to SND_PCM_STATE_PREPARED state. The hardware parameters cannot be changed when the stream is running (active). The software parameters can be changed at any time.
	The configuration soace will be updated to reflect the chosen parameters.
*/
int snd_pcm_hw_params(snd_pcm_t* pcm, snd_pcm_hw_params_t* params) {}

/**
	Restrict a configuration space to contain only one buffer size.

	@param pcm PCM handle
	@param params Configuratio space
	@param buffer size in frames

	@return 0 otherwise a negative error code if configuration space would become empty
*/
int snd_pcm_hw_params_set_buffer_size(snd_pcm_t* pcm, snd_pcm_hw_params_t* params, snd_pcm_uframes_t val) {}

/**
	Restric  configuration space to contain only one periods count.
	 
	 @param pcm PCM handle
	 @param params Configuration space
	 @param val approximate periods per buffer
	 @param dir Sub unit direction

	 @return 0 otherwise a negative error code if configuration space would become empty

	 Wanted exact value is <,=,> val following dir (-1,0,1)
*/
int snd_pcm_hw_params_set_periods(snd_pcm_t* pcm, snd_pcm_hw_params_t* params, unsigned int val, int dir) {}

/**
	Restrict a configuration space to contain one channels count.

	@param pcm PCM handle
	@param params Configuration space
	@param val channels count

	@return 0, otherwis a negative error code if configuration space would become empty
*/
int snd_pcm_hw_params_set_channels(snd_pcm_t* pcm, snd_pcm_hw_params_t* params, unsigned int val) {}

/**
	Restrict a configuration space to have rate nearest to a target.
	
	@param pcm PCM handle
	@param params Configuration space
	@param [in/out] val approximate target rate / returned approximate set rate
	@param dir Sub unit direction

	@return 0, otherwise a negative error code if configuration space is empty

	target/chosen exact value is <,=,> val following dir (-1,0,1)
*/
int snd_pcm_hw_params_set_rate_near(snd_pcm_t* pcm, snd_pcm_hw_params_t* params, unsigned int* val, int dir) {}

/**
	Restrict a configuration space to contain only one format.

	@param pcm PCM handle
	@param params Configuration space
	@param format

	@return 0, otherwise a negative error code
*/
int snd_pcm_hw_params_set_format(snd_pcm_t* pcm, snd_pcm_hw_params_t* params, snd_pcm_format_t format) {}

/**
	Restrict a configuration space to contain only one acess type.

	@param PCM handle
	@param Configuration space
	@param access type

	@return 0, otherwise a negative error code if configuration space would become empty
*/
int snd_pcm_hw_params_set_access(snd_pcm_t* pcm, snd_pcm_hw_params_t* params, snd_pcm_access_t access) {}

/**
	Fill params with a ful configuration space for a PCM.
	
	@param pcm PCM handle
	@param params Configuration space

	The configuration space will be filled with all possibe ranges for the PCM device.
*/
int snd_pcm_hw_params_any(snd_pcm_t* pcm, snd_pcm_hw_params_t* params) {}

/**
	Opens a PCM.

	@param pcmp return a PCM handle
	@param name ASCII identifier of the PCM handle
	@param stream Wanted stream
	@param mode Open mode

	@return 0 on success, otherwise a negative error code
*/
int snd_pcm_open(snd_pcm_t** pcmp, const char* name, snd_pcm_stream_t stream, int mode) {}

#endif // !ALSADUMMY_H
