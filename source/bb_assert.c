#include "bb_assert.h"
#include "bb_defines.h"
#include "bb_stacktrace.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

// special colour for asserts: black text on red background
#define BB_COL_ASSERT   "\x1B[31;1;7m"
#if !defined(BB_COL_DEFAULT)
	#define BB_COL_DEFAULT "\x1B[0m"
#endif
#define BB_LOG_ASSERT_BEGIN(filename, line) fprintf(stderr, BB_COL_ASSERT "[ASSERT ] %s:%d: Assertion failed: ", filename, line)
#define BB_LOG_ASSERT_END() fprintf(stderr, BB_COL_DEFAULT "\n")

void bb_assert(const char* filename, int line, const char* format, ...)
{
	va_list argp;
	va_start(argp, format);
	BB_LOG_ASSERT_BEGIN(filename, line);
	vfprintf(stderr, format, argp);
	BB_LOG_ASSERT_END();
	va_end(argp);

	BB_STACK_TRACE();
}
