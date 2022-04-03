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
    
    mod->ast_root = ast_new(AstRoot);
    mod->dependencies = 0;
    mod->dependency_store = 0;

    mod->filename = 0;
    mod->dir = 0;
    mod->src_code = 0;

    return mod;
}

void module_free(module* mod)
{
    if (mod->dependency_store != 0)
    {
        for (size_t i = 0; i < mod->dependency_store->capacity; i++)
        {
            if (mod->dependency_store->keys[i] != 0)
            {
                module_free(mod->dependency_store->items[i]);
            }
        }
        hashtable_free(mod->dependency_store);
    }
    if (mod->dependencies != 0)
    {
        hashtable_free(mod->dependencies);
    }
    if (mod->dir != 0)
    {
        free(mod->dir);
    }

    free(mod->name);
    ast_free(mod->ast_root);
    free(mod->filename);
    free(mod->src_code);
    free(mod);
}

module* module_parse(char* filename, module* parent)
{
    // TODO : implement general file handling utils

    module* mod = module_new();
    mod->parent = parent;

    char* path = malloc(1024);
    path[0] = '\0';

    // combine filename with the parent's directory, if present
    if (parent != 0)
    {
        strcpy(path, parent->dir);
    }
    strcpy(path + strlen(path), filename);

    // seperate filename into directory and name
    char* s = strrchr(path, '/');
    if (s != 0)
    {
        char* temp = strdup(path);
        temp[s - path + 1] = '\0';
        mod->dir = strdup(temp);
        free(temp);

        // get rid of extension
        temp = strdup(s + 1);
        mod->filename = strdup(temp);
        s = strrchr(temp, '.');
        if (s != 0)
        {
            s[0] = '\0';
        }
        mod->name = strdup(temp);
        free(temp);
    }

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
    free(path);

    if (parser_status != 0)
    {
        printf("error: could not parse %s\n", filename);
        module_free(mod);
		return 0;
    }
    
    if (!preprocessor_process(mod))
    {
        module_free(mod);
		return 0;
    }
    
    return mod;
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
    bool new_file = file == 0;
    char* out_filename;

    if (new_file)
    {	    
        out_filename = malloc(strlen(mod->filename) + 10); // plus space for file extension
	    strcpy(out_filename, mod->filename);
	    strcat(out_filename, ".mod.json");
	
	    file = fopen(out_filename, "wb");
    }

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

    if (new_file)
    {
        fclose(file);
	    free(out_filename);
    }
}