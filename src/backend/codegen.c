#include <time.h>

#include "codegen.h"
#include "ast.h"

#include "backend/generators.h"

/*
 * Depth-first recursive tree traversal
 */
int codegen_traverse_ast(FILE *file, ast_node *node, int reg)
{
    int *regs = malloc(sizeof(int) * node->children->size);

    // HACK for something I shouldn't really need to do...
    if (node->type == AstAssignment)
    {
        // Reverse order
        for (int i = node->children->size - 1; i >= 0; i--)
        {
            ast_node *child = vector_get_item(node->children, i);
            regs[i] = codegen_traverse_ast(file, child, reg);
        }
    }
    else
    {
        for (int i = 0; i < node->children->size; i++)
        {
            ast_node *child = vector_get_item(node->children, i);
            regs[i] = codegen_traverse_ast(file, child, reg);
        }
    }

    trace("\nAST traversal - node: %s", ast_type_names[node->type]);
    switch (node->type)
    {
    //
    // Operators
    //
    case AstAdd:
        return gen_add(file, regs[0], regs[1]);
    case AstSub:
        return gen_sub(file, regs[0], regs[1]);
    case AstMul:
        return gen_mul(file, regs[0], regs[1]);
    case AstDiv:
        return gen_div(file, regs[0], regs[1]);

    //
    // Literals
    //
    case AstIntegerLit:
        return gen_load(file, node->value.integer);
    case AstStringLit:
        return gen_rodata_string(file, "Lstr0", node->value.string);

    //
    // Keywords
    //
    case AstPrint:
        return gen_print(file, "Lstr0");

    //
    // Variables
    //
    case AstSymbol:
    {
        if (node->parent->type == AstAssignment)
        {
            return gen_store_global(file, reg, node->value.symbol->name);
        }
        else if (node->parent->type == AstFunction)
        {
            trace("\tFunctions are not supported yet");
            return 0;
        }
        else
        {
            return gen_load_global(file, node->value.symbol->name);
        }
    }
    case AstAssignment:
        return regs[1];

    case AstReturn:
        trace("\tReturning from function");
        return regs[0];

    default:
        trace("Unhandled node type: %s", ast_type_names[node->type]);
        break;
    }

    return 0;
}

void codegen_init()
{
    register_freeall();
    variable_init();
    rodata_init();
}

void write_preamble(FILE *file)
{
    trace("Writing preamble");

    write_to_file(".globl __start\n");
}

void write_postamble(FILE *file)
{
    trace("Writing postamble");
}

bool codegen_generate(module *mod, ast_node *node, FILE *file)
{
    codegen_init();
    write_preamble(file);

    // Text section
    write_to_file(".text\n\n");

    // ASM
    write_to_file("__start:\n");
    write_to_file("\t# Function preamble\n");
    write_to_file("\taddi\tsp, sp, -32\n");
    write_to_file("\tsw\tsp, 24(sp)\n");
    write_to_file("\taddi\ts0, sp, 32\n");

    int reg;
    reg = codegen_traverse_ast(file, node, 0);
    gen_printint(file, reg);

    write_postamble(file);
    gen_rodata(file);
}