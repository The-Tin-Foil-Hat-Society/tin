#include "interpreter.h"
#include "module.h"

#include "parser.tab.h"
#include "lex.yy.h"

#include "exec.h"
#include "utils/path.h"

#ifdef TIN_COMPILER
#define ASM_PATH "/usr/bin/riscv64-linux-gnu-as"
#define LD_PATH "/usr/bin/riscv64-linux-gnu-ld"
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

	print_step("Parsing main module\n");
	module *mod = module_parse(argv[1], 0);

	if (mod == 0) // parsing failed
	{
		print_step("Parsing and preprocessing failed\n");
		goto end;
	}
	module_print_to_file(mod, 0);
	print_step("Parsed and preprocessed the program successfully\n");

#ifdef TIN_INTERPRETER
	print_step("Running in interpreter mode\n");

	// interpret(mod, 0, 0); // not implemented yet . . .

#elif TIN_COMPILER
	print_step("Running in compiler mode\n");

	// Get the module's path without the extension
	char* filename = path_join(2, mod->dir, mod->name);

	// call the code generator
	{
		char* codegen_output_name = path_join(2, filename, ".s");

		print_step("Compiling %s to %s\n", argv[1], codegen_output_name);

		FILE *compiled_file = fopen(codegen_output_name, "wb");
		codegen_generate(mod, compiled_file);

		fclose(compiled_file);
		free(codegen_output_name);
	}

	// Run cross-assembler
	{
		print_step("Running assembler\n");

		// Assembler input name: "{filename}.s"
		// Assembler output name: "{filename}.o"
		char* asm_input_name = path_join(2, filename, ".s");
		char* asm_output_name = path_join(2, filename, ".o");

		print_step("Assembling %s to %s\n", asm_input_name, asm_output_name);

		// Concatenate assembler arguments
		char *asm_args_str = malloc(strlen(ASM_PATH) + strlen(asm_input_name) + strlen(asm_output_name) + 8); // plus spaces and '-o'
		sprintf(asm_args_str, "%s %s -o %s", ASM_PATH, asm_input_name, asm_output_name);

		exec(asm_args_str);

		free(asm_args_str);
		free(asm_input_name);
		free(asm_output_name);
	}

	// Run linker
	{
		print_step("Running linker\n");

		// Linker input name: "{filename}.o"
		// Linker output name: "{filename}.out"
		char* linker_input_name = path_join(2, filename, ".o");
		char* linker_output_name = path_join(2, filename, ".out");

		print_step("Linking %s to %s\n", linker_input_name, linker_output_name);

		// Concatenate linker arguments
		char *linker_args_str = malloc(strlen(LD_PATH) + strlen(linker_input_name) + strlen(linker_output_name) + 8); // plus spaces and '-o'
		sprintf(linker_args_str, "%s %s -o %s", LD_PATH, linker_input_name, linker_output_name);

		exec(linker_args_str);

		free(linker_args_str);
		free(linker_input_name);
		free(linker_output_name);
	}
	
	free(filename);
#endif

end:
	if (mod == 0)
	{
		return 1; // failure
	}
	module_free(mod, 0);

	return 0;
}
