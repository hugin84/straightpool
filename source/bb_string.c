#include <bb_string.h>

size_t bb_strlcpy(char* dst, const char* src, size_t buffer_size)
{
	size_t src_len = BB_STRLEN(src);
	if (buffer_size != 0)
	{
		size_t num_chars = buffer_size - 1;
		if (src_len < num_chars)
		{
			num_chars = src_len;
		}

		BB_MEMCPY(dst, src, num_chars);
		dst[num_chars] = '\0';
	}

	return src_len;
}

