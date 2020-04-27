#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <bb_string.h>
#include <bb_logging.h>

static int bb_dirent_filter(const struct dirent* p_dirent)
{
	return (p_dirent->d_type == DT_REG);
}


uint bb_get_filenames_in_folder(const char* foldername, char*** p_names_array_out)
{
	DIR* p_dir = opendir(foldername);
	if (NULL == p_dir)
	{
		BB_LOG_ERROR("Failed to open folder %s", foldername);
		perror(NULL);
		return 0u;
	}

	struct dirent** dirent_list = NULL;
	int num_files = scandir(foldername, &dirent_list, bb_dirent_filter, alphasort);

	char** names_out = (char**)malloc(num_files * sizeof(char*));
	*p_names_array_out = names_out;

	uint  foldername_len = BB_STRLEN(foldername);

	for (int i = 0; i < num_files; ++i)
	{
		struct dirent* p_dirent = dirent_list[i];
		uint current_len = foldername_len + BB_STRLEN(p_dirent->d_name) + 2;
		(*p_names_array_out)[i] = (char*)malloc(current_len);
		char* curr_path = (*p_names_array_out)[i];
		
		BB_SPRINTF(curr_path, "%s/%s", foldername, p_dirent->d_name);
		curr_path[current_len - 1] = '\0';
	}

	closedir(p_dir);
	return (uint)num_files;
}
