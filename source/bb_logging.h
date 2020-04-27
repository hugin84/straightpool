#pragma once

#if defined(BB_NO_LOG)
	#define BB_LOG(format, ...)
	#define BB_LOG_DEBUG(format, ...)
	#define BB_LOG_WARNING(format, ...)
	#define BB_LOG_ERROR(format, ...)
#else
	#include <stdio.h>

	/****************
	 * from http://ascii-table.com/ansi-escape-sequences.php :
	 * Text Attributes:
	 * 0	All attributes off
	 * 1	Bold on
	 * 4	Underscore (on monochrome display adapter only)
	 * 5	Blink on
	 * 7	Reverse video on
	 * 8	Concealed on
	 * 
	 * Foreground colors
	 * 30	Black
	 * 31	Red
	 * 32	Green
	 * 33	Yellow
	 * 34	Blue
	 * 35	Magenta
	 * 36	Cyan
	 * 37	White
	 * 
	 * Background colors
	 * 40	Black
	 * 41	Red
	 * 42	Green
	 * 43	Yellow
	 * 44	Blue
	 * 45	Magenta
	 * 46	Cyan
	 * 47	White
	 ****************/

	// "default"
	#define BB_COL_DEFAULT  "\e[0m"
	#define BB_COL_RED      "\e[31m"
	#define BB_COL_GREEN    "\e[32m"
	#define BB_COL_YELLOW   "\e[33m"
	#define BB_COL_BLUE     "\e[34m"
	#define BB_COL_MAGENTA  "\e[35m"
	#define BB_COL_CYAN     "\e[36m"
	#define BB_COL_WHITE    "\e[37m"
	// bold
	#define BB_COL_BRED     "\e[31;1m"
	#define BB_COL_BGREEN   "\e[32;1m"
	#define BB_COL_BYELLOW  "\e[33;1m"
	#define BB_COL_BBLUE    "\e[34;1m"
	#define BB_COL_BMAGENTA "\e[35;1m"
	#define BB_COL_BCYAN    "\e[36;1m"
	#define BB_COL_BWHITE   "\e[37;1m"
	
	// helper macros:
	// __FILENAME__ returns the filename without folder,
	#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
	// BB_FUNCTION_LEN_STR defines minimum length of a string argument, fprintf applies whitespace padding to fill the available space
	#define BB_FUNCTION_LEN_STR "%-23s"
	#define BB_FILENAME_LEN_STR "%-23s"

	#define BB_LOG(format, ...)         fprintf(stdout, format"\n", ##__VA_ARGS__)
	#define BB_LOG_DEBUG(format, ...)   fprintf(stdout, "[" BB_COL_BBLUE   "DEBUG  " BB_COL_DEFAULT "]" BB_FILENAME_LEN_STR ":%3d [" BB_COL_CYAN BB_FUNCTION_LEN_STR BB_COL_DEFAULT "] " format "\n", __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
	#define BB_LOG_WARNING(format, ...) fprintf(stderr, "[" BB_COL_BYELLOW "WARNING" BB_COL_DEFAULT "]" BB_FILENAME_LEN_STR ":%3d [" BB_COL_CYAN BB_FUNCTION_LEN_STR BB_COL_DEFAULT "] " format "\n", __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
	#define BB_LOG_ERROR(format, ...)   fprintf(stderr, "[" BB_COL_BRED    "ERROR  " BB_COL_DEFAULT "]" BB_FILENAME_LEN_STR ":%3d [" BB_COL_CYAN BB_FUNCTION_LEN_STR BB_COL_DEFAULT "] " format "\n", __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#endif
