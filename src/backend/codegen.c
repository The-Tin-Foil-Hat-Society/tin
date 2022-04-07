#include "codegen.h"
#include "ast.h"

#include "backend/generators.h"

int codegen_traverse_ast(FILE *file, ast_node *node, int reg);

/*
 * "overrides" original tree traversal, write labels etc. for
 * this if statement
 */
static int codegen_generate_if_ast(FILE *file, ast_node *node)
{
    int false_label, else_label;
    ast_node *left, *right, *mid;

    left = (ast_node *)ast_get_child(node, 0); // Comparison
    mid = (ast_node *)ast_get_child(node, 1);  // Condition

    right = (ast_node *)ast_get_child(node, 2); // Else
    bool has_else = (right != NULL);

    trace("\tProcessing if statement");
    trace("\t\t* Comparison node type: %s", ast_type_names[left->type]);
    trace("\t\t* Condition node type: %s", ast_type_names[mid->type]);

    if (has_else)
        trace("\t\t* Else node type: %s", ast_type_names[right->type]);

    false_label = label_add();
    trace("\t\t* False label: %d", false_label);

    if (has_else)
    {
        else_label = label_add();
        trace("\t\t* End label: %d", else_label);
    }

    codegen_traverse_ast(file, left, false_label);
    register_freeall();

    codegen_traverse_ast(file, mid, 0);
    register_freeall();

    if (has_else)
    {
        gen_jump(file, else_label);
    }

    gen_label(file, false_label);

    if (has_else)
    {
        codegen_traverse_ast(file, right, 0);
        register_freeall();
        gen_label(file, else_label);
    }
    return 0;
}

int *regs;
/*
 * Depth-first recursive tree traversal
 */
int codegen_traverse_ast(FILE *file, ast_node *node, int reg)
{
    trace("\n-- AST traversal - node: %s\n-- Log:", ast_type_names[node->type]);

    trace("\tAllocating %ld registers", node->children->size);
    if (regs != NULL)
        free(regs);
    regs = malloc(sizeof(int) * node->children->size + 1);

    switch (node->type)
    {
    case AstIf:
    {
        return codegen_generate_if_ast(file, node);
    }
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
    if (node->type == AstAssignment || node->type == AstIf)
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

    trace("-- Processed children for %s, processing node", ast_type_names[node->type]);

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
    case AstPow:
        // TODO
        // This one will be difficult, we're going to need loops first
    case AstMod:
        return gen_mod(file, regs[0], regs[1]);

    //
    // Comparisons
    //
    case AstEqual:
    case AstNotEqual:
    case AstLessThan:
    case AstGreaterThanOrEqual:
    case AstLessThanOrEqual:
    case AstGreaterThan:
    {
        if (node->parent->type == AstIf)
        {
            return gen_comparison_jump(file, node->type, regs[0], regs[1], reg);
        }
        else
        {
            compiler_error("TODO: compare and assign bool\n");
        }
        break;
    }

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
            return gen_store_global(file, reg, node->value.symbol->name, node->value.symbol->dtype->size);
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
        else if (node->parent->type == AstDefinitionList)
        {
            // Do nothing
            return 0;
        }
        else
        {
            trace("\tLoading (parent type: %s)", ast_type_names[node->parent->type]);
            return gen_load_global(file, node->value.symbol->name, node->value.symbol->dtype->size);
        }
    }
    case AstAssignment:
        return regs[1];

    default:
        trace("\t!! Unhandled node type: %s", ast_type_names[node->type]);
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

    gen_rodata(file);
}

bool codegen_generate(module *mod, ast_node *node, FILE *file)
{
    codegen_init();
    write_preamble(file);

    // Text section
    write_to_file(".text\n\n");

    int reg;
    reg = codegen_traverse_ast(file, node, 0);

    // TODO: Clean up
    // ASM
    write_to_file("\t# Program entry point\n");
    write_to_file("_start:\n");
#ifdef TIN_DEBUG_VERBOSE
    write_to_file("\t# Function prologue\n");
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
    write_to_file("\t# Exit program cleanly\n");
#endif
    write_to_file("\tli\ta0, 0\n");
    write_to_file("\tli\ta7, 93\n");
    write_to_file("\tecall\n");

    write_postamble(file);
}