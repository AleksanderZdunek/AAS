#include <Audioclient.h>
#include <mmdeviceapi.h>
#include <functional>
#include "waveformatexhelper.h"
#include <iostream>

#define REFTIMES_PER_SEC 10000000 //One REFTIME is 100 nanoseconds. Or one tenth of a microsecond. Or 10^-7 seconds.
#define REFTIMES_PER_MILLISEC 10000

namespace Archie{
static const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
static const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
static const IID IID_IAudioClient = __uuidof(IAudioClient);
static const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

IMMDeviceEnumerator* deviceEnumerator = nullptr;
IMMDevice* audioDevice = nullptr;
IAudioClient* audioClient = nullptr;
IAudioRenderClient *renderClient = nullptr;

REFERENCE_TIME audioClientBufferTentativeDuration = 1 * REFTIMES_PER_SEC;
static REFERENCE_TIME audioClientBufferDuration;
static UINT32 audioClientBufferSize = 0;
static UINT32& audioClientBufferFrameCount = audioClientBufferSize;
WAVEFORMATEXTENSIBLE configFormat {0};
WAVEFORMATEXTENSIBLE* closestSupportedFormat(nullptr);

std::function<DWORD(UINT32, BYTE*)> LoadData = [](UINT32, BYTE*) {return AUDCLNT_BUFFERFLAGS_SILENT;};

#define SAFE_RELEASE(punk) if ((punk) != NULL) { (punk)->Release(); (punk) = NULL; }
void CleanUp()
{
	CoUninitialize();
	SAFE_RELEASE(deviceEnumerator);
	SAFE_RELEASE(audioDevice);
	SAFE_RELEASE(audioClient);
	SAFE_RELEASE(renderClient);
}

inline void ThrowOnError(HRESULT hr, char* s)
{
	if (FAILED(hr))
	{
		CleanUp();
		throw std::runtime_error(s);
	}
};

void Init()
{
	HRESULT errorcode;

	//Init Step 1. Initialize COM (Component Object Model)
	errorcode = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED); ThrowOnError(errorcode, "Archie::Init::CoInitializeEx");

	//Init Step 2. Create Device Enumerator
	errorcode = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&deviceEnumerator); ThrowOnError(errorcode, "Archie::Init CoCreateInstance");

	//Init Step 3. Get audio endpoint
	errorcode = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &audioDevice); ThrowOnError(errorcode, "Archie::Init GetDefaultAudioEndpoint");

	//Init Step 4. Activate audio client
	errorcode = audioDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&audioClient); ThrowOnError(errorcode, "Archie::Init Activate");

	//Rest if init in Init2()
}

WAVEFORMATEXTENSIBLE GetMixerFormat()
{
	WAVEFORMATEXTENSIBLE r{0};
	WAVEFORMATEXTENSIBLE* p = nullptr;
	HRESULT errorcode = audioClient->GetMixFormat((WAVEFORMATEX**)&p); ThrowOnError(errorcode, "Archie::GetMixerFormat");
	if (p) r = *p;
	CoTaskMemFree(p);
	return r;
}

bool Test()
{
	HRESULT errorcode = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0x88000000, audioClientBufferTentativeDuration, 0, &configFormat.Format, NULL); ThrowOnError(errorcode, "Archie::Test Initialize");
	return FAILED(errorcode);
}

bool IsFormatSupported(const WAVEFORMATEX* f)
{
	WAVEFORMATEX* closestMatch;
	HRESULT errorcode = audioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, f,	&closestMatch);
	CoTaskMemFree(closestMatch);
	return !(FAILED(errorcode) || S_FALSE == errorcode);
}
bool IsFormatSupported(const WAVEFORMATEX& f)
{
	return IsFormatSupported(&f);
}
bool IsFormatSupported(const WAVEFORMATEXTENSIBLE& f)
{
	return IsFormatSupported((WAVEFORMATEX*)&f);
}

void Init2()
{
	HRESULT errorcode;
	//Init Step 5. Check format support
	/*
	HRESULT errorcode = audioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, &configFormat.Format, reinterpret_cast<WAVEFORMATEX**>(&closestSupportedFormat));
	if (S_FALSE == errorcode) //See for explanation: https://msdn.microsoft.com/en-us/library/windows/desktop/dd370876(v=vs.85).aspx
	{
		CleanUp();
		throw std::runtime_error("Archie::Init2 IsFormatSupported + S_FALSE");
	}
	ThrowOnError(errorcode, "Archie::Init2 IsFormatSupported");
	*/

	//Init Step 6. Initialize audio stream
	errorcode = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, audioClientBufferTentativeDuration, 0, &configFormat.Format, NULL); ThrowOnError(errorcode, "Archie::Init2 Initialize");

	//Init Step 6.1. Get the actual size of the allocated buffer.
	errorcode = audioClient->GetBufferSize(&audioClientBufferSize); ThrowOnError(errorcode, "Archie::Init2 GetBufferSize");
	UINT audioFrameSize = configFormat.Format.nChannels * configFormat.Format.wBitsPerSample / 8;
	audioClientBufferFrameCount = audioClientBufferSize / audioFrameSize;
	audioClientBufferDuration = (REFERENCE_TIME)((double)REFTIMES_PER_SEC * audioClientBufferFrameCount / configFormat.Format.nSamplesPerSec);

	//Init Step 7. Get access to the rendering interface of the audio client.
	errorcode = audioClient->GetService(IID_IAudioRenderClient, (void**)&renderClient); ThrowOnError(errorcode, "Archie::Init2 GetService");

	//~init finished
}

void Play()
{
	Init2();

	HRESULT errorcode;

	DWORD flag = 0;
	BYTE* data_pointer;
	UINT32 numPaddingFrames = 0;
	UINT32 numAvailableFrames = 0;

	//Preload render buffer
	errorcode = renderClient->GetBuffer(audioClientBufferFrameCount, &data_pointer); ThrowOnError(errorcode, "Archie::Play GetBuffer on preload");
	DWORD flags = LoadData(audioClientBufferFrameCount*configFormat.Format.nBlockAlign, data_pointer);
	errorcode = renderClient->ReleaseBuffer(audioClientBufferFrameCount, flag); ThrowOnError(errorcode, "Archie::Play ReleaseBuffer on preload");

	//Start playing
	errorcode = audioClient->Start(); ThrowOnError(errorcode, "Archie::Play Start");
	while (flag != AUDCLNT_BUFFERFLAGS_SILENT)
	{
		//Sleep for half of the buffer duration
		Sleep((DWORD)( audioClientBufferDuration/REFTIMES_PER_MILLISEC / 2 ));

		errorcode = audioClient->GetCurrentPadding(&numPaddingFrames); ThrowOnError(errorcode, "Archie::Play GetCurrentPadding in loop");
		numAvailableFrames = audioClientBufferFrameCount - numPaddingFrames;

		errorcode = renderClient->GetBuffer(numAvailableFrames, &data_pointer); ThrowOnError(errorcode, "Archie::Play GetBuffer in loop");
		flag = LoadData(numAvailableFrames*configFormat.Format.nBlockAlign, data_pointer);
		errorcode = renderClient->ReleaseBuffer(numAvailableFrames, flag); ThrowOnError(errorcode, "Archie::Play ReleaseBuffer in loop");
	}
	Sleep((DWORD)(audioClientBufferDuration / REFTIMES_PER_MILLISEC / 2)); //Wait for last data to play before stopping
	errorcode = audioClient->Stop(); ThrowOnError(errorcode, "Archie::Play Stop");

	//release aquired resources
	SAFE_RELEASE(renderClient);
}

}//!Archie