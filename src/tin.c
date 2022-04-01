#include "ast.h"
#include "interpreter.h"
#include "module.h"
#include "optimisation.h"
#include "preprocessor.h"

#include "parser.tab.h"
#include "lex.yy.h"

int main(int argc, char** argv)
{ 
	if (argc < 2)
	{
		printf("Usage: tin TIN_FILE\n");
		return 0;
	}

	module* mod = module_new();
	if (module_parse(mod, argv[1]) != 0)
	{
		goto end;
	}

	// preprocessing

	if (!preprocessor_process(mod, mod->ast_root))
	{
		goto end;
	}
	optimize(mod, mod->ast_root);

#ifdef TIN_INTERPRETER

	// interpret(mod, mod->ast_root, 0); // not implemented yet . . .

	// output ast

	// make the filename for the ast by appending to the given code file
	int orig_len = strlen(argv[1]);
	char* ast_filename = malloc(orig_len + 10); // plus space for file extension
	strcpy(ast_filename, argv[1]);
	strcat(ast_filename, ".ast.json");
	
	FILE* ast_file = fopen(ast_filename, "wb");
	ast_print_to_file(mod->ast_root, ast_file, true);
	fclose(ast_file);
	free(ast_filename);
#elif  TIN_COMPILER

	// call the code generator and stuff here

#endif

end:
	module_free(mod);

    return 0; 
}