#pragma once

#include "ast.h"
#include "module.h"

int eval_condition(ast_node* condition, ast_node* node1, ast_node* node2);
ast_node* find_conditions(ast_node* node);
void optimize(module* mod, ast_node* node);