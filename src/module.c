#include "module.h"
#include <stdlib.h>

module* module_new(void)
{
    module* mod = malloc(sizeof(module));

    mod->ast_root = ast_new(AstRoot);
    mod->src_code = 0;

    return mod;
}

void module_free(module* mod)
{
    ast_free(mod->ast_root);
    free(mod->src_code);
    free(mod);
}

void module_set_src_file(module* mod, FILE* file)
{
    fseek(file, 0, SEEK_SET); // in case its been read already

    fseek(file, 0, SEEK_END);
    size_t len = ftell(file); // get number of bytes read
    fseek(file, 0, SEEK_SET);

    mod->src_code = malloc(len + 1);
    size_t _ = fread(mod->src_code, len, 1, file);
    mod->src_code[len] = '\0';

    fseek(file, 0, SEEK_SET);
}

char* module_get_src_line(module* mod, int linneno)
{
    if (mod->src_code == 0)
    {
        return 0;
    }

    int len = strlen(mod->src_code);

    char* line = malloc(512);
    int line_i = 0;
    int linneno_ = 1;

    line_i += sprintf(line, "line %d: ", linneno);

    for (int i = 0; i < len - 1; i++)
    {
        if (linneno_ == linneno && mod->src_code[i] != '\r' && mod->src_code[i] != '\n')
        {
            if (mod->src_code[i] == 0x22 || mod->src_code[i] == 0x27) // escape quotes and apostrophes
            {
                line[line_i] = 0x5c; // backslash
                line_i += 1;
            }

            if (mod->src_code[i] == ' ' && mod->src_code[i+1] == ' ') // ignore multiple spaces
            {
                i += 3;
                continue;
            }

            line[line_i] = mod->src_code[i];
            line_i += 1;
        }

        if (mod->src_code[i] == '\n')
        {
            linneno_ += 1;
        }
    }

    line[line_i] = '\0'; // -1 since that's the newline char and we don't want to keep it

    char* ret = 0;
    if (line_i > 0)
    {
        ret = strdup(line);
    }

    free(line);
    return ret;
}