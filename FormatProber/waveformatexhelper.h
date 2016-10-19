#pragma once
#ifndef WAVEFORMATEXHELPER_H
#define WAVEFORMATEXHELPER_H

#include <Audioclient.h>
#include <string>

void printWAVEFORMATEX(const WAVEFORMATEX& s);
void printWAVEFORMATEXTENSIBLE(const WAVEFORMATEXTENSIBLE& s);
std::string tostringWAVEFORMATEX(const WAVEFORMATEX& s);
std::string tostringWAVEFORMATEXTENSIBLE(const WAVEFORMATEXTENSIBLE& s);

#endif