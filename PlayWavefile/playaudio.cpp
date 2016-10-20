#include <Audioclient.h>
#include <mmdeviceapi.h>
#include <functional>
#include "waveformatexhelper.h"


#define REFTIMES_PER_SEC 10000000 //One REFTIME is 100 nanoseconds. Or one tenth of a microsecond. Or 10^-7 seconds.
#define REFTIMES_PER_MILLISEC 10000

//namespace Archie
//{
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
WAVEFORMATEXTENSIBLE formatConfig {0};
WAVEFORMATEXTENSIBLE* closestSupportedFormat;

std::function<DWORD(UINT32, BYTE*)> LoadData;
//std::function< unsigned long (unsigned int, char*) > LoadData;
//HRESULT LoadData(UINT32 bufferFrameCount, BYTE *pData, DWORD *pFlags);

#include <iostream>
void Init()
{
	HRESULT errorcode;

	//Step 1. Initialize COM (Component Object Model)
	errorcode = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

	//Step 2. Create Device Enumerator
	errorcode = CoCreateInstance(CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&deviceEnumerator);

	//Step 3. Get audio endpoint
	errorcode = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &audioDevice);

	//Step 4. Activate audio client
	errorcode = audioDevice->Activate(IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&audioClient);

	//Step 5. Check format support
	errorcode = audioClient->IsFormatSupported(AUDCLNT_SHAREMODE_SHARED, &formatConfig.Format, reinterpret_cast<WAVEFORMATEX**>(&closestSupportedFormat));
	if (FAILED(errorcode))
	{
		std::cerr << std::endl << "Format Not Supported!" << std::endl;
	}
	else if (S_FALSE == errorcode)
	{
		std::cerr << std::endl << "Format Not Quite Supported!" << std::endl;
		std::cerr << "Closest supported format: " << std::endl;
		printWAVEFORMATEXTENSIBLE(*closestSupportedFormat);
		CoTaskMemFree(closestSupportedFormat);
		system("pause");
	}
	else
	{
		std::cerr << "Format Supported! " << std::endl;
	}
	//TODO: this errorcode really should be handled

	//Step 6. Initialize audio stream
	errorcode = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0, audioClientBufferTentativeDuration, 0, &formatConfig.Format, NULL);
	//errorcode = audioClient->Initialize(AUDCLNT_SHAREMODE_SHARED, 0x80000000, audioClientBufferTentativeDuration, 0, &formatConfig.Format, NULL);

	//Step 6.1. Get the actual size of the allocated buffer.
	errorcode = audioClient->GetBufferSize(&audioClientBufferSize);
	UINT audioFrameSize = formatConfig.Format.nChannels * formatConfig.Format.wBitsPerSample / 8;
	audioClientBufferFrameCount = audioClientBufferSize / audioFrameSize;
	audioClientBufferDuration = (REFERENCE_TIME)((double)REFTIMES_PER_SEC * audioClientBufferFrameCount / formatConfig.Format.nSamplesPerSec);

	//Step 7. Get access to the rendering interface of the audio client.
	errorcode = audioClient->GetService(IID_IAudioRenderClient, (void**)&renderClient);

	//~init finished
}

#define SAFE_RELEASE(punk) if ((punk) != NULL) { (punk)->Release(); (punk) = NULL; }
void CleanUp()
{
	CoUninitialize();
	SAFE_RELEASE(deviceEnumerator);
	SAFE_RELEASE(audioDevice);
	SAFE_RELEASE(audioClient);
	SAFE_RELEASE(renderClient);
}

void Play()
{
	HRESULT errorcode;

	DWORD flag = 0;
	BYTE* data_pointer;
	UINT32 numPaddingFrames = 0;
	UINT32 numAvailableFrames = 0;

	//Preload render buffer
	errorcode = renderClient->GetBuffer(audioClientBufferFrameCount, &data_pointer);
	DWORD flags = LoadData(audioClientBufferFrameCount*formatConfig.Format.nBlockAlign, data_pointer);
	errorcode = renderClient->ReleaseBuffer(audioClientBufferFrameCount, flag);

	//Start playing
	errorcode = audioClient->Start();
	while (flag != AUDCLNT_BUFFERFLAGS_SILENT)
	{
		//Sleep for half of the buffer duration
		Sleep((DWORD)( audioClientBufferDuration/REFTIMES_PER_MILLISEC / 2 ));

		errorcode = audioClient->GetCurrentPadding(&numPaddingFrames);
		numAvailableFrames = audioClientBufferFrameCount - numPaddingFrames;

		errorcode = renderClient->GetBuffer(numAvailableFrames, &data_pointer);
		flag = LoadData(numAvailableFrames*formatConfig.Format.nBlockAlign, data_pointer);
		errorcode = renderClient->ReleaseBuffer(numAvailableFrames, flag);
	}
	Sleep((DWORD)(audioClientBufferDuration / REFTIMES_PER_MILLISEC / 2)); //Wait for last data to play before stopping
	errorcode = audioClient->Stop();

	CleanUp();
}

//}