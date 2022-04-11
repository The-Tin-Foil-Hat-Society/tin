#include "path.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

// duplicates the path so it doesn't take up PATH_MAX_LENGTH memory
char* path_trunc(char* path)
{
    char* ret = strdup(path);
    free(path);
    return ret;
}

char* path_join(int count, ...)
{
    // TODO: ensure there are delimiter between elements

    int len = 0;
    char* path = malloc(PATH_MAX_LENGTH);
    path[0] = '\0';

    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++)
    {
        char* arg = va_arg(args, char*);
        if (arg != 0)
        {
            strcat(path, arg);
            len += strlen(arg);
        }
    }

    va_end(args);

    return path_trunc(path);
}
char* path_get_directory(char* path)
{
    // check for both kinds of delimiters
    char* s = strrchr(path, PATH_DELIM_UNIX);
    if (s == 0)
    {
        s = strrchr(path, PATH_DELIM_WIN);
    }

    if (s != 0)
    {
        char* path_copy = strdup(path);
        path_copy[s - path + 1] = '\0';

        return path_trunc(path_copy);
    }
    
    return 0;
}
char* path_get_filename(char* path)
{
    char* filename;

    // check for both kinds of delimiters
    char* s = strrchr(path, PATH_DELIM_UNIX);
    if (s == 0)
    {
        s = strrchr(path, PATH_DELIM_WIN);
    }

    if (s == 0)
    {
        filename = strdup(path);
    }
    else
    {
        filename = strdup(s + 1);
    }

    return path_trunc(filename);
}
char* path_get_filename_wo_ext(char* path)
{
    char* filename = path_get_filename(path);

    char* s = strrchr(filename, '.');
    if (s != 0)
    {
        s[0] = '\0';
    }

    return path_trunc(filename);
}