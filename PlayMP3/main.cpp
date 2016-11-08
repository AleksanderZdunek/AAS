#include <iostream>
#include "MMFwrapper.h"
#include <string>
#include <stdlib.h>

std::wstring charstarToWString(char* s)
{
	const size_t size = std::strlen(s);
	size_t c{ 0 };
	std::wstring r;
	r.resize(size+1);
	mbstowcs_s(&c, &r[0], size+1, s, size);
	return r;
};

int main(int argc, char* argv[])
{
	std::wstring filename(L"");
	if (1 < argc) filename = charstarToWString(argv[1]);

	std::cout << "Play MP3!" << std::endl;
	try {
		Play(filename.c_str());
		//Play(L"LeavesInTheWind.mp3");
		//L"No Doubt - Tragic Kingdom.wav"
	}
	catch (std::runtime_error e)
	{
		std::cout << "Runtime error. what: " << e.what() << std::endl;
	}

	//system("pause");
	return 0;
}