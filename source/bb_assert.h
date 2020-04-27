#pragma once
#include "bb_platformconfig.h"
#if (defined(BB_IS_CLANG) || defined(BB_IS_GCC))
	#include "signal.h"
#endif

#undef assert

#ifdef __cplusplus
extern "C" {
#endif

void bb_assert(const char* filename, int line, const char* format_string, ...);

#ifdef __cplusplus
}
#endif

#if !defined(BB_DISABLE_ASSERTS)
	#if (defined(BB_IS_CLANG) || defined(BB_IS_GCC))
		#define BB_DEBUG_BREAK() raise(SIGTRAP)
		#define BB_FATAL_BREAK() __builtin_trap()
	#elif defined(BB_IS_MSVC)
		#define BB_DEBUG_BREAK() __debugbreak()
		#define BB_FATAL_BREAK() __debugbreak()
	#else
		#warning "BB_DEBUG_BREAK() undefined, asserts will not work"
		#define BB_DEBUG_BREAK() (void)(0)
	#endif
	#define GET_MACRO(_1,_2,_3,_4,BB_ASSERT,...) BB_ASSERT
	#define BB_ASSERT4(x, format, ...) (!(x) ? bb_assert(__FILE__, __LINE__, "("#x ") " format, ##__VA_ARGS__), (BB_DEBUG_BREAK(),(void)(0)) : (void)(0))
	#define BB_ASSERT3(x, format, ...) (!(x) ? bb_assert(__FILE__, __LINE__, "("#x ") " format, ##__VA_ARGS__), (BB_DEBUG_BREAK(),(void)(0)) : (void)(0))
	#define BB_ASSERT2(x, text       ) (!(x) ? bb_assert(__FILE__, __LINE__, "("#x ") " text,   ""           ), (BB_DEBUG_BREAK(),(void)(0)) : (void)(0))
	#define BB_ASSERT1(x)              (!(x) ? bb_assert(__FILE__, __LINE__, "("#x ") ",        ""           ), (BB_DEBUG_BREAK(),(void)(0)) : (void)(0))
	#define BB_ASSERT(...) GET_MACRO(__VA_ARGS__, BB_ASSERT4, BB_ASSERT3, BB_ASSERT2, BB_ASSERT1)(__VA_ARGS__)
#else
	#define BB_ASSERT(...) (void)(0)
#endif
