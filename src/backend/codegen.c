#include <math.h>

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

static int codegen_generate_while_ast(FILE *file, ast_node *node)
{
    int start_label, end_label;

    ast_node *left, *right;

    left = (ast_node *)ast_get_child(node, 0);  // Comparison
    right = (ast_node *)ast_get_child(node, 1); // Block

    start_label = label_add();
    end_label = label_add();

    trace("\tProcessing while statement");
    trace("\t\t* Comparison node type: %s", ast_type_names[left->type]);
    trace("\t\t* Block node type: %s", ast_type_names[right->type]);
    trace("\t\t* Start label: %d", start_label);
    trace("\t\t* End label: %d", end_label);

    emit_comment("While loop start\n");
    gen_label(file, start_label);

    codegen_traverse_ast(file, left, end_label);
    register_freeall();

    codegen_traverse_ast(file, right, 0);
    register_freeall();

    emit_comment("Jump to while loop start\n");
    gen_jump(file, start_label);

    emit_comment("While loop end\n");
    gen_label(file, end_label);

    return 0;
}

#define MAX_REGISTERS 32

int *codegen_alloc_registers()
{
    /*
     * This is a pretty hacky way to ensure that
     * memory gets freed here - regs is actually used
     * on a per-node basis
     */
    trace("\tAllocating %d registers", MAX_REGISTERS);
    static int *regs;

    if (regs == NULL)
        regs = calloc(MAX_REGISTERS, sizeof(int));
    else
        memset(regs, 0, MAX_REGISTERS * sizeof(int));

    return regs;
}

/*
 * Depth-first recursive tree traversal
 */
int codegen_traverse_ast(FILE *file, ast_node *node, int reg)
{
    trace("\n-- AST traversal - node: %s\n-- Log:", ast_type_names[node->type]);

    int *regs = codegen_alloc_registers();

    switch (node->type)
    {
    case AstIf:
    {
        return codegen_generate_if_ast(file, node);
    }
    case AstWhile:
    {
        return codegen_generate_while_ast(file, node);
    }
    case AstFunction:
    {
        // Symbol is first child
        ast_node *child = vector_get_item(node->children, 0);
        gen_function(file, reg, child->value.symbol->key);
        break;
    }
    case AstFunctionCall:
    {
        // Symbol is first child
        ast_node *child = vector_get_item(node->children, 0);
        gen_function_call(file, reg, child->value.symbol->key);
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
        if (node->parent->type == AstIf || node->parent->type == AstWhile)
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
        int max_digits = 32;
        name = malloc(sizeof(char) * (strlen(name) + max_digits + 1));

        sprintf(name, "Lstr%d", rodata_count++);
        int ret = gen_rodata_string(file, name, node->value.string);
        free(name);
        return ret;
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
    {
        bool is_at_end = ast_get_child_index(node->parent, node) == node->parent->children->size - 1;
        return gen_return(file, regs[0], is_at_end);
    }

    //
    // Variables
    //
    case AstSymbol:
    {
        if (node->parent->type == AstAssignment)
        {
            return gen_store_global(file, reg, node->value.symbol->key, node->value.symbol->dtype->size);
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
            return gen_load_global(file, node->value.symbol->key, node->value.symbol->dtype->size);
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

void codegen_write_preamble(FILE *file)
{
    trace("Writing preamble");

    write_to_file(".globl _start\n");
}

void codegen_write_postamble(FILE *file)
{
    trace("Writing postamble");

    gen_rodata(file);
}

bool codegen_generate(module *mod, FILE *file)
{
    codegen_init();
    codegen_write_preamble(file);

    // Text section
    write_to_file(".text\n\n");

    int reg;
    reg = codegen_traverse_ast(file, mod->ast_root, 0);

    // get the unique key for main function
    char* main_function_key = symbol_generate_key("main", mod);

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
    write_to_file("\tcall\t%s\n", main_function_key);

    // Exit cleanly
#ifdef TIN_DEBUG_VERBOSE
    write_to_file("\t# Exit program cleanly\n");
#endif
    // exit syscall
    write_to_file("\tli\ta0, 0\n");  // Exit code
    write_to_file("\tli\ta7, 93\n"); // Syscall number
    write_to_file("\tecall\n");

    codegen_write_postamble(file);

    // cleanup
    free(main_function_key);
    free(codegen_alloc_registers()); // if regs are already allocated, free them
    rodata_free();
    variable_freeall();
}