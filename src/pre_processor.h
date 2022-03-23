#pragma once

#include "module.h"
#include "ast.h"
#include <stdbool.h>

int build_symbols(module* mod, ast_node* node);
void optimize(module* mod, ast_node* node);  // optional