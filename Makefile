#This is the make file for the tin compiler

TIN_COPYRIGHT = \"Copyright (c) 2022 Kallum Doughty, Alexander J Guthrie, Andrejs Krauze, Joshua S Lotriet, Hardijs Raubiskis\"
GIT_VERSION = $(shell git describe --tags)@$(shell git rev-parse --abbrev-ref HEAD)
GIT_ORIGIN = $(shell git config --get remote.origin.url)
BUILD_TIME = $(shell date -u --iso=seconds)

#File to compile:
file = examples/for.tin
# or debug 
build = release 
FLAGS = -D TIN_COMPILER -DTIN_COPYRIGHT="$(TIN_COPYRIGHT)" -DBUILD_TIME="\"$(BUILD_TIME)\"" -DGIT_VERSION="\"$(GIT_VERSION)\"" -DGIT_ORIGIN="\"$(GIT_ORIGIN)\""
BUILD_PATH = build/tin
INCLUDES = -Isrc -Igenerated

CC = gcc
ifeq ($(CC),afl-gcc-fast)
	FLAGS += -D TIN_FUZZ
	BUILD_PATH = fuzzing/tin
	INCLUDES += -Ifuzzing
endif
ifeq ($(build),debug)
	CCFLAGS = -g3 -Og 
	FLAGS += -D TIN_DEBUG -D TIN_DEBUG_VERBOSE
else
	CCFLAGS = -g0 -O3 -s	
	FLAGS += -D TIN_RELEASE
endif

preproc: FLAGS += -D TIN_INTERPRETER

SOURCES = src/*.c src/backend/*.c src/utils/*.c
SOURCES_GENERATED = generated/lex.yy.c generated/parser.tab.c

tin: dir parser.o lex.o
	@$(CC) $(FLAGS) $(INCLUDES) -Wall -Wextra -pedantic -Werror $(CCFLAGS) $(SOURCES) $(SOURCES_GENERATED) -o $(BUILD_PATH) -lm

debug_assembly:
	@riscv64-linux-gnu-as ./examples/itoa.s -o ./examples/itoa.o
	@riscv64-linux-gnu-ld -o ./examples/itoa.out ./examples/itoa.o
	@qemu-riscv64 ./examples/itoa.out

#Builds and runs tin with just the preprocessor
preproc: tin
	@./build/tin $(file)

run: tin
	@./build/tin $(file)
	@echo "\nRunning result file"
	@qemu-riscv64 $(basename $(file))

#Generates directories to store generated files and build
dir:
	@mkdir -p generated
	@mkdir -p build

#Generates parserfiles
parser.o: src/tin.y
	@bison -d -b generated/parser src/tin.y

#Generates lexer files
lex.o: src/tin.l
	@flex --header-file=generated/lex.yy.h --outfile=generated/lex.yy.c src/tin.l

#Removes all generated and built files including generated direcories
clean:
	-@rm -f generated/* build/*
	-@rm -rf generated build
	-@rm -f examples/*.s
	-@rm -f examples/*.o
	-@rm -f examples/*.out
	-@rm -f examples/*.mod.json
	-@rm -f units/*.s
	-@rm -f units/*.o
	-@rm -f units/*.out
	-@rm -f units/*.mod.json
#Delete files without extensions
	-@find units/ -not -iname "*.*" -type f -exec rm '{}' \;
	-@find examples/ -not -iname "*.*" -type f -exec rm '{}' \; 

memcheck: tin
	@valgrind ./build/tin $(file)

memcheck_full: tin
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./build/tin $(file)
