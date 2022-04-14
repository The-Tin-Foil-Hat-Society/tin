#pragma once

#include <stdio.h>
#include "utils/hashtable.h"

#include "backend/common.h"

//
// Registers
//
// https://en.wikichip.org/wiki/risc-v/registers
#define REGISTER_COUNT 7
static char *registers[REGISTER_COUNT] = {"t0", "t1", "t2", "t3", "t4", "t5", "t6"};
static int free_registers[REGISTER_COUNT];

void register_freeall();
int register_alloc();
void free_register(int reg);

//
// Variables
//
hashtable *variables;

void variable_init();
void variable_freeall();
void variable_free(char *identifier);
int variable_get(char *identifier);
int variable_set(char *identifier, int value, int size);

static int current_variable_offset = 20;

//
// Read-only data
//
typedef struct rodata_entry rodata_entry;
struct rodata_entry
{
    char *identifier;
    char *string;
};

#define RODATA_COUNT 1024
static struct rodata_entry *rodata[RODATA_COUNT];
static int rodata_count = 0;

void rodata_init();
void rodata_add(char *identifier, char *string);
void rodata_free();
void gen_rodata(FILE *file);

//
// Generators
//
int gen_load(FILE *file, int value);

int gen_asm(FILE *file, char *string, int reg);

int gen_load_global(FILE *file, char *identifier, int size);
int gen_store_global(FILE *file, int reg, char *identifier, int size);
void gen_global_symbol(FILE *file, char *identifier);

int gen_rodata_string(FILE *file, char *identifier, char *string);
int gen_print_string(FILE *file, int index, int reg);

int gen_add(FILE *file, int left, int right);
int gen_sub(FILE *file, int left, int right);
int gen_mul(FILE *file, int left, int right);
int gen_div(FILE *file, int left, int right);
int gen_mod(FILE *file, int left, int right);

void gen_printint(FILE *file, int r);

int gen_function(FILE *file, int reg, char *identifier);
int gen_function_epilogue(FILE *file, int reg);
int gen_function_call(FILE *file, int reg, char *identifier);
int gen_return(FILE *file, int reg, bool is_at_end);
int gen_comparison_jump(FILE *file, int operation, int reg1, int reg2, int label);

void gen_jump(FILE *file, int label);
void gen_label(FILE *file, int label);

int label_add();