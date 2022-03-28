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

// replaces all identifiers with shared symbols
void build_symbols(preproc_state* state, ast_node* node);
// process all nodes (after building symbols)
void process_nodes(preproc_state* state, ast_node* node);

bool preprocessor_process(module* mod, ast_node* node);

void optimize(module* mod, ast_node* node);  // optional