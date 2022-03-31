#include "backend/generators.h"

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
            printf("Allocated register %s\n", registers[i]);
            free_registers[i] = 0;
            return (i);
        }
    }

    compiler_error("No free registers");
}

void free_register(int reg)
{
    if (free_registers[reg] != 0)
    {
        compiler_error("Error trying to free register %d\n", reg);
    }
    free_registers[reg] = 1;
}

int gen_load(FILE *file, int value)
{
    int r = register_alloc();

    emit("\tli\t%s, %d\n", registers[r], value);
    return r;
}

int gen_add(FILE *file, int left, int right)
{
    emit("\tadd\t%s, %s, %s\n", registers[left], registers[left], registers[right]);
    free_register(right);

    return left;
}

int gen_sub(FILE *file, int left, int right)
{
    emit("\tsub\t%s, %s, %s\n", registers[left], registers[left], registers[right]);
    free_register(right);

    return left;
}

int gen_mul(FILE *file, int left, int right)
{
    emit("\tmul\t%s, %s, %s\n", registers[left], registers[left], registers[right]);
    free_register(right);

    return left;
}

int gen_div(FILE *file, int left, int right)
{
    emit("\tdiv\t%s, %s, %s\n", registers[left], registers[left], registers[right]);
    free_register(right);

    return left;
}

void gen_printint(FILE *file, int r)
{
    emit("\tli\ta0, 1\n");
    emit("\tmv\ta1, %s\n", registers[r]);
    emit("\tecall\n");

    free_register(r);
}