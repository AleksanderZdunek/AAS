#include <iostream>
#include "archie.h"
#include "waveformatexhelper.h"

int main(int argc, char* argv[])
{
	std::cout << "Audio Format Prober!" << std::endl << std::endl;

	WAVEFORMATEXTENSIBLE myFormat;
	Archie::Init();
	myFormat = Archie::GetMixerFormat();
	//printWAVEFORMATEXTENSIBLE(myFormat);
	myFormat.Format.nSamplesPerSec = 44100;
	myFormat.Format.nBlockAlign = 4;
	myFormat.Format.nAvgBytesPerSec = myFormat.Format.nSamplesPerSec*myFormat.Format.nBlockAlign;
	myFormat.Format.wBitsPerSample = 16;
	myFormat.Samples.wValidBitsPerSample = 16;
	myFormat.SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
	printWAVEFORMATEXTENSIBLE(myFormat);
	Archie::configFormat = myFormat;

	std::cout << std::endl;
	try {
		std::cout << "Test: " << (Archie::Test()?"Error, but no exception":"OK!") << std::endl;
	}
	catch (std::runtime_error e) {
		std::cerr << "Caught Exception. What: " << e.what() << std::endl;
	}

	/*
	if (Archie::IsFormatSupported(myFormat))
	{
		std::cout << "Format Supported!" << std::endl;
	}
	else
	{
		std::cout << "Format Not Supported!" << std::endl;
	}*/

	/*
	while(true)
	{
		std::cout << std::endl << "Mixer Format:" << std::endl;
		printWAVEFORMATEXTENSIBLE(Archie::GetMixerFormat());
		system("pause");
	}*/

	system("pause");
	return 0;
}