#include "ast.h"
#include "interpreter.h"
#include "module.h"

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
	if (!module_parse(mod, argv[1]))
	{
		goto end;
	}

#ifdef TIN_INTERPRETER

	// interpret(mod, 0, 0); // not implemented yet . . .

	// output ast

	// make the filename for the ast by appending to the given code file
	int orig_len = strlen(argv[1]);
	char* mod_filename = malloc(orig_len + 10); // plus space for file extension
	strcpy(mod_filename, argv[1]);
	strcat(mod_filename, ".mod.json");
	
	FILE* mod_file = fopen(mod_filename, "wb");
	module_print_to_file(mod, mod_file, true);
	fclose(mod_file);
	free(mod_filename);
#elif  TIN_COMPILER

	// call the code generator and stuff here

#endif

end:
	module_free(mod);

    return 0; 
}