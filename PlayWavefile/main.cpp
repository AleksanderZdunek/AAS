#include <iostream>
#include <fstream>
#include "archie.h"
#include "WaveHeaderStructs.h"

struct Header
{
	RiffHeader h1;
	WaveHeader h2;
};

int main(int argc, char* argv[])
{
	std::cout << "Play Wavefile!" << std::endl;

	Header h;
	ChunkHeader tmpChunkHeader;
	WAVEFORMATEXTENSIBLE formatChunk{0};

	std::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
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

				if (WAVE_FORMAT_PCM == formatChunk.Format.wFormatTag)
				{
					formatChunk.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
					formatChunk.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
					formatChunk.Format.cbSize = 22;
					formatChunk.Samples.wValidBitsPerSample = formatChunk.Format.wBitsPerSample;
				}
				Archie::configFormat = formatChunk;
				Archie::Init();
			}
			else if ('atad' == tmpChunkHeader.IDtag)
			{
				std::cout << "Playing data" << std::endl;
				Archie::LoadData = [&file, remaining = tmpChunkHeader.chunkSize](UINT32 size, BYTE* data)mutable
				{
					if (!file.good()) return 2;

					if (size <= remaining) 
					{
						file.read((char*)data, size);
						remaining -= size;
					}
					else if (0 < remaining)
					{
						file.read((char*)data, remaining);
						remaining = 0;
					}
					else
					{
						return 2;
					}
					return 0;
				};
				Archie::Play();
			}
			else
			{
				std::cerr << "Unknown chunk type read. ";
				char arr[5]{0};
				memcpy(&arr, &tmpChunkHeader.IDtag, 4);
				std::cerr << "chunk id: \"" << arr <<"\""<<std::endl;
				file.seekg(tmpChunkHeader.chunkSize, std::ios_base::cur);
			}
			std::cout << std::endl;
		}
	}

	Archie::UnInit();
	file.close();

	return 0;
}