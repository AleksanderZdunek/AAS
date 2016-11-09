//MMF wrapper ( Microsoft Media Foundation )
//based on https://msdn.microsoft.com/en-us/library/windows/desktop/ms703190(v=vs.85).aspx
#include <mfapi.h>
#include <mfidl.h>

#include <comdef.h>
#include <propvarutil.h>
#include <strsafe.h>

#include <stdexcept>
#include <functional>

IMFMediaSession* pMediaSession = nullptr;
IMFSourceResolver* pSourceResolver = nullptr;
MF_OBJECT_TYPE mediaSourceObjectType;
IUnknown* pMediaSourceObject = nullptr;
IMFMediaSource* pMediaSource = nullptr;
IMFTopology* pTopology = nullptr;
IMFPresentationDescriptor* pPresentationDescriptor = nullptr;
DWORD numberOfSourceStreams = 0;

std::function<void(std::wstring)> metadataCallback = [](std::wstring _) {};

#define SAFE_RELEASE(punk) if ((punk) != NULL) { (punk)->Release(); (punk) = NULL; }
void CleanUp()
{
	//TODO
	SAFE_RELEASE(pPresentationDescriptor);
	SAFE_RELEASE(pTopology);
	SAFE_RELEASE(pMediaSource);
	SAFE_RELEASE(pMediaSourceObject);
	SAFE_RELEASE(pSourceResolver);
	SAFE_RELEASE(pMediaSession);
}
inline void ThrowOnError(HRESULT hr, char* s)
{
	if (FAILED(hr))
	{
		CleanUp();
		throw std::runtime_error(s);
	}
};
inline void ThrowOnError(HRESULT hr, char* s, std::function<void()> cleanUpFunction)
{
	if (FAILED(hr))
	{
		cleanUpFunction();
		throw std::runtime_error(s);
	}
}
struct throwOnError_struct
{
	char* message;
	std::function<void()> cleanUpFunction;
};
inline throwOnError_struct ThrowOnError(char *s, std::function<void()> cleanUpFunction = [](){})
{
	return{s, cleanUpFunction };
}
inline HRESULT operator>>(HRESULT hr, throwOnError_struct s)
{
	if (FAILED(hr))
	{
		s.cleanUpFunction();
		_com_error err(hr);
		throw std::runtime_error(std::string(s.message) +"\nErrorcode: " + err.ErrorMessage());
	}
	return hr;
}

const wchar_t* fileName; //LPCWSTR fileName;

//function prototype. clean this up later
void helper_AddBranchToPartialTopology(IMFTopology *pTopology_param, IMFMediaSource* pMediaSource_param, IMFPresentationDescriptor* pPresentationDescriptor_param, DWORD streamIndex_param);
void Shutdown();

void Init()
{
	HRESULT errorcode;

	//1. Initialize the Media Foundation platform.
	errorcode = MFStartup(MF_VERSION, MFSTARTUP_LITE); ThrowOnError(errorcode, "MMFwrapper MFStartup");

	//2. Create media session instance.
	errorcode = MFCreateMediaSession(NULL, &pMediaSession); ThrowOnError(errorcode, "MMFwrapper MFCreateMediaSession");

	//3. Create media source. Use Source Resolver?
	errorcode = MFCreateSourceResolver(&pSourceResolver); ThrowOnError(errorcode, "MMFwrapper MFCreateSourceResolver");
	errorcode = pSourceResolver->CreateObjectFromURL(fileName, MF_RESOLUTION_MEDIASOURCE, nullptr, &mediaSourceObjectType, &pMediaSourceObject); ThrowOnError(errorcode, "MMFwrapper MFCreateObjectFromURL");
	errorcode = pMediaSourceObject->QueryInterface(IID_PPV_ARGS(&pMediaSource)); ThrowOnError(errorcode, "MMFwrapper QueryInterface");

	//4. Create playback topology.
	errorcode = MFCreateTopology(&pTopology); ThrowOnError(errorcode, "MMFwrapper MFCreateTopology");
	errorcode = pMediaSource->CreatePresentationDescriptor(&pPresentationDescriptor); ThrowOnError(errorcode, "MMFwrapper CreatePresentationDescriptor");
	DWORD count_SourceStreams = 0;
	pPresentationDescriptor->GetStreamDescriptorCount(&count_SourceStreams); ThrowOnError(errorcode, "MMFwrapper GetStreamDescriptorCount");
	try{
		for (DWORD i = 0; i < count_SourceStreams; i++)
		{
			helper_AddBranchToPartialTopology(pTopology, pMediaSource, pPresentationDescriptor, i);
		}
	}
	catch (std::runtime_error e)
	{
		CleanUp();
		throw e;
	}

	//5. Set the topology on the Media Session.
	pMediaSession->SetTopology(0, pTopology);

	//6. Get events from the Media Session
	//do this in Play() instead
	//IMFMediaEvent* pEvent = nullptr;
	//pMediaSession->GetEvent(0, &pEvent) >> ThrowOnError("MMFwrapper Init get set topology event", [&]{SAFE_RELEASE(pEvent); CleanUp();}); //The method blocks until the event generator queues an event.
}

void Metadata()
{
	IMFMetadataProvider* pMetadataProvider = nullptr;
	IMFMetadata* pMetadata = nullptr;

	auto cleanUp_internal = [&]
	{
		SAFE_RELEASE(pMetadata);
		SAFE_RELEASE(pMetadataProvider);
	};

	MFGetService(pMediaSource, MF_METADATA_PROVIDER_SERVICE, IID_PPV_ARGS(&pMetadataProvider)) >> ThrowOnError("MMFwrapper Get metadata service", cleanUp_internal);
	pMetadataProvider->GetMFMetadata(pPresentationDescriptor, 0, 0, &pMetadata) >> ThrowOnError("MMFwrapper Get metadata", cleanUp_internal);
	
	std::wstring metadataString(L"");
	PROPVARIANT varNames;
	pMetadata->GetAllPropertyNames(&varNames) >> ThrowOnError("MMFwrapper Metadata get all property names", cleanUp_internal);
	for (ULONG i = 0; i < varNames.calpwstr.cElems; i++)
	{
		(metadataString += varNames.calpwstr.pElems[i]) += L": ";
		PROPVARIANT varValue;
		pMetadata->GetProperty(varNames.calpwstr.pElems[i], &varValue) >> ThrowOnError("MMFwrapper Metadata GetProperty");
		std::wstring stringBuffer;
		stringBuffer.resize(128);
		HRESULT errorcode = PropVariantToString(varValue, &stringBuffer[0], stringBuffer.capacity());
		if (STRSAFE_E_INSUFFICIENT_BUFFER == errorcode) stringBuffer += L"...";
		else errorcode >> ThrowOnError("MMFwrapper Metadata PropVariantToString", cleanUp_internal);
		(metadataString += stringBuffer.c_str()) += L"\n";
		PropVariantClear(&varValue);
	}
	PropVariantClear(&varNames);
	metadataCallback(metadataString);
	return cleanUp_internal();
}

void Play(const wchar_t* filename)
{
	bool playbackEnded(false);

	fileName = filename;
	Init();
	Metadata();
	PROPVARIANT varStart;
	PropVariantInit(&varStart); //zeroes the PROPVARIANT union
	pMediaSession->Start(&GUID_NULL, &varStart);
	PropVariantClear(&varStart); //unnecesary in this particular case

	//Get events
	IMFMediaEvent* pEvent = nullptr;
	while (!playbackEnded)
	{
		pMediaSession->GetEvent(0, &pEvent) >> ThrowOnError("MMFwrapper Play GetEvent", [&] {SAFE_RELEASE(pEvent); CleanUp(); });
		MediaEventType eventType = MEUnknown;
		pEvent->GetType(&eventType) >> ThrowOnError("MMFwrapper Play Get event type", [&] {SAFE_RELEASE(pEvent); CleanUp(); });
		if (MESessionEnded == eventType) playbackEnded = true;
		//else std::cerr << "some other event received\n";
		SAFE_RELEASE(pEvent);
	}	

	return Shutdown();
}

void Shutdown()
{
	pMediaSession->Close() >> ThrowOnError("MMFwrapper Shutdown media session close", CleanUp);
	bool mediaSessionClosed{ false };
	while (!mediaSessionClosed)
	{
		IMFMediaEvent* pEvent = nullptr;
		pMediaSession->GetEvent(0, &pEvent) >> ThrowOnError("MMFwrapper Shutdown GetEvent", [&] {SAFE_RELEASE(pEvent); CleanUp(); });
		MediaEventType eventType = MEUnknown;
		pEvent->GetType(&eventType) >> ThrowOnError("MMFwrapper Shutdown Get event type", [&] {SAFE_RELEASE(pEvent); CleanUp(); });
		if (MESessionClosed == eventType) mediaSessionClosed = true;
		SAFE_RELEASE(pEvent);
	}
	pMediaSource->Shutdown() >> ThrowOnError("MMFwrapper Shutdown media source shutdown", CleanUp);
	pMediaSession->Shutdown() >> ThrowOnError("MMFwrapper Shutdown media session shutdown", CleanUp);
	MFShutdown() >> ThrowOnError("MMFwrapper Shutdown media foundation shutdown", CleanUp);
	return;
}

void helper_AddBranchToPartialTopology(IMFTopology *pTopology_param, IMFMediaSource* pMediaSource_param, IMFPresentationDescriptor* pPresentationDescriptor_param, DWORD streamIndex_param)
{
	IMFStreamDescriptor* pStreamDescriptor_internal = nullptr;
	IMFActivate* pSinkActivate_internal = nullptr;
	IMFTopologyNode* pSourceNode_internal = nullptr;
	IMFTopologyNode* pOutputNode_internal = nullptr;
	BOOL fSelected_internal = FALSE;

	IMFMediaTypeHandler* pMediaTypeHandler_internal = nullptr;
	GUID guidMajorType_internal;

	std::function<void()> cleanUp_internal{[&]()
	{
		SAFE_RELEASE(pMediaTypeHandler_internal);
		SAFE_RELEASE(pOutputNode_internal);
		SAFE_RELEASE(pSourceNode_internal);
		SAFE_RELEASE(pSinkActivate_internal);
		SAFE_RELEASE(pStreamDescriptor_internal);
	}};

	pPresentationDescriptor->GetStreamDescriptorByIndex(streamIndex_param, &fSelected_internal, &pStreamDescriptor_internal) >> ThrowOnError("GetStreamDescriptorByIndex", cleanUp_internal);

	if (fSelected_internal)
	{
		//Create media sink
		pStreamDescriptor_internal->GetMediaTypeHandler(&pMediaTypeHandler_internal) >> ThrowOnError("GetMediaTypeHandler", cleanUp_internal);
		pMediaTypeHandler_internal->GetMajorType(&guidMajorType_internal) >> ThrowOnError("GetMajorType", cleanUp_internal);
		if (MFMediaType_Audio == guidMajorType_internal)
		{
			MFCreateAudioRendererActivate(&pSinkActivate_internal) >> ThrowOnError("MFCreateAudioRendererActivate", cleanUp_internal);
		}
		else
		{
			pPresentationDescriptor_param->DeselectStream(streamIndex_param) >> ThrowOnError("DeselectStream");
			return cleanUp_internal();
		}
		//Create source node
		MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &pSourceNode_internal) >> ThrowOnError("MFCreateTopologyNode", cleanUp_internal);
		pSourceNode_internal->SetUnknown(MF_TOPONODE_SOURCE, pMediaSource_param) >> ThrowOnError("SetUnknown(MF_TOPONODE_SOURCE", cleanUp_internal);
		pSourceNode_internal->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, pPresentationDescriptor_param) >> ThrowOnError("SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR", cleanUp_internal);
		pSourceNode_internal->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, pStreamDescriptor_internal) >> ThrowOnError("SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR", cleanUp_internal);
		pTopology_param->AddNode(pSourceNode_internal) >> ThrowOnError("AddNode(pSourceNode_internal)", cleanUp_internal);
		//Create output node
		MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &pOutputNode_internal) >> ThrowOnError("MFCreateTopologyNode", cleanUp_internal);
		pOutputNode_internal->SetObject(pSinkActivate_internal) >> ThrowOnError("SetObject(pSinkActivate_internal)", cleanUp_internal);
		UINT32 defaultStreamSinkID = 0;//Since stream sink id is 0, these two lines are probably superflous~
		pOutputNode_internal->SetUINT32(MF_TOPONODE_STREAMID, defaultStreamSinkID) >> ThrowOnError("SetUINT32(MF_TOPONODE_STREAMID", cleanUp_internal);//~
		pOutputNode_internal->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE); //"optional but recommended"
		pTopology_param->AddNode(pOutputNode_internal);

		pSourceNode_internal->ConnectOutput(0, pOutputNode_internal, 0) >> ThrowOnError("ConnectOutput", cleanUp_internal);
	}

	return cleanUp_internal();
}