#pragma once

#include <string.h>
#include <stdio.h>
#include <bb_defines.h>

#define BB_STRCMP(a, b) strcmp(a, b)
#define BB_STRLEN(x) strlen(x)
#define BB_STRNCPY(dst, src, len) bb_strlcpy(dst, src, len)
#define BB_STRICMP(a, b) strcasecmp(a, b)
#define BB_FIND_SUBSTRING(haystack, needle) strstr(haystack, needle)
#define BB_FIND_SUBSTRING_NO_CASE(haystack, needle) strcasestr(haystack, needle)
#define BB_FIND_SUBSTRING_REVERSE(haystack, needle) strrstr(haystack, needle)
#define BB_FIND_SUBSTRING_REVERSE_NO_CASE(haystack, needle) strrstr(haystack, needle)
#define BB_FIND_CHAR(haystack, needle) strchr(haystack, needle)
#define BB_FIND_CHAR_REVERSE(haystack, needle) strrchr(haystack, needle)
#define BB_SPRINTF(dst, format, ...) sprintf(dst, format, __VA_ARGS__)
#define BB_SNPRINTF(dst, len, format, ...) snprintf(dst, len, format, __VA_ARGS__)
#define BB_SSCANF(str, format, ...) sscanf(str, format, __VA_ARGS__);
#define BB_IS_NULL_OR_EMPTY_STRING(str) ((NULL == str) || (((const char*)str)[0] == '\0'))

#if !defined(BB_PATH_SEP)
	// default is unix path separator
	#define BB_PATH_SEP '/'
#endif
