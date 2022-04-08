#pragma once

#define PATH_DELIM_UNIX '/'
#define PATH_DELIM_WIN '\\'

#ifdef _WIN32
    #define PATH_DELIM_DEFAULT PATH_DELIM_WIN
#else
    #define PATH_DELIM_DEFAULT PATH_DELIM_UNIX
#endif

#define PATH_MAX_LENGTH 1024

// joins a variable 'count' number of c-strings together to build a path
char* path_join(int count, ...);
// gets the parent directory of the given path
char* path_get_directory(char* path);
// gets the filename from the path
char* path_get_filename(char* path);
// gets the filename without the extension from the path
char* path_get_filename_wo_ext(char* path);