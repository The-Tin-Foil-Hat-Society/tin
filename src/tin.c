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

	module* mod = module_parse(argv[1], 0);

	if (mod == 0) // parsing failed
	{
		goto end;
	}
	module_print_to_file(mod, 0);

#ifdef TIN_INTERPRETER

	// interpret(mod, 0, 0); // not implemented yet . . .
	
#elif  TIN_COMPILER

	// call the code generator and stuff here

#endif

end:
	if (mod != 0)
	{
		module_free(mod);
	}

    return 0; 
}