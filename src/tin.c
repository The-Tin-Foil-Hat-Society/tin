#include "module.h"
#include "ast.h"
#include "interpreter.h"
#include "pre_processor.h"
#include "parser.tab.h" // always include parser before lexer to avoid circular dependency
#include "lex.yy.h"

int main(int argc, char** argv)
{ 
	if (argc < 2)
	{
		printf("Usage: tin TIN_FILE [AST_FILE]\n");
		return 0;
	}

	FILE* src_file = fopen(argv[1], "rb");

	module* mod = module_new();
	module_set_src_file(mod, src_file);

	// lexing and parsing

    yyscan_t scanner;

	yylex_init(&scanner);
	yyset_in(src_file, scanner); /* parse file in from arg*/

	yyparse(scanner, mod);

	yylex_destroy(scanner);
	fclose(src_file);

	// preprocessing

	int error_count = build_symbols(mod, mod->ast_root);
	if (error_count > 0)
	{
		printf("total %d errors\n", error_count);
		goto free;
	}

	optimize(mod, mod->ast_root);

#ifdef TIN_INTERPRETER
	interpret(mod, mod->ast_root);
#elif  TIN_COMPILER
	// call the code generator and stuff here
#endif

	// output

	char* ast_filename;
	if (argc > 2)
	{
		ast_filename = strdup(argv[2]);
	}
	else
	{
		// if no filename is given for the ast, make our own
		int orig_len = strlen(argv[1]);
		ast_filename = malloc(orig_len + 10); // plus space for file extension
		strcpy(ast_filename, argv[1]);
		strcat(ast_filename, ".ast.json");
	}

	// save ast
	FILE* ast_file = fopen(ast_filename, "wb");
	ast_print_to_file(mod->ast_root, ast_file, true);

free:
	// free everything
	fclose(ast_file);
	free(ast_filename);
	module_free(mod);

    return 0; 
}