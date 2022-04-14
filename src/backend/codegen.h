#pragma once

#include <stdio.h>

#include "ast.h"
#include "module.h"
#include "utils/vector.h"

bool codegen_generate(module *mod, ast_node *node, FILE *file);