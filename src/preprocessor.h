#pragma once

#include "ast.h"
#include "module.h"
#include <stdbool.h>

typedef struct preproc_state preproc_state;
struct preproc_state
{
    module* mod;
    size_t error_counter;
};

preproc_state* preproc_state_new();
void preproc_state_free(preproc_state* state);

bool preprocessor_process(module* mod, ast_node* node);

#define preproc_error( state, node, fmt, ... ) \
    printf("%s\n", ast_find_closest_src_line(node)); \
    printf("preprocessor error: "); \
    printf(fmt, ##__VA_ARGS__ ); \
    state->error_counter += 1

#define preproc_warn( state, node, fmt, ... ) \
    printf("%s\n", ast_find_closest_src_line(node)); \
    printf("preprocessor warning: "); \
    printf(fmt, ##__VA_ARGS__ )
