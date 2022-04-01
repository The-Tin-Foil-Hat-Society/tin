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

    fprintf(file, "\tsw\t%s, -%d(s0)\n", registers[reg], offset);
    free_register(reg);

    return reg;
}

int gen_load_global(FILE *file, char *identifier)
{
    int reg = register_alloc();
    int offset = variable_get(identifier);

    fprintf(file, "\tlw\t%s, -%d(s0)\n", registers[reg], offset);
    return reg;
}

void gen_global_symbol(FILE *file, char *identifier)
{
    fprintf(file, "\t.globl\t%s\n", identifier);
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

    emit("Load integer", "li", "%s, %d", registers[r], value);
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