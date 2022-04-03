#pragma once

#define PATH_DELIM_UNIX '/'
#define PATH_DELIM_WIN '\\'

#ifdef _WIN32
    #define PATH_DELIM_DEFAULT PATH_DELIM_WIN
#else
    #define PATH_DELIM_DEFAULT PATH_DELIM_UNIX
#endif

#define PATH_MAX_LENGTH 1024

char* path_join(int count, ...);
char* path_get_directory(char* path);
char* path_get_filename(char* path);
char* path_get_filename_wo_ext(char* path);