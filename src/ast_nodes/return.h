#pragma once

#include "preprocessor.h"
#include "ast.h"
#include "symbol.h"
#include "data_type.h"

void preprocess_return(preproc_state* state, ast_node* node)
{
    // TODO: validate return type matches function
    return;
}