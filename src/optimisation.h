#pragma once

#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wuninitialized"
#pragma GCC diagnostic ignored "-Warray-bounds"
#pragma GCC diagnostic ignored "-Wreturn-type"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wswitch"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#pragma GCC diagnostic ignored "-Wformat-security"

#include "ast.h"
#include "module.h"
#include <math.h>

void reclass_as_bool(ast_node* parent, int child_index, bool value);
void reclass_as_num(ast_node* parent, int child_index, float result, bool is_integer);
void reclass_node(ast_node* parent, int child_index);

void evaluate_expression(ast_node* parent, int child_index, bool determinable);
void assign_variable(ast_node* node, bool determinable);
void simplify_expression(ast_node* parent, int child_index, bool determinable);

void replace_if_statements(ast_node* node, bool determinable);
void find_expressions(ast_node* parent, int child_index, bool determinable);
void reset_variables(ast_node* node);

void optimise(module* mod, ast_node* node);