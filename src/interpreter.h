#pragma once

#include "ast.h"
#include "module.h"
#include "utils/vector.h"

bool interpret(module *mod, ast_node *node, vector *stack);
bool interpret_function(module *mod, ast_node *node, vector *stack);
bool interpret(module *mod, ast_node *node, vector *stack);