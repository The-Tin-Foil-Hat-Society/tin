#include "backend/common.h"

void _emit_comment(FILE *file, const char *fmt, ...)
{
#ifdef TIN_DEBUG_VERBOSE
    va_list args;
    va_start(args, fmt);
    fprintf(file, "\t# ");
    vfprintf(file, fmt, args);
    va_end(args);
#endif
}

void _emit(FILE *file, const char *comment, const char *opcode, const char *operand_fmt, ...)
{
    va_list args;

    va_start(args, operand_fmt);
#ifdef TIN_DEBUG_VERBOSE
    char *comment_prefixed = malloc(strlen(comment) + 32);
    sprintf(comment_prefixed, " # %s", comment);

    char *operand_formatted = malloc(strlen(operand_fmt) + 1024);
    vsprintf(operand_formatted, operand_fmt, args);

    char *instruction = malloc(strlen(opcode) + strlen(operand_formatted) + 32);
    sprintf(instruction, "%-8s %s", opcode, operand_formatted);

    fprintf(file, "\t%-32s %-32s\n", instruction, comment_prefixed);

    free(comment_prefixed);
    free(operand_formatted);
    free(instruction);
#else
    char *operand_formatted = malloc(strlen(operand_fmt) + 1024);
    vsprintf(operand_formatted, operand_fmt, args);

    char *instruction = malloc(strlen(opcode) + strlen(operand_formatted) + 2);
    sprintf(instruction, "%s %s", opcode, operand_formatted);

    fprintf(file, "\t%s\n", instruction);

    free(operand_formatted);
    free(instruction);
#endif
    va_end(args);
}