#pragma once

#include "_globals.h"
#include "ast.h"
#include "module.h"
#include <stdbool.h>

typedef struct preproc_state preproc_state;
struct preproc_state
{
    module* mod;
    size_t error_counter;
    int32_t index_offset;
};

preproc_state* preproc_state_new();
void preproc_state_free(preproc_state* state);

bool preprocessor_process(module* mod);

#define preproc_error( state, node, fmt, ... ) \
    fprintf(stderr, "%s: %s\n", state->mod->name, ast_find_closest_src_line(node)); \
    fprintf(stderr, "preprocessor error: "); \
    fprintf(stderr, fmt, ##__VA_ARGS__ ); \
    state->error_counter += 1

#define preproc_verb( state, node, fmt, ... ) \
    if (tin_verbose) \
    { \
        printf("%s: %s\n", state->mod->name, ast_find_closest_src_line(node)); \
        printf("preprocessor verb: "); \
        printf(fmt, ##__VA_ARGS__ ); \
    }

#define preproc_warn( state, node, fmt, ... ) \
    printf("%s: %s\n", state->mod->name, ast_find_closest_src_line(node)); \
    printf("preprocessor warning: "); \
    printf(fmt, ##__VA_ARGS__ )

#define min( a, b ) (((a) < (b)) ? (a) : (b))