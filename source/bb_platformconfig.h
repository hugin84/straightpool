#pragma once

#define BB_LINUX    1
#define BB_MACOS    2
#define BB_BSD      4
#define BB_ANDROID  8
#define BB_WINDOWS 16

#if defined(__ANDROID__)
	#define BB_PLATFORM_ANDROID
	#define BB_PLATFORM BB_ANDROID
#elif defined(__linux)
	#define BB_PLATFORM_LINUX
	#define BB_PLATFORM BB_LINUX
#elif defined(__APPLE__)
	#include <TargetConditionals.h>
	#if TARGET_OS_MAC == 1
		#define BB_PLATFORM_MACOS
		#define BB_PLATFORM BB_MACOS
	#endif
#elif defined(_WIN32)
	#define BB_PLATFOM_WINDOWS
	#define BB_PLATFORM BB_WINDOWS
#endif

#if !defined(BB_PLATFORM)
	#error "unsupported platform"
#endif

// assume nobody is running on PPC, Itanium, Arm, etc
#if defined(__x86_64) || defined(__amd64__) || defined(_M_AMD64) || defined(_M_X64)
	#define BB_BITS_64 1
	#define BB_BITS_32 0
#else
	#define BB_BITS_64 0
	#define BB_BITS_32 1
	#warning "32bit architectures are not officially supported"
#endif

// detect compiler, see http://nadeausoftware.com/articles/2012/10/c_c_tip_how_detect_compiler_name_and_version_using_compiler_predefined_macros
#define BB_CLANG 1
#define BB_GCC   2
#define BB_MSVC  4

#if defined(__clang__)
	#define BB_IS_CLANG 1
	#define BB_COMPILER BB_CLANG
#elif defined(__GNUC__)
	#define BB_IS_GCC 1
	#define BB_COMPILER BB_GCC
#elif defined(_MSC_VER)
	#define BB_IS_MSVC 1
	#define BB_COMPILER BB_MSVC
#else
	#error "unsupported compiler"
#endif
