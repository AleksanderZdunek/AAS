#pragma once
#ifndef ARCHIE_LINUX_H
#define ARCHIE_LINUX_H

#include <functional>
#include "archie_linux_aux.h"

namespace Archie {

	extern WAVEFORMATEXTENSIBLE configFormat;
	extern std::function<DWORD(UINT32, BYTE*)> LoadData;

	bool Init();
	void UnInit();

	void Play();

}//!namespace Archie

#endif // !ARCHIE_LINUX_H
