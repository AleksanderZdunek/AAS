#include <cstdint>
#include <cstring>
//namespace Archie { namespace util {

void samplerateConverter(uint32_t target_rate, uint32_t source_rate, uint32_t blockSize, char* target, char* source, size_t target_size, size_t source_size)
{
	const float convertRatio = float(source_rate) / target_rate;

	//Warning! Assumes target size is smaller than (or equal) source size, ie. only upsampling is supperted now. I think.
	for (size_t target_offset = 0; target_offset < target_size; target_offset += blockSize)
	{
		memmove(target+target_offset, source + size_t(target_offset*convertRatio), blockSize);
	}
}

//}}//!util//!Archie