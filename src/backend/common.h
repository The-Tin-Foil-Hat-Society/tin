#pragma once

#include "ast.h"

#include "backend/syscalls.h"

#include <stdarg.h>
#include <stdbool.h>

void _emit_comment(FILE *file, const char *fmt, ...);
void _emit(FILE *file, const char *comment, const char *opcode, const char *operand, ...);

#define emit_comment(...) \
    _emit_comment(file, __VA_ARGS__)

#define emit(comment, opcode, operand, ...) \
    _emit(file, comment, opcode, operand, ##__VA_ARGS__)

#ifdef TIN_DEBUG_VERBOSE
#define trace(...)       \
    printf(__VA_ARGS__); \
    printf("\n")
#else
#define trace(...)
#endif // TIN_DEBUG_VERBOSE

#define write_to_file(...) \
    fprintf(file, __VA_ARGS__)

#define compiler_error(...) \
    fprintf(stderr, "Error: " __VA_ARGS__);
