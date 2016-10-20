#pragma once
#ifndef ARCHIE_H
#define ARCHIE_H
#include <functional>
#include <Audioclient.h>

namespace Archie{

extern WAVEFORMATEXTENSIBLE configFormat;
extern std::function<DWORD(UINT32, BYTE*)> LoadData;

bool Init();
void UnInit();

WAVEFORMATEXTENSIBLE GetDefaultFormat();

bool IsFormatSupported(const WAVEFORMATEX* f);
bool IsFormatSupported(const WAVEFORMATEX& f);
bool IsFormatSupported(const WAVEFORMATEXTENSIBLE& f);

void Play();

bool Test();

}//!namespace Archie

#endif //!ARCHIE_H