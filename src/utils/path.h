#pragma once

// delimiter for directories in path
#define PATH_DELIM_UNIX '/'
#define PATH_DELIM_WIN '\\'
// delimiter for paths in PATH env variable
#define ENV_PATH_DELIM_UNIX ":"
#define ENV_PATH_DELIM_WIN ";"

#ifdef _WIN32
    #define PATH_DELIM PATH_DELIM_WIN
    #define ENV_PATH_DELIM ENV_PATH_DELIM_WIN
#else
    #define PATH_DELIM PATH_DELIM_UNIX
    #define ENV_PATH_DELIM ENV_PATH_DELIM_UNIX
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
// finds the given relative path in either the working directory or PATH (standard library files), parent_dir can be 0
char* path_locate_file(char* path, char* parent_dir);