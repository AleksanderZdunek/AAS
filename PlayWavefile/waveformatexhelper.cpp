#include "waveformatexhelper.h"
#include <sstream>
#include <iostream>

void printWAVEFORMATEXTENSIBLE(WAVEFORMATEXTENSIBLE s)
{
	std::ostringstream oss;
	oss << std::showbase;
	oss << "FormatTag: " << std::hex << s.Format.wFormatTag << std::endl;
	oss << "Channels: " << std::dec << s.Format.nChannels << std::endl;
	oss << "SamplePerSec: " << std::dec << s.Format.nSamplesPerSec << std::endl;
	oss << "AvgBytesPerSec: " << std::dec << s.Format.nAvgBytesPerSec << std::endl;
	oss << "BlockAlign: " << std::dec << s.Format.nBlockAlign << std::endl;
	oss << "BitsPerSample: " << std::dec << s.Format.wBitsPerSample << std::endl;
	oss << "cbSize: " << std::dec << s.Format.cbSize << std::endl;
	oss << "Samples.ValidBitsPerSample: " << std::dec << s.Samples.wValidBitsPerSample << std::endl;
	oss << "ChannelMask: " << std::hex << s.dwChannelMask << std::endl;
	oss << "GUID: " << std::dec << "GUID not printable" << std::endl;

	std::cout << oss.str();
};

void printWAVEFORMATEX(WAVEFORMATEX s)
{
	std::ostringstream oss;
	oss << std::showbase;
	oss << "FormatTag: " << std::hex << s.wFormatTag << std::endl;
	oss << "Channels: " << std::dec << s.nChannels << std::endl;
	oss << "SamplePerSec: " << std::dec << s.nSamplesPerSec << std::endl;
	oss << "AvgBytesPerSec: " << std::dec << s.nAvgBytesPerSec << std::endl;
	oss << "BlockAlign: " << std::dec << s.nBlockAlign << std::endl;
	oss << "BitsPerSample: " << std::dec << s.wBitsPerSample << std::endl;

	std::cout << oss.str();
};