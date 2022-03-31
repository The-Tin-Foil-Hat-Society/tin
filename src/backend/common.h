#pragma once

#include "ast.h"

#include "backend/syscalls.h"
#include "backend/builtin/instructions.h"
#include "backend/builtin/variable.h"
#include "backend/builtin/rodata.h"

#include <stdarg.h>
#include <stdbool.h>

#define emit(fmt, ...) \
    fprintf(file, fmt, ##__VA_ARGS__);

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
