#include "interpreter.h"
#include "module.h"

#include "parser.tab.h"
#include "lex.yy.h"

#include "exec.h"
#include "utils/path.h"

#include "_globals.h"
bool tin_verbose;

#ifdef TIN_COMPILER
#define ASM_PATH "/usr/bin/riscv64-linux-gnu-as"
#define LD_PATH "/usr/bin/riscv64-linux-gnu-ld"
#include "backend/codegen.h"
#endif // TIN_COMPILER

#define print_step(...)     \
	if (tin_verbose) \
	{ \
		fprintf(stderr, "-- "); \
		fprintf(stderr, __VA_ARGS__); \
	}

void arg_help(void)
{
	printf("Usage: tin [options] <file>\nOptions:\n");
	printf(" -h/--help\t\tDisplay this message.\n");
	printf(" -o/--output <file>\tOutput to <file>.\n");
	printf(" -v/--version\t\tDisplays compiler version info.\n");
	printf(" --verbose\t\tDisplays verbose output.\n");
	printf(" -O<level>\t\tSet optimisation level between 0-1.\n");
	printf(" -c\t\t\tCompile without linking.\n");
	printf(" -s\t\t\tCompile without assembling.\n");
#ifdef GIT_ORIGIN
	printf("\norigin: %s\n", GIT_ORIGIN);
#endif
}

void arg_version(void)
{
#ifdef TIN_RELEASE
	printf("-D TIN_RELEASE\n");
#endif
#ifdef TIN_DEBUG
	printf("-D TIN_DEBUG\n");
#endif
#ifdef TIN_DEBUG_VERBOSE
	printf("-D TIN_DEBUG_VERBOSE\n");
#endif
#ifdef BUILD_TIME
	printf("BUILD_TIME:\t%s\n", BUILD_TIME);
#endif
#ifdef GIT_VERSION
	printf("VERSION:\t%s\n", GIT_VERSION);
#endif
#ifdef GIT_ORIGIN
	printf("ORIGIN:\t\t%s\n", GIT_ORIGIN);
#endif
}

int main(int argc, char **argv)
{	
	tin_verbose = false;
	int code = 0;

	if (argc < 2)
	{
		arg_help();
		return 0;
	}

	char* input_file = 0;
	char* output_file = 0;

	bool arg_c = false;
	bool arg_s = false;
	int arg_O = 0;
	// parse arguments
	for (int i = 1; i < argc; i++)
	{
		// last arg must be the input file and not an option
		if (i == argc - 1 && argv[i][0] != '-')
		{
			input_file = argv[i];
		}
		else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0)
		{
			arg_help();
			return 0;
		}
		else if ((strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) && i < argc - 1 && argv[i + 1][0] != '-') // check that the next arg isn't an opt
		{
			output_file = argv[i + 1];
			i += 1; // skip the next arg since its our file
		}
		else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0)
		{
			arg_version();
			return 0;
		}
		else if (strcmp(argv[i], "--verbose") == 0)
		{
			tin_verbose = true;
		}
		// optimizer option
		else if (strlen(argv[i]) == 3 && argv[i][0] == '-' && argv[i][1] == 'O')
		{
			arg_O = atoi(argv[i + 2]); // should only have a numeral after -O
			if (arg_O > 1)
			{
				printf("-O option must be either 0 and 1\n");
			}
		}
		else if (strcmp(argv[i], "-c") == 0)
		{
			arg_c = true;
		}
		else if (strcmp(argv[i], "-s") == 0)
		{
			arg_s = true;
		}
		else
		{
			printf("invalid argument: %s\n", argv[i]);
			return 1;
		}
	}

	if (arg_s && arg_c)
	{
		printf("can only supply one of -c or -s arguments but not both\n");
		return 1;
	}

	if (input_file == 0)
	{
		printf("no input file given!\n");
		return 1;
	}

	print_step("Parsing %s\n", input_file);
	module *mod = module_parse(input_file, 0);

	/*
	if (arg_O > 0)
	{
		optimize(mod, mod->ast_root);
	}
	*/

	if (mod == 0) // parsing failed
	{
		print_step("Parsing and preprocessing failed\n");
		return 1; // no need to cleanup here
	}

#ifndef TIN_RELEASE
	module_print_to_file(mod, 0);
#endif

	print_step("Parsed and preprocessed the program successfully\n");

#ifdef TIN_INTERPRETER
	print_step("Running in interpreter mode\n");

	// interpret(mod, 0, 0); // not implemented yet . . .

#elif TIN_COMPILER
	print_step("Running in compiler mode\n");

	// Get the module's path without the extension
	char* filename = path_join(2, mod->dir, mod->name);
	char* asm_filename = path_join(2, filename, ".s");
	char* obj_filename = path_join(2, filename, ".o");

	if (output_file == 0)
	{
		output_file = filename;
	}

	// call the code generator
	{
		print_step("Compiling %s to %s\n", argv[1], asm_filename);

		FILE *compiled_file = fopen(asm_filename, "wb");
		bool codegen_res = codegen_generate(mod, compiled_file);
		fclose(compiled_file);

		if (codegen_res == false)
		{
			code = 1;
			goto compiler_end;
		}
	}

	if (arg_s)
	{
		goto compiler_end;
	}

	// Run cross-assembler
	{
		print_step("Running assembler\n");
		print_step("Assembling %s to %s\n", asm_filename, obj_filename);

		// Concatenate assembler arguments
		char *asm_args_str = malloc(strlen(ASM_PATH) + strlen(asm_filename) + strlen(obj_filename) + 8); // plus spaces and '-o'
		sprintf(asm_args_str, "%s %s -o %s", ASM_PATH, asm_filename, obj_filename);
		exec(asm_args_str);
		free(asm_args_str);
	}

	if (arg_c)
	{
		goto compiler_end;
	}

	// Run linker
	{
		print_step("Running linker\n");
		print_step("Linking %s to %s\n", obj_filename, output_file);

		// Concatenate linker arguments
		char *linker_args_str = malloc(strlen(LD_PATH) + strlen(obj_filename) + strlen(output_file) + 8); // plus spaces and '-o'
		sprintf(linker_args_str, "%s %s -o %s", LD_PATH, obj_filename, output_file);
		exec(linker_args_str);
		free(linker_args_str);
	}
	
	print_step("%s compiled\n", output_file);

compiler_end:

#ifdef TIN_RELEASE
// delete intermediate files
	if (!arg_s)
	{
		remove(asm_filename);
	}
	if (!arg_c)
	{
		remove(obj_filename);
	}
#endif

	free(filename);
	free(asm_filename);
	free(obj_filename);

#endif

	if (mod != 0)
	{
		module_free(mod, 0);
	}

	return code;
}
