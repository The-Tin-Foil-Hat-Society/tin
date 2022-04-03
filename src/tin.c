#include "ast.h"
#include "interpreter.h"
#include "module.h"

#include "parser.tab.h"
#include "lex.yy.h"

#include "exec.h"

#ifdef TIN_COMPILER
#include "backend/codegen.h"
#endif // TIN_COMPILER

#ifdef TIN_DEBUG_VERBOSE
#define print_step(...)     \
	fprintf(stderr, "-- "); \
	fprintf(stderr, __VA_ARGS__);
#else
#define print_step(...)
#endif

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("Usage: tin TIN_FILE\n");
		return 0;
	}

	module *mod = module_parse(argv[1], 0);

	if (mod == 0) // parsing failed
	{
		goto end;
	}
	module_print_to_file(mod, 0);

#ifdef TIN_DEBUG_OUTPUT_AST
	print_step("Writing AST\n");

	// output ast
	{
		// make the filename for the ast by appending to the given code file
		int orig_len = strlen(argv[1]);
		char *ast_filename = malloc(orig_len + 10); // plus space for file extension
		strcpy(ast_filename, argv[1]);
		strcat(ast_filename, ".ast.json");

		FILE *ast_file = fopen(ast_filename, "wb");
		ast_print_to_file(mod->ast_root, ast_file);
		fclose(ast_file);
		free(ast_filename);
	}
#endif // TIN_DEBUG_OUTPUT_AST

#ifdef TIN_INTERPRETER
	print_step("Running in interpreter mode\n");

	// interpret(mod, 0, 0); // not implemented yet . . .

#elif TIN_COMPILER
	print_step("Running in compiler mode\n");

	// Remove extension from file name
	char *filename = malloc(strlen(argv[1]) + 1);
	strcpy(filename, argv[1]);
	char *dot = strrchr(filename, '.');
	if (dot != NULL)
		*dot = '\0';

	// call the code generator
	{
		char *codegen_output_name = malloc(strlen(filename) + 3);
		strcpy(codegen_output_name, filename);
		strcat(codegen_output_name, ".s");

		FILE *compiled_file = fopen(codegen_output_name, "wb");
		codegen_generate(mod, mod->ast_root, compiled_file);

		print_step("Compiling %s to %s\n", argv[1], codegen_output_name);

		fclose(compiled_file);
		free(codegen_output_name);
	}

	// Run cross-assembler
	{
		print_step("Running assembler\n");

		// Assembler input name: "{filename}.s"
		// Assembler output name: "{filename}.o"
		char *asm_input_name = malloc(strlen(filename) + 3);
		strcpy(asm_input_name, filename);
		strcat(asm_input_name, ".s");

		char *asm_output_name = malloc(strlen(filename) + 3);
		strcpy(asm_output_name, filename);
		strcat(asm_output_name, ".o");

		print_step("Assembling %s to %s\n", asm_input_name, asm_output_name);

		char *asm_args[4] = {"/usr/bin/riscv64-linux-gnu-as", asm_input_name, "-o", asm_output_name};
		// Concatenate assembler arguments
		char *asm_args_str = malloc(strlen(asm_args[0]) + strlen(asm_args[1]) + strlen(asm_args[2]) + strlen(asm_args[3]) + 1);
		strcpy(asm_args_str, asm_args[0]);
		strcat(asm_args_str, " ");
		for (int i = 1; i < 4; i++)
		{
			strcat(asm_args_str, asm_args[i]);
			strcat(asm_args_str, " ");
		}

		exec(asm_args_str);

		free(asm_args_str);
	}

	// Run linker
	{
		print_step("Running linker\n");

		// Linker input name: "{filename}.o"
		// Linker output name: "{filename}.out"
		char *linker_input_name = malloc(strlen(filename) + 3);
		strcpy(linker_input_name, filename);
		strcat(linker_input_name, ".o");

		char *linker_output_name = malloc(strlen(filename) + 5);
		strcpy(linker_output_name, filename);
		strcat(linker_output_name, ".out");

		print_step("Linking %s to %s\n", linker_input_name, linker_output_name);

		char *linker_args[4] = {"/usr/bin/riscv64-linux-gnu-ld", linker_input_name, "-o", linker_output_name};
		// Concatenate linker arguments
		char *linker_args_str = malloc(strlen(linker_args[0]) + strlen(linker_args[1]) + strlen(linker_args[2]) + strlen(linker_args[3]) + 1);
		strcpy(linker_args_str, linker_args[0]);
		strcat(linker_args_str, " ");
		for (int i = 1; i < 4; i++)
		{
			strcat(linker_args_str, linker_args[i]);
			strcat(linker_args_str, " ");
		}

		exec(linker_args_str);

		free(linker_args_str);
	}
#endif

end:
	if (mod != 0)
	{
		module_free(mod);
	}

	return 0;
}
