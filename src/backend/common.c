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
    fprintf(file, "\t%s\t", opcode);
    vfprintf(file, operand_fmt, args);
    va_end(args);

    _emit_comment(file, comment);
    fprintf(file, "\n");
}