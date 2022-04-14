#This is the make file for the tin compiler

#File to compile:
file = examples/for.tin
flags = -D TIN_COMPILER -D TIN_DEBUG_VERBOSE

preproc: flags = -D TIN_DEBUG_VERBOSE

sources = src/*.c src/backend/*.c src/utils/*.c
sources_generated =generated/lex.yy.c generated/parser.tab.c

tin: dir parser.o lex.o
	@ gcc $(flags) -Isrc -Igenerated -Werror -fsanitize=address -g -O0 $(sources) $(sources_generated) -o build/tin -lm

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
	@qemu-riscv64 $(basename $(file)).out

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
	@rmdir generated build
	-@rm -f examples/*.s
	-@rm -f examples/*.o
	-@rm -f examples/*.out
	-@rm -f examples/*.mod.json

memcheck: tin
	@valgrind ./build/tin $(file)

memcheck_full: tin
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./build/tin $(file)
