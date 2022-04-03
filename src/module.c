#include "module.h"
#include "preprocessor.h"
#include "parser.tab.h"
#include "lex.yy.h"
#include <stdlib.h>

module* module_new()
{
    module* mod = malloc(sizeof(module));

    mod->parent = 0;
    mod->name = 0;
    mod->dir = 0;
    mod->ast_root = ast_new(AstRoot);
    mod->dependencies = 0;
    mod->dependency_store = 0;
    mod->src_code = 0;

    return mod;
}

void module_free(module* mod)
{
    if (mod->dependencies != 0)
    {
        hashtable_free(mod->dependencies);
        hashtable_free(mod->dependency_store);
    }
    if (mod->dir != 0)
    {
        free(mod->dir);
    }

    free(mod->name);
    ast_free(mod->ast_root);
    free(mod->src_code);
    free(mod);
}

bool module_parse(module* mod, char* filename)
{
    char* path = malloc(1024);
    path[0] = '\0';
    if (mod->parent != 0)
    {
        strcpy(path, mod->parent->dir);
    }
    strcpy(path + strlen(path), filename);

    // get directory without filename, if it is present
    char* s = strrchr(path, '/');
    if (s != 0)
    {
        char* temp = strdup(path);
        temp[s - path + 1] = '\0';
        mod->dir = strdup(temp);
        free(temp);
    }

    printf("path: %s\n", path);
    printf("dir:  %s\n", mod->dir);

    FILE* src_file;
	if (!(src_file = fopen(path, "rb")))
	{
		printf("error: could not find file %s\n", filename);
		return false;
	}

	module_set_src_file(mod, src_file);

    yyscan_t scanner;

	yylex_init(&scanner);
	yyset_in(src_file, scanner); /* parse file in from arg*/

	int parser_status = yyparse(scanner, mod);

	yylex_destroy(scanner);
	fclose(src_file);

    if (parser_status != 0)
    {
        printf("error: could not parse %s\n", filename);
		return false;
    }
    
    return preprocessor_process(mod);
}

void module_add_dependency(module* mod, module* dependency)
{
    if (mod->dependencies == 0)
    {
        mod->dependencies = hashtable_new();
    }
    hashtable_set_item(mod->dependencies, dependency->name, dependency);

    while (mod->parent != 0)
    {
        mod = mod->parent;
    }

    if (mod->dependency_store == 0)
    {
        mod->dependency_store = hashtable_new();
    }
    hashtable_set_item(mod->dependency_store, dependency->name, dependency);
}

module* module_get_dependency(module* mod, char* name)
{
    if (mod->dependencies == 0)
    {
        return 0;
    }

    return hashtable_get_item(mod->dependencies, name);
}

module* module_find_dependency(module* mod, char* name)
{
    while(mod->parent != 0)
    {
        mod = mod->parent;
    }

    if (mod->dependency_store == 0)
    {
        return 0;
    }

    return hashtable_get_item(mod->dependency_store, name);
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

// prints a json representation of the module to the console
void module_print(module* mod, bool recursive)
{
    module_print_to_file(mod, stdout, recursive);
}

// prints a json representation of the module to the given file
void module_print_to_file(module* mod, FILE* file, bool recursive)
{
    fprintf(file, "{\"name\": \"%s\", \"ast\":", mod->name);
    ast_print_to_file(mod->ast_root, file, recursive);

    if (mod->dependency_store != 0 && mod->dependency_store->size > 0)
    {
        fprintf(file, ",\"dependencies\": [");

        vector* dependency_vec = hashtable_to_vector(mod->dependency_store);
        for (int i = 0; i < dependency_vec->size; i++)
        {
            module_print_to_file(vector_get_item(dependency_vec, i), file, recursive);
            if (i < dependency_vec->size - 1) // don't print a comma after the last item
            {
                fprintf(file, ",");
            }
        }

        fprintf(file, "]");
    }

    fprintf(file, "}");
}