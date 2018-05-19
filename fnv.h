#pragma once

//
// Simple FNV hash function
//

#include <stdint.h>

static inline void fnv_init(uint32_t* hval)
{
	*hval = 0x811c9dc5;
}

static inline void fnv_update(uint32_t* hval, uint8_t const *buf, uint8_t len)
{
	const uint32_t FNV_32_PRIME = 0x01000193;

	for (; len; --len, ++buf)
	{
		*hval ^= (uint32_t)*buf++;
		*hval *= FNV_32_PRIME;
	}
}
