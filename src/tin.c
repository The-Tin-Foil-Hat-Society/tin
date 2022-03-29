#include "ast.h"
#include "interpreter.h"
#include "module.h"
#include "optimisation.h"
#include "preprocessor.h"

#include "parser.tab.h" // always include parser before lexer to avoid circular dependency
#include "lex.yy.h"

#ifdef TIN_COMPILER
#include "backend/codegen.h"
#endif // TIN_COMPILER

int main(int argc, char** argv)
{ 
	if (argc < 2)
	{
		printf("Usage: tin TIN_FILE\n");
		return 0;
	}

	FILE* src_file = fopen(argv[1], "rb");

	module* mod = module_new();
	module_set_src_file(mod, src_file);

	// lexing and parsing

    yyscan_t scanner;

	yylex_init(&scanner);
	yyset_in(src_file, scanner); /* parse file in from arg*/

	int parser_status = yyparse(scanner, mod);

	yylex_destroy(scanner);
	fclose(src_file);

	if (parser_status != 0)
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
	printf( "Running in interpreter mode\n" );

	// interpret(mod, mod->ast_root, 0); // not implemented yet . . .

#elif TIN_COMPILER
	printf( "Running in compiler mode\n" );

	// call the code generator
	{
		int orig_len = strlen(argv[1]);
		char* compiled_filename = malloc(orig_len + 3);
		strcpy(compiled_filename, argv[1]);
		strcat(compiled_filename, ".s");
		
		FILE* compiled_file = fopen(compiled_filename, "wb");
		codegen_generate(mod, mod->ast_root, compiled_file);

		fclose(compiled_file);
		free(compiled_filename);
	}
#endif

#ifdef TIN_DEBUG_OUTPUT_AST
	printf( "Writing AST\n" );
	
	// output ast
	{
		// make the filename for the ast by appending to the given code file
		int orig_len = strlen(argv[1]);
		char* ast_filename = malloc(orig_len + 10); // plus space for file extension
		strcpy(ast_filename, argv[1]);
		strcat(ast_filename, ".ast.json");
		
		FILE* ast_file = fopen(ast_filename, "wb");
		ast_print_to_file(mod->ast_root, ast_file, true);
		fclose(ast_file);
		free(ast_filename);
	}
#endif // TIN_DEBUG_OUTPUT_AST

end:
	module_free(mod);

    return 0; 
}