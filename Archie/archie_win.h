#pragma once
#ifndef ARCHIE_WIN_H
#define ARCHIE_WIN_H
#include <functional>
#include <Audioclient.h>

namespace Archie{

extern WAVEFORMATEXTENSIBLE configFormat;
extern std::function<DWORD(UINT32, BYTE*)> LoadData;

bool Init();
void UnInit();

void Play();

}//!namespace Archie

#endif //!ARCHIE_WIN_H