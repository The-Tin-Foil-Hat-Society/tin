# Filename: echo.s
# Author: Alex
# 
# Copyright (c) 2022 Alex Guthrie
# License: MIT License
#
# https://jupitersim.gitbook.io/jupiter/assembler/ecalls

.globl __start

.data
    echo: .zero 256 # buffer with length 256

.rodata
	prompt: .string "> "
.text

__start:	
	# Print prompt text
	li a0, 4
	la a1, prompt
	ecall

	# Read string
	li a0, 8
    la a1, echo # Buffer address
    li a2, 256 # Buffer size
	ecall

	# Print string
	li a0, 4
	la a1, echo # String to print
	ecall
	
	# End program nicely
	li a0, 10
	ecall
