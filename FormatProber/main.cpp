#include <iostream>
#include "archie.h"
#include "waveformatexhelper.h"

int main(int argc, char* argv[])
{
	std::cout << "~Audio Format Probe~" << std::endl << std::endl;

	WAVEFORMATEXTENSIBLE myFormat;
	Archie::Init();
	myFormat = Archie::configFormat;
	std::cout << "Default Format" << std::endl;
	printWAVEFORMATEXTENSIBLE(myFormat);
	Archie::UnInit();

	std::cout << std::endl;
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
		Archie::Init();
		std::cout << "Init OK" << std::endl;
		Archie::Play(); std::cout << "Play()" << std::endl;
	}
	catch (std::runtime_error e) {
		std::cerr << "Caught Exception. What: " << e.what() << std::endl;
	}

	std::cout << std::endl;
	system("pause");
	Archie::UnInit();
	return 0;
}