#include <time.h>

#include "codegen.h"
#include "ast.h"

#include "backend/common.h"
#include "backend/generators.h"
#include "backend/builtin/rodata.h"

/*
 * Depth-first recursive tree traversal
 */
int codegen_traverse_ast(FILE *file, ast_node *node)
{
    int left, right;

    ast_node *left_node = ast_get_child(node, 0);
    ast_node *right_node = ast_get_child(node, 1);

    if (left_node)
        left = codegen_traverse_ast(file, left_node);
    if (right_node)
        right = codegen_traverse_ast(file, right_node);

    switch (node->type)
    {
    case AstAdd:
        return gen_add(file, left, right);
    case AstSub:
        return gen_sub(file, left, right);
    case AstMul:
        return gen_mul(file, left, right);
    case AstDiv:
        return gen_div(file, left, right);
    case AstIntegerLit:
        return gen_load(file, node->value.integer);

    default:
        trace("Unknown node type: %s\n", ast_type_names[node->type]);
        break;
    }

    return 0;
}

void codegen_init()
{
    register_freeall();
    instructions_init();
    rodata_init();
}

void write_preamble(FILE *file)
{
    emit(".globl __start\n\n");
    rodata_write(file);
}

void write_postamble(FILE *file)
{
}

bool codegen_generate(module *mod, ast_node *node, FILE *file)
{
    codegen_init();
    write_preamble(file);

    // Text section
    write_to_file(".text\n\n");

    // ASM
    write_to_file("__start:\n");
    int reg;
    reg = codegen_traverse_ast(file, node);
    gen_printint(file, reg);
}