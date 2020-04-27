#pragma once

#include "bb_platformconfig.h"
#include "bb_logging.h"
#include <execinfo.h>

#if (defined(BB_PLATFORM_LINUX) || defined(BB_PLATFORM_ANDROID))
	#define BB_STACK_TRACE() \
	{\
		void*  return_addresses[16] = {};\
		int    stack_depth  = backtrace((void**)&return_addresses, 16);\
		char** symbol_names = backtrace_symbols((void**)&return_addresses, stack_depth);\
		for (int i = 0; i < stack_depth; ++i)\
		{\
			BB_LOG("%s\n", symbol_names[i]);\
		}\
		free(symbol_names);\
	}
#else
	#define BB_STACK_TRACE() (void)(0)
#endif
