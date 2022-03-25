#This is the make file for the tin interpreter
tin: dir parser.o lex.o
	@ gcc -D TIN_COMPILER -D TIN_DEBUG_OUTPUT_AST -Isrc -Igenerated -Werror -g -O0 src/*.c src/backend/*.c generated/lex.yy.c \
	generated/parser.tab.c -o build/tin

run: tin
	@./build/tin ./units/hello_world.tin
	@jupiter ./units/hello_world.tin.s

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
	@rm generated/* build/*
	@rmdir generated build

check_leaks: tin
	@valgrind ./build/tin ./units/hello_world.tin --leak-check=full