#include "module.h"
#include "preprocessor.h"
#include "utils/path.h"
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
    mod->module_store = 0;

    mod->filename = 0;
    mod->dir = 0;
    mod->src_code = 0;

    return mod;
}

void module_free(module* mod, bool keep_symbols)
{
    if (mod == 0)
    {
        return;
    }

    if (mod->module_store != 0)
    {
        vector* modules_vec = hashtable_to_vector(mod->module_store);
        for (int i = 0; i < modules_vec->size; i++)
        {
            module_free(vector_get_item(modules_vec, i), keep_symbols);
        }
        vector_free(modules_vec);
        hashtable_free(mod->module_store);
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
    ast_free(mod->ast_root, keep_symbols);
    free(mod->filename);
    free(mod->src_code);
    free(mod);
}

module* module_parse(char* path, module* parent)
{
    module* mod = module_new();
    mod->parent = parent;

    /*
        we need to add the parent's (the file that includes this one)
        directory to the given path to accomodate subdirectories and
        ensure that the path stays relative to the working directory, 
        so we can can still fopen it.
    */
    char* parent_dir = 0;
    if (parent != 0)
    {
        parent_dir = parent->dir;
    }
    
    // if the include has no extension, add it back
    char* ext = strstr(path, ".tin");
    if (ext != 0 && (path + (int)strlen(path) - ext) == 4)
    {
        path = strdup(path);
    }
    else
    {
        path = path_join(2, path, ".tin");
    }

    char* src_path;
    if (!(src_path = path_locate_file(path, parent_dir)))
    {
        printf("error: could not find file %s\n", path);

        free(src_path);
        free(path);
        module_free(mod, 0);
        
        return false;
    }

    FILE* src_file = fopen(src_path, "rb");

    mod->dir = path_get_directory(src_path);
    mod->filename = path_get_filename(path);
    mod->name = path_get_filename_wo_ext(path);

	module_set_src_file(mod, src_file);

    yyscan_t scanner;

	yylex_init(&scanner);
	yyset_in(src_file, scanner); /* parse file in from arg*/

	int parser_status = yyparse(scanner, mod);

	yylex_destroy(scanner);

	fclose(src_file);
    free(src_path);
    free(path);

    if (parser_status != 0)
    {
        printf("error: could not lex and/or parse %s\n", path);
        module_free(mod, 0);
		return 0;
    }
    
    if (!preprocessor_process(mod))
    {
        module_free(mod, 0);
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

    if (mod->module_store == 0)
    {
        mod->module_store = hashtable_new();
    }
    hashtable_set_item(mod->module_store, dependency->name, dependency);
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

    if (mod->module_store == 0)
    {
        return 0;
    }

    return hashtable_get_item(mod->module_store, name);
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

void module_print_to_file(module* mod, FILE* file)
{
    bool new_file = file == 0;
    char* out_filename;

    if (new_file)
    {
        out_filename = path_join(3, mod->dir, mod->name, ".mod.json");
	
	    file = fopen(out_filename, "wb");
    }

    fprintf(file, "{\"name\": \"%s\", \"ast\":", mod->name);
    ast_print_to_file(mod->ast_root, file);

    if (mod->module_store != 0 && mod->module_store->size > 0)
    {
        fprintf(file, ",\"modules\": [");

        vector* modules_vec = hashtable_to_vector(mod->module_store);
        for (int i = 0; i < modules_vec->size; i++)
        {
            module_print_to_file(vector_get_item(modules_vec, i), file);
            if (i < modules_vec->size - 1) // don't print a comma after the last item
            {
                fprintf(file, ",");
            }
        }
        vector_free(modules_vec);

        fprintf(file, "]");
    }

    fprintf(file, "}");

    if (new_file)
    {
        fclose(file);
	    free(out_filename);
    }
}