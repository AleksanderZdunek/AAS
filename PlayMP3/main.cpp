#include <iostream>
#include "MMFwrapper.h"

int main(int argc, char* argv[])
{
	std::cout << "Play MP3!" << std::endl;
	try {
		Play(L"LeavesInTheWind.mp3");
		//L"No Doubt - Tragic Kingdom.wav"
	}
	catch (std::runtime_error e)
	{
		std::cout << "Runtime error. what: " << e.what() << std::endl;
	}

	system("pause");
	return 0;
}