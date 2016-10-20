#include <iostream>
#include <fstream>

#include "playaudio.h"
#include "waveformatexhelper.h"
#include "archieutil.h"

#include <mmdeviceapi.h>
//#include <mmreg.h>
#include <KS.H>
#include <ksmedia.h>
#include "WaveHeaderStructs.h"
#include <string>
#include <sstream>
#include <cstdlib>

struct Header
{
	RiffHeader h1;
	WaveHeader h2;
	ChunkHeader fmtChunkHeader;
	FormatChunk fmtChunk;
};

void printWAVEFORMATEXTENSIBLE(WAVEFORMATEXTENSIBLE s);

int main(int argc, char* argv[])
{
	std::cout << "Play Wavefile!" << std::endl;

	Header h;
	ChunkHeader tmpChunkHeader;
	WAVEFORMATEXTENSIBLE formatChunk{0};

	std::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	//file.open("windmill.wav", std::ifstream::binary);
	file.open("No Doubt - Tragic Kingdom.wav", std::ifstream::binary);

	file.read((char*)&h, 12);

	if (h.h1.IDtag == 'XFIR')
	{
		std::cerr << "File is a RIFX format. Program does not handle RIFX format." << std::endl;
	}
	else if (h.h1.IDtag != 'FFIR' || h.h2.IDtag != 'EVAW')
	{
		std::cerr << "File is not a Wave file." << std::endl;
	}
	else
	{ //normal operations

		while (EOF != file.peek())
		{
			file.read((char*)&tmpChunkHeader, 8);

			if (' tmf' == tmpChunkHeader.IDtag)
			{
				file.read((char*)&formatChunk, tmpChunkHeader.chunkSize);
				printWAVEFORMATEXTENSIBLE(formatChunk);

				formatConfig = formatChunk;
				printWAVEFORMATEXTENSIBLE(formatConfig);
				//formatConfig.Format.nSamplesPerSec = 192000;
				//formatConfig.Format.nAvgBytesPerSec = formatConfig.Format.nSamplesPerSec * formatConfig.Format.nBlockAlign;
				formatConfig.Format.cbSize = 22;
				formatConfig.Samples.wValidBitsPerSample = formatConfig.Format.wBitsPerSample;
				//formatConfig.dwChannelMask = 3;
				formatConfig.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
				formatConfig.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
				printWAVEFORMATEXTENSIBLE(formatConfig);
				Init();
			}
			else if ('atad' == tmpChunkHeader.IDtag)
			{
				std::cout << "Playing data" << std::endl;
				//file.seekg(tmpChunkHeader.chunkSize, std::ios_base::cur);
				UINT32 sourceSamplerate = formatChunk.Format.nSamplesPerSec;
				//UINT32 sourceSamplerate = formatConfig.Format.nSamplesPerSec;
				UINT32 targetSamplerate = formatConfig.Format.nSamplesPerSec;
				LoadData = [&file, remaining = tmpChunkHeader.chunkSize, convert = samplerateConverter, targetSamplerate, sourceSamplerate, blockSize = formatConfig.Format.nBlockAlign](UINT32 size, BYTE* data)mutable
				{
					if (!file.good()) return 2;

					size_t sourceSize = size_t((size / float(blockSize))*(sourceSamplerate / float(targetSamplerate))*blockSize);
					//size_t sourceSize = size_t(	blockSize*size*sourceSamplerate / (float(blockSize)*float(targetSamplerate)) );
					size_t offset = size - sourceSize;

					if (sourceSize <= remaining) 
					{
						file.read((char*)data+offset, sourceSize);
						convert(targetSamplerate, sourceSamplerate, blockSize, (char*)data, (char*)data+offset, size, sourceSize);
						remaining -= size;
					}
					else if (0 < remaining)
					{
						file.read((char*)data+offset, remaining);
						convert(targetSamplerate, sourceSamplerate, blockSize, (char*)data, (char*)data + offset, size, sourceSize);
						memset((char*)data+remaining, data[remaining-1], size-remaining);
						remaining = 0;
					}
					else
					{
						return 2;
					}
					return 0;

					/*interesting accidental stuff
					const float twoPI = (float)atan(1) * 8;
					const float phaseDelta = 261.626f * twoPI / sampleRate;
					static float phase = 0;
					for (UINT32 i = 0; i < size-8; i += 8)
					{
						if (0 == i % 8)
						{
							UINT32 j = i / 8;
							float waveSample = sin(phase);

							phase += phaseDelta;
							if (phase >= twoPI) phase -= twoPI;

							*((float*)data + 2 * j + 0) = waveSample; //left channel
							*((float*)data + 2 * j + 1) = waveSample; //right channel
						}
					}
					return 0;
					*/
				};
				Play();
			}
			else
			{
				std::cerr << "Unknown chunk type read. ";
				char arr[5]{0};
				memcpy(&arr, &tmpChunkHeader.IDtag, 4);
				std::cerr << "chunk id: \"" << arr <<"\""<<std::endl;
				//file.ignore(tmpChunkHeader.chunkSize); //slow
				file.seekg(tmpChunkHeader.chunkSize, std::ios_base::cur);
			}
			std::cout << std::endl;
		}
	}

	file.close();

	system("pause");
	return 0;
}