# tin Programming Language and RISC-V Compiler

This is a team project for University which involves us designing a simple programming language, and building a compiler to RISC-V for it.

## Requirements
- bison >= 3.5.1
- flex  >= 2.6.4
- gcc

## TODO
- optimisation, i.e. precompute an expression of constants to a single constant and etc, check what optimisation techniques other compilers use
- check for memory leaks in valgrind with `--leak-check=full`
- implement different sized data types, currently we are treating all integer types the same
- allow for programs with multiple files/modules
- write new example code and make sure it's parses
- check for other TODO's in the code ( CTRL+SHIFT+F )
- make MAKE files