#pragma once

#include "ast.h"

#include "backend/syscalls.h"

#include <stdarg.h>
#include <stdbool.h>

void _emit_comment(FILE *file, const char *fmt, ...);
void _emit(FILE *file, const char *comment, const char *opcode, const char *operand, ...);

#ifdef TIN_DEBUG_VERBOSE
#define trace(...)       \
    printf(__VA_ARGS__); \
    printf("\n")

#define emit_comment(...) \
    _emit_comment(file, __VA_ARGS__)
#else
#define trace(...)
#define emit_comment(...)
#endif // TIN_DEBUG_VERBOSE

#define emit(comment, opcode, operand, ...) \
    _emit(file, comment, opcode, operand, ##__VA_ARGS__)

#define write_to_file(...) \
    fprintf(file, __VA_ARGS__)

#define compiler_error(...) \
    fprintf(stderr, "Error: " __VA_ARGS__);
