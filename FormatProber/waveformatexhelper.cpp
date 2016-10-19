#include "waveformatexhelper.h"
#include <sstream>
#include <iostream>
#include <unordered_map>

/*This code ripped directly from Stackoverflow
inline bool operator == (const GUID& a, const GUID& b)
{
	return std::memcmp(&a, &b, sizeof(GUID)) == 0;
}*/
namespace std {
template<> struct hash<GUID>
{
	size_t operator()(const GUID& guid) const noexcept
	{
		const std::uint64_t* p = reinterpret_cast<const std::uint64_t*>(&guid);
		std::hash<std::uint64_t> hash;
		return hash(p[0]) ^ hash(p[1]);
	}
};
}/**/

const std::unordered_map<GUID, std::string> subtypeGUIDstringrepresentationMap {
	{KSDATAFORMAT_SUBTYPE_ADPCM,"KSDATAFORMAT_SUBTYPE_ADPCM"},
	{KSDATAFORMAT_SUBTYPE_ALAW,"KSDATAFORMAT_SUBTYPE_ALAW"},
	{KSDATAFORMAT_SUBTYPE_DRM,"KSDATAFORMAT_SUBTYPE_DRM"},
	{KSDATAFORMAT_SUBTYPE_IEC61937_DOLBY_DIGITAL_PLUS,"KSDATAFORMAT_SUBTYPE_IEC61937_DOLBY_DIGITAL_PLUS"},
	{KSDATAFORMAT_SUBTYPE_IEC61937_DOLBY_DIGITAL,"KSDATAFORMAT_SUBTYPE_IEC61937_DOLBY_DIGITAL"},
	{KSDATAFORMAT_SUBTYPE_IEEE_FLOAT,"KSDATAFORMAT_SUBTYPE_IEEE_FLOAT"},
	{KSDATAFORMAT_SUBTYPE_MPEG,"KSDATAFORMAT_SUBTYPE_MPEG"},
	{KSDATAFORMAT_SUBTYPE_MULAW,"KSDATAFORMAT_SUBTYPE_MULAW"},
	{KSDATAFORMAT_SUBTYPE_PCM,"KSDATAFORMAT_SUBTYPE_PCM"}
};


void printWAVEFORMATEX(const WAVEFORMATEX& s)
{
	std::cerr << ((WAVE_FORMAT_EXTENSIBLE == s.wFormatTag) ? tostringWAVEFORMATEXTENSIBLE(*reinterpret_cast<const WAVEFORMATEXTENSIBLE*>(&s)) : tostringWAVEFORMATEX(s));
}

void printWAVEFORMATEXTENSIBLE(const WAVEFORMATEXTENSIBLE& s)
{
	printWAVEFORMATEX(s.Format);
}

std::string tostringWAVEFORMATEX(const WAVEFORMATEX& s)
{
	std::ostringstream oss;
	oss << std::showbase;
	oss << "FormatTag: " << std::hex << s.wFormatTag << std::endl;
	oss << "Channels: " << std::dec << s.nChannels << std::endl;
	oss << "SamplePerSec: " << std::dec << s.nSamplesPerSec << std::endl;
	oss << "AvgBytesPerSec: " << std::dec << s.nAvgBytesPerSec << std::endl;
	oss << "BlockAlign: " << std::dec << s.nBlockAlign << std::endl;
	oss << "BitsPerSample: " << std::dec << s.wBitsPerSample << std::endl;
	oss << "cbSize: " << std::dec << s.cbSize << std::endl;
	return oss.str();
}

std::string tostringWAVEFORMATEXTENSIBLE(const WAVEFORMATEXTENSIBLE& s)
{
	std::ostringstream oss;
	oss << tostringWAVEFORMATEX(s.Format);
	oss << std::showbase;
	oss << "Samples.ValidBitsPerSample: " << std::dec << s.Samples.wValidBitsPerSample << std::endl;
	oss << "ChannelMask: " << std::hex << s.dwChannelMask << std::endl;

	auto it = subtypeGUIDstringrepresentationMap.find(s.SubFormat);
	if(it == subtypeGUIDstringrepresentationMap.end())
	{
		RPC_CSTR pzsGUID;
		UuidToString(&s.SubFormat, &pzsGUID);
		oss << "GUID: " << pzsGUID << std::endl;
		RpcStringFree(&pzsGUID);
	}
	else
	{
		oss << "GUID: " << it->second << std::endl;
	}

	return oss.str();
}