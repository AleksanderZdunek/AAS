#pragma once
#ifndef ARCHIEUTIL_H
#define ARCHIEUTIL_H
#include  <cstdint>

void samplerateConverter(uint32_t target_rate, uint32_t source_rate, uint32_t blockSize, char* target, char* source, size_t target_size, size_t source_size);

#endif // !ARCHIEUTIL_H
