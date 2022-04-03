mkdir generated 2>NUL
mkdir build 2>NUL
flex --header-file=generated/lex.yy.h --outfile=generated/lex.yy.c src/tin.l
bison -d -b generated/parser src/tin.y
gcc -D TIN_INTERPRETER -Isrc -Isrc/utils -Igenerated -Werror -g -O0 src/*.c src/utils/*.c generated/lex.yy.c generated/parser.tab.c -o build/tin.exe