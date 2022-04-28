#pragma once

#include "ast.h"
#include "module.h"
#include <math.h>

void reclass_as_bool(ast_node* parent, int child_index, bool value);
void reclass_as_num(ast_node* parent, int child_index, float result, bool is_integer);
void reclass_node(ast_node* parent, int child_index);

void evaluate_expression(ast_node* parent, int child_index, bool determinable);
void assign_variable(ast_node* node, bool determinable);
void simplify_expression(ast_node* parent, int child_index, bool determinable);

void replace_if_statements(ast_node* node);
void find_expressions(ast_node* parent, int child_index, bool determinable);
void reset_variables(ast_node* node);

void optimise(ast_node* node);