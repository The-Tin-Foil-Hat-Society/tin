#include "backend/generators.h"

int gen_store_global(FILE *file, int reg, char *identifier, int size)
{
    emit_comment("Store global %s into register %s (size: %d)\n", identifier, registers[reg], size);

    // Do we already have an offset for this variable
    int offset = variable_get(identifier);

    // If we don't have an offset, add it
    if (offset == -1)
    {
        // Align current_variable_offset to size
        if (current_variable_offset % size != 0)
        {
            current_variable_offset += size - (current_variable_offset % size);
            trace("\tAligned current_variable_offset to %d\n", current_variable_offset);
        }

        offset = variable_set(identifier, current_variable_offset, size);
    }

    char *instruction;
    switch (size)
    {
    case 8:
        instruction = "sd"; // Store double
        break;
    case 4:
        instruction = "sw"; // Store word
        break;
    case 2:
        instruction = "sh"; // Store half
        break;
    case 1:
        instruction = "sb"; // Store byte
        break;
    default:
        compiler_error("Unknown size %d\n", size);
        break;
    }

    trace("\tSize %d has instruction %s", size, instruction);

    emit(
        "Store global variable into memory",
        instruction,
        "%s, -%d(s0)",
        registers[reg],
        offset);
    free_register(reg);

    return reg;
}

int gen_load_global(FILE *file, char *identifier, int size)
{
    int reg = register_alloc();
    int offset = variable_get(identifier);

    emit_comment("Load global %s from offset %d into register %s (size: %d)\n", identifier, offset, registers[reg], size);

    char *instruction;
    switch (size)
    {
    case 8:
        instruction = "ld"; // Load double
        break;
    case 4:
        instruction = "lw"; // Load word
        break;
    case 2:
        instruction = "lh"; // Load half
        break;
    case 1:
        instruction = "lb"; // Load byte
        break;
    default:
        compiler_error("Unknown size %d\n", size);
        break;
    }

    trace("\tSize %d has instruction %s", size, instruction);

    emit(
        "Load global variable from memory",
        instruction,
        "%s, -%d(s0)",
        registers[reg],
        offset);

    return reg;
}

int gen_rodata_string(FILE *file, char *identifier, char *string)
{
    rodata_add(identifier, string);
}

int gen_print_string(FILE *file, int index, int reg)
{
    emit_comment("Print string constant %d\n", index);

    // Sys_Write parameters:
    // a0 - file descriptor (stdout is 1)
    // a1 - address of string
    // a2 - length of string

    emit(
        "Syscall args: file descriptor (stdout = 1)",
        "li",
        "a0, 1");

    emit(
        "Syscall args: string address",
        "la",
        "a1, Lstr%d",
        index);

    rodata_entry *string_entry = rodata[index];

    // Get string length with carriage returns stripped
    int len = 0;
    for (int i = 0; i < strlen(string_entry->string); i++)
    {
        if (string_entry->string[i] == '\r')
            continue;
        len += 1;
    }

    emit(
        "Syscall args: string length",
        "li",
        "a2, %d",
        len);

    emit(
        "Syscall type: write",
        "li",
        "a7, 64");

    emit(
        "Syscall type",
        "li",
        "a7, %d",
        Sys_Write);

    emit("Syscall", "ecall", "");

    return reg;
}

void register_freeall()
{
    for (int i = 0; i < REGISTER_COUNT; i++)
    {
        free_registers[i] = 1;
    }
}

int register_alloc()
{
    for (int i = 0; i < REGISTER_COUNT; i++)
    {
        if (free_registers[i])
        {
            trace("\tAllocated register %s", registers[i]);
            free_registers[i] = 0;
            return (i);
        }
    }

    compiler_error("No free registers");
}

void free_register(int reg)
{
    trace("\tFreeing register %s\n", registers[reg]);
    if (free_registers[reg] != 0)
    {
        compiler_error("Error trying to free register %d\n", reg);
    }
    free_registers[reg] = 1;
}

int gen_load(FILE *file, int value)
{
    int r = register_alloc();

    emit_comment("Load constant %d into register %s\n", value, registers[r]);

    emit("Load integer literal", "li", "%s, %d", registers[r], value);
    return r;
}

int gen_add(FILE *file, int left, int right)
{
    emit_comment("Addition of %s and %s\n", registers[left], registers[right]);

    emit("Add integers", "add", "%s, %s, %s", registers[left], registers[left], registers[right]);
    free_register(right);

    return left;
}

int gen_sub(FILE *file, int left, int right)
{
    emit_comment("Subtraction of %s and %s\n", registers[left], registers[right]);

    emit("Subtract integers", "sub", "%s, %s, %s", registers[left], registers[left], registers[right]);
    free_register(right);

    return left;
}

int gen_mul(FILE *file, int left, int right)
{
    emit_comment("Multiplication of %s and %s\n", registers[left], registers[right]);

    emit("Multiply integers", "mul", "%s, %s, %s", registers[left], registers[left], registers[right]);
    free_register(right);

    return left;
}

int gen_div(FILE *file, int left, int right)
{
    emit_comment("Division of %s and %s\n", registers[left], registers[right]);

    emit("Divide integers", "div", "%s, %s, %s", registers[left], registers[left], registers[right]);
    free_register(right);

    return left;
}

int gen_mod(FILE *file, int left, int right)
{
    emit_comment("Remainder of %s and %s\n", registers[left], registers[right]);

    emit("Find remainder", "rem", "%s, %s, %s", registers[left], registers[left], registers[right]);
    free_register(right);

    return left;
}

