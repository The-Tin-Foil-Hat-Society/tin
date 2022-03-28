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
