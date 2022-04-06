#pragma once

#include "ast.h"
#include "module.h"

ast_node* evaluate_expression(ast_node* node);//, ast_node* node1, ast_node* node2);

ast_node* simplify_expression(ast_node* node);
ast_node* initial_iteration(ast_node* node, hashtable* symbols);
ast_node* find_conditions(ast_node* node);
ast_node* remove_variables(ast_node* node);
ast_node* find_expressions(ast_node* node);

void optimize(module* mod, ast_node* node);