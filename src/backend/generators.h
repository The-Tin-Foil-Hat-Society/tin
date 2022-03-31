#pragma once

#include <stdio.h>

#include "backend/common.h"

// https://en.wikichip.org/wiki/risc-v/registers
#define REGISTER_COUNT 4
static char *registers[REGISTER_COUNT] = {"t0", "t1", "t2", "t3"};
static int free_registers[REGISTER_COUNT];

void register_freeall();
int register_alloc();
void free_register(int reg);

int gen_load(FILE *file, int value);
int gen_add(FILE *file, int left, int right);
int gen_sub(FILE *file, int left, int right);
int gen_mul(FILE *file, int left, int right);
int gen_div(FILE *file, int left, int right);

void gen_printint(FILE *file, int r);