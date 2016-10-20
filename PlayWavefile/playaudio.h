#pragma once
#ifndef PLAYAUDIO_H
#define PLAYAUDIO_H
#include <functional>
#include <Audioclient.h>

extern WAVEFORMATEXTENSIBLE formatConfig;
extern std::function<DWORD(UINT32, BYTE*)> LoadData;

void Init();
void Play();

#endif //!PLAYAUDIO_H