#pragma once
#include "bb_platformconfig.h"
#include <string.h>

#define BB_MEMCPY(p_dest, p_src, num_bytes)     memcpy (p_dest,  p_src,    num_bytes)
#define BB_MEMMOVE(p_dest, p_src, num_bytes)    memmove(p_dest,  p_src,    num_bytes)
#define BB_MEMSET(p_dest, value, num_bytes)     memset (p_dest,  value,    num_bytes)
#define BB_MEMCMP(p_first, p_second, num_bytes) memcmp (p_first, p_second, num_bytes)

#define BB_MIN(a, b) (a<b?a:b)
#define BB_MAX(a, b) (a>b?a:b)

#define BB_UNUSED(x) (void)(x)

#if (BB_COMPILER == BB_CLANG)
	#define BB_DEOPTIMIZE clang optimize off
	#define BB_REOPTIMIZE clang optimize on
#elif (BB_COMPILER == BB_MSVC)
	#define BB_DEOPTIMIZE __pragma optimize( "", off )
	#define BB_REOPTIMIZE __pragma optimize( "", on )
#else
	#define BB_DEOPTIMIZE message("BB_DEOPTIMIZE not available")
	#define BB_REOPTIMIZE message("BB_REOPTIMIZE not available")
#endif
