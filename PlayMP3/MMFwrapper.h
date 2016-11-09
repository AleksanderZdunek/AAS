#pragma once
#ifndef MMFWRAPPER_H
#define MMFWRAPPER_H
#include<functional>

extern std::function<void(std::wstring)> metadataCallback;

void Play(const wchar_t* filename);

#endif // !MMFWRAPPER_H