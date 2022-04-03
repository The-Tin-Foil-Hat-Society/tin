#include <time.h>

#include "codegen.h"
#include "ast.h"

#include "backend/generators.h"

/*
 * Depth-first recursive tree traversal
 */
int codegen_traverse_ast(FILE *file, ast_node *node, int reg)
{
    trace("\nAST traversal - node: %s", ast_type_names[node->type]);
    int *regs = malloc(sizeof(int) * node->children->size);

    switch (node->type)
    {
    case AstFunction:
    {
        // Symbol is first child
        ast_node *child = vector_get_item(node->children, 0);
        gen_function(file, reg, child->value.symbol->name);
        break;
    }
    case AstFunctionCall:
    {
        // Symbol is first child
        ast_node *child = vector_get_item(node->children, 0);
        gen_function_call(file, reg, child->value.symbol->name);
        break;
    }
    }

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
    {
        char *name = "Lstr";
        name = malloc(sizeof(char) * (strlen(name) + 1));
        sprintf(name, "Lstr%d", rodata_count++);
        return gen_rodata_string(file, name, node->value.string);
    }

    //
    // Keywords
    //
    case AstPrint:
        return gen_print_string(file, rodata_count - 1, regs[0]);
    case AstAsm:
        return gen_asm(file, node->value.string, regs[0]);

    //
    // Functions
    //
    case AstFunction:
        return gen_function_epilogue(file, reg);
    case AstReturn:
        return gen_return(file, regs[0]);

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
        else if (node->parent->type == AstFunctionCall)
        {
            trace("\tFunction calls are not supported yet");
            return 0;
        }
        else
        {
            return gen_load_global(file, node->value.symbol->name);
        }
    }
    case AstAssignment:
        return regs[1];

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

    write_to_file(".globl _start\n");
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

    int reg;
    reg = codegen_traverse_ast(file, node, 0);
    write_postamble(file);

    // ASM
    write_to_file("_start:\n");
#ifdef TIN_DEBUG_VERBOSE
    write_to_file("\t# Function preamble\n");
#endif
    write_to_file("\taddi\tsp, sp, -16\n");
    write_to_file("\tsw\tra, 8(sp)\n");
    write_to_file("\tsw\tsp, 0(sp)\n");
    write_to_file("\taddi\ts0, sp, 16\n");
#ifdef TIN_DEBUG_VERBOSE
    write_to_file("\t# Call main function\n");
#endif
    write_to_file("\tcall\tmain\n");

    // Exit cleanly
#ifdef TIN_DEBUG_VERBOSE
    write_to_file("\t# Exit cleanly\n");
#endif
    write_to_file("\tli\ta0, 0\n");
    write_to_file("\tli\ta7, 93\n");
    write_to_file("\tecall\n");

    gen_rodata(file);
}