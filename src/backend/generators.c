#include "backend/generators.h"

int gen_store_global(FILE *file, int reg, char *identifier)
{
    // Do we already have an offset for this variable
    int offset = variable_get(identifier);

    // If we don't have an offset, add it
    if (offset == -1)
    {
        offset = variable_set(identifier, current_variable_offset);
    }

    emit(
        "Store global variable into memory",
        "sw",
        "%s, -%d(s0)",
        registers[reg],
        offset);
    free_register(reg);

    return reg;
}

int gen_load_global(FILE *file, char *identifier)
{
    int reg = register_alloc();
    int offset = variable_get(identifier);

    emit(
        "Load global variable from memory",
        "lw",
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
    emit(
        "Print string",
        "li",
        "a0, %d",
        PrintString);

    // Move offset into register a1
    emit(
        "Syscall args: print string",
        "la",
        "a1, Lstr%d",
        index);

    emit(
        "syscall",
        "ecall",
        "");

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

    emit("Load integer literal", "li", "%s, %d", registers[r], value);
    return r;
}

int gen_add(FILE *file, int left, int right)
{
    emit("Add integers", "add", "%s, %s, %s", registers[left], registers[left], registers[right]);
    free_register(right);

    return left;
}

int gen_sub(FILE *file, int left, int right)
{
    emit("Subtract integers", "sub", "%s, %s, %s", registers[left], registers[left], registers[right]);
    free_register(right);

    return left;
}

int gen_mul(FILE *file, int left, int right)
{
    emit("Multiply integers", "mul", "%s, %s, %s", registers[left], registers[left], registers[right]);
    free_register(right);

    return left;
}

int gen_div(FILE *file, int left, int right)
{
    emit("Divide integers", "div", "%s, %s, %s", registers[left], registers[left], registers[right]);
    free_register(right);

    return left;
}

void gen_printint(FILE *file, int r)
{
    emit("Syscall ID", "li", "a0, %d", PrintInt);
    emit("Parameter", "mv", "a1 %s", registers[r]);
    emit("Syscall", "ecall", "");

    free_register(r);
}

void variable_init()
{
    variables = hashtable_new();
}

void variable_free(char *identifier)
{
    trace("\tFreeing variable %s\n", identifier);
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

int variable_set(char *identifier, int value)
{
    trace("\tSetting variable %s to %d\n", identifier, value);
    int *item = (int *)malloc(sizeof(int));
    *item = value;
    hashtable_set_item(variables, identifier, item);

    current_variable_offset += 4;
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

void gen_rodata(FILE *file)
{
    write_to_file(".section\t.rodata\n");
    for (int i = 0; i < rodata_count; i++)
    {
        trace("\tWriting rodata string %s ('%s')", rodata[i]->identifier, rodata[i]->string);
        write_to_file("%s:\n", rodata[i]->identifier);
        write_to_file("\t.string\t\"%s\"\n", rodata[i]->string);
    }
}

int gen_function(FILE *file, int reg, char *identifier)
{
    write_to_file("%s:\t# Declare function %s\n", identifier, identifier);

    emit_comment("Function prologue\n");
    emit("Move the stack point 16 bytes back", "addi", "sp, sp, -16");
    emit("Store the return address", "sw", "ra, 12(sp)");
    emit("Store the s0 register", "sw", "s0, 8(sp)");
    emit("Move the stack point back", "addi", "sp, sp, 16");
    emit_comment("End function prologue\n");

    return reg;
}

int gen_function_epilogue(FILE *file, int reg)
{
    emit_comment("Function epilogue\n");
    emit("Load the return address", "lw", "ra, 12(sp)");
    emit("Load the s0 register", "lw", "s0, 8(sp)");
    emit("Move the stack point 16 bytes back", "addi", "sp, sp, 16");
    emit("Return", "jr", "ra");
    emit_comment("End function epilogue\n");

    return reg;
}

int gen_function_call(FILE *file, int reg, char *identifier)
{
    emit("Call function", "call", "%s", identifier);
    return reg;
}

int gen_return(FILE *file, int reg)
{
    emit("Return", "ret", "");
    return reg;
}