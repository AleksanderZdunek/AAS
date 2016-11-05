//MMF wrapper ( Microsoft Media Foundation )
//based on https://msdn.microsoft.com/en-us/library/windows/desktop/ms703190(v=vs.85).aspx
#include <mfapi.h>
#include <mfidl.h>

#include <comdef.h>

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
		throw std::runtime_error(std::string(s.message) + "\nErrorcode: " + err.ErrorMessage);
	}
}

const wchar_t* fileName; //LPCWSTR fileName;

//function prototype. clean this up later
void helper_AddBranchToPartialTopology(IMFTopology *pTopology_param, IMFMediaSource* pMediaSource_param, IMFPresentationDescriptor* pPresentationDescriptor_param, DWORD streamIndex_param);

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
}

void help_CreateMediaSinkActivate(IMFStreamDescriptor* pSourceStreamDescriptor_param, IMFActivate** ppActivate_param)
{
	IMFMediaTypeHandler* pMediaTypeHandler_internal = nullptr;
	IMFActivate *pActivate_internal = nullptr;

	pSourceStreamDescriptor_param->GetMediaTypeHandler(&pMediaTypeHandler_internal);
	GUID guidMajorType_internal;
	pMediaTypeHandler_internal->GetMajorType(&guidMajorType_internal);
	if (MFMediaType_Audio == guidMajorType_internal)
	{
		MFCreateAudioRendererActivate(&pActivate_internal);
	}
	else
	{
		//IMFPresentationDescriptor::DeselectStream(streamIndex)
	}
	*ppActivate_param = pActivate_internal;
	return;
}

void help_AddSourceNode(IMFTopology* pTopology_param, IMFMediaSource* pMediaSource_param, IMFPresentationDescriptor* pPresentationDescriptor_param, IMFStreamDescriptor* pStreamDescriptor_param, IMFTopologyNode** ppNode_param)
{
	IMFTopologyNode* pNode_internal = nullptr;
	MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &pNode_internal);
	pNode_internal->SetUnknown(MF_TOPONODE_SOURCE, pMediaSource_param);
	pNode_internal->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, pPresentationDescriptor_param);
	pNode_internal->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, pStreamDescriptor_param);
	pTopology_param->AddNode(pNode_internal);
	*ppNode_param = pNode_internal;
	return;
}

void help_AddOutputNode(IMFTopology* pTopology_param, IMFActivate *pActivate_param, DWORD streamSinkID_param, IMFTopologyNode** ppNode_param)
{
	IMFTopologyNode* pNode_internal = nullptr;
	MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &pNode_internal);
	pNode_internal->SetObject(pActivate_param);
	pNode_internal->SetUINT32(MF_TOPONODE_STREAMID, streamSinkID_param);
	pNode_internal->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE);
	pTopology->AddNode(pNode_internal);
	*ppNode_param = pNode_internal;
	return;
}

void helper_AddBranchToPartialTopology(IMFTopology *pTopology_param, IMFMediaSource* pMediaSource_param, IMFPresentationDescriptor* pPresentationDescriptor_param, DWORD streamIndex_param)
{
	IMFStreamDescriptor* pStreamDescriptor_internal = nullptr;
	IMFActivate* pSinkActivate_internal = nullptr;
	IMFTopologyNode* pSourceNode_internal = nullptr;
	IMFTopologyNode* pOutputNode_internal = nullptr;
	BOOL fSelected_internal = FALSE;

	std::function<void()> cleanUp_internal{[&]()
	{
		SAFE_RELEASE(pOutputNode_internal);
		SAFE_RELEASE(pSourceNode_internal);
		SAFE_RELEASE(pSinkActivate_internal);
		SAFE_RELEASE(pStreamDescriptor_internal);
	}};

	pPresentationDescriptor->GetStreamDescriptorByIndex(streamIndex_param, &fSelected_internal, &pStreamDescriptor_internal) >> ThrowOnError("GetStreamDescriptorByIndex", cleanUp_internal);

	if (fSelected_internal)
	{
		try {
			help_CreateMediaSinkActivate(pStreamDescriptor_internal, &pSinkActivate_internal);
			help_AddSourceNode(pTopology_param, pMediaSource_param, pPresentationDescriptor_param, pStreamDescriptor_internal, &pSourceNode_internal);
			help_AddOutputNode(pTopology_param, pSinkActivate_internal, 0, &pOutputNode_internal);
		}
		catch (std::runtime_error e)
		{
			cleanUp_internal();
			throw e;
		}
		pSourceNode_internal->ConnectOutput(0, pOutputNode_internal, 0) >> ThrowOnError("ConnectOutput", cleanUp_internal);
	}
	return;
}

/*REMOVE ME
void help_CreatePlaybackTopology(IMFMediaSource* pMediaSource_param, IMFPresentationDescriptor *pPresentationDescriptor_param, IMFTopology** ppTopology_param)
{
	IMFTopology *pTopology_internal = nullptr;
	DWORD countSourceStreams_internal = 0;

	MFCreateTopology(&pTopology_internal);
	pPresentationDescriptor_param->GetStreamDescriptorCount(&countSourceStreams_internal);
	for (DWORD i = 0; i < countSourceStreams_internal; i++)
	{
		help_AddBranchToPartialTopology(pTopology_internal, pMediaSource_param, pPresentationDescriptor_param, i);
	}
	*ppTopology_param = pTopology_internal;
	return;
}
*/