void variable_init()
{
    variables = hashtable_new();
}

void variable_freeall()
{
    for (int i = 0; i < variables->capacity; i++)
    {
        if (variables->keys[i] != NULL)
        {
            free(variables->items[i]);
        }
    }
    hashtable_free(variables);
}

void variable_free(char *identifier)
{
    trace("\tFreeing variable %s\n", identifier);
    free(hashtable_get_item(variables, identifier));
    hashtable_delete_item(variables, identifier);
}

int variable_get(char *identifier)
{
    int *value = (int *)hashtable_get_item(variables, identifier);
    if (value == NULL)
    {
        trace("\tVariable %s not found", identifier);
        return -1;
    }
    trace("\tVariable %s found at offset %d", identifier, *value);
    return *value;
}

int variable_set(char *identifier, int value, int size)
{
    trace("\tSetting variable %s to %d\n", identifier, value);
    int *item = (int *)malloc(sizeof(int));
    *item = value;
    hashtable_set_item(variables, identifier, item);

    current_variable_offset += size;
    return value;
}

void rodata_init()
{
}

void rodata_add(char *identifier, char *string)
{
    rodata_entry *entry = (rodata_entry *)malloc(sizeof(rodata_entry));
    entry->identifier = strdup(identifier);
    entry->string = strdup(string);

    trace("\tAdding string '%s' to rodata under identifier '%s'", entry->identifier, entry->string);

    rodata[rodata_count] = entry;
    rodata_count++;
}
void rodata_free()
{
    for (int i = 0; i < rodata_count; i++)
    {
        rodata_entry *entry = (rodata_entry *)rodata[i];
        if (entry == NULL)
        {
            continue;
        }
        free(entry->identifier);
        free(entry->string);
        free(entry);
    }
}

void gen_rodata(FILE *file)
{
    write_to_file(".section\t.rodata\n");
    for (int i = 0; i < rodata_count; i++)
    {
        trace("\tWriting rodata string %s ('%s')", rodata[i]->identifier, rodata[i]->string);
        write_to_file("%s:\n", rodata[i]->identifier);
        write_to_file("\t.ascii\t\"%s\\0\"\n", rodata[i]->string);
    }
}

int gen_function(FILE *file, int reg, char *identifier)
{
    write_to_file("%s:\n", identifier);

    emit_comment("Function prologue\n");
    emit("Move the stack point back", "addi", "sp, sp, -64");
    emit("Store the return address", "sd", "ra, 56(sp)");
    emit("Store the s0 register", "sd", "s0, 48(sp)");
    emit("Move the stack point back", "addi", "s0, sp, 64");
    emit_comment("End function prologue\n");

    return reg;
}

int gen_function_epilogue(FILE *file, int reg)
{
    emit_comment("Function epilogue\n");
    emit("Load the return address", "ld", "ra, 56(sp)");
    emit("Load the s0 register", "ld", "s0, 48(sp)");
    emit("Move the stack point back forward", "addi", "sp, sp, 64");
    emit("Return", "jr", "ra");
    emit_comment("End function epilogue\n");

    return reg;
}

int gen_function_call(FILE *file, int reg, char *identifier)
{
    emit_comment("Function call %s\n", identifier);

    emit("Call function", "call", "%s", identifier);
    return reg;
}

int gen_return(FILE *file, int reg, bool is_at_end)
{
    emit_comment("Return\n");

    if (is_at_end)
    {
        emit_comment("Omitted epilogue - return is at end of function\n");
    }
    else
    {
        gen_function_epilogue(file, reg);
    }

    return reg;
}

int gen_asm(FILE *file, char *string, int reg)
{
    emit_comment("Assembler directive\n");
    write_to_file("\t%s\n", string);

    return reg;
}

void gen_label(FILE *file, int label)
{
    write_to_file(".LB%d:\n", label);
}

void gen_jump(FILE *file, int label)
{
    emit("Jump to label", "jal", ".LB%d", label);
}

int gen_comparison_jump(FILE *file, int operation, int reg1, int reg2, int label)
{
    trace("\tComparing %d and %d (operation: %s)\n", reg1, reg2, ast_type_names[operation]);
    /*
     * We actually do the opposite of the operation here, because we want to jump
     * to a 'false' label if the condition is false.
     */
    switch (operation)
    {
    case AstEqual:
        emit("Equal", "bne", "%s, %s, .LB%d", registers[reg1], registers[reg2], label);
        break;
    case AstNotEqual:
        emit("Not equal", "beq", "%s, %s, .LB%d", registers[reg1], registers[reg2], label);
        break;

    case AstLessThan:
        emit("Less than", "bge", "%s, %s, .LB%d", registers[reg1], registers[reg2], label);
        break;
    case AstGreaterThanOrEqual:
        emit("Greater than or equal", "blt", "%s, %s, .LB%d", registers[reg1], registers[reg2], label);
        break;

    case AstLessThanOrEqual:
        emit("Less than or equal", "blt", "%s, %s, .LB%d", registers[reg2], registers[reg1], label);
        break;
    case AstGreaterThan:
        emit("Greater than", "bge", "%s, %s, .LB%d", registers[reg2], registers[reg1], label);
        break;

    default:
        compiler_error("Unsupported comparison operation %s", ast_type_names[operation]);
        break;
    }
}

int label_add()
{
    static int current_label = 0;
    return current_label++;
}