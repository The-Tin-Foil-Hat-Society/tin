#include "path.h"
#include <stdarg.h>
#include <stdio.h>
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
            // if there is no delimiter on either the previous or current element, and the current element isn't an extension, add the delimiter
            if (len > 0 && path[len - 1] != PATH_DELIM_UNIX && path[len - 1] != PATH_DELIM_WIN && arg[0] != PATH_DELIM_UNIX && arg[0] != PATH_DELIM_WIN && arg[0] != '.')
            {
                path[len] = PATH_DELIM;
                path[len + 1] = '\0';
                len += 1;
            }

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
char* path_locate_file(char* path, char* parent_dir)
{
    char* new_path = 0;

    // try to find the file locally
    if (parent_dir != 0)
    {
        new_path = path_join(2, parent_dir, path);
    }
    else
    {
        new_path = strdup(path);
    }

    FILE* file;
    if (file = fopen(new_path, "rb"))
	{
        fclose(file);
        return new_path;
	}
    free(new_path);
    new_path = 0;

    // try to find the file in PATH
    char* env_path = getenv("PATH");
    if (env_path == 0)
    {
        return 0;
    }

    // split the PATH variable into seperate paths
    char* split_path = strtok(env_path, ENV_PATH_DELIM);
    while (split_path != 0)
    {
        char* joined_path = path_join(2, split_path, path);
        if (file = fopen(joined_path, "rb"))
        {
            new_path = joined_path;
            fclose(file);
            break;
        }
        free(joined_path);
        split_path = strtok(NULL, ENV_PATH_DELIM);
    }

    return new_path;
}