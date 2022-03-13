# Filename: tohex.s
# Author: Alex
# 
# Copyright (c) 2022 Alex Guthrie
# License: MIT License
#
# https://jupitersim.gitbook.io/jupiter/assembler/ecalls

.globl __start

.rodata
	prompt: .string "Input: "
	output: .string "Hex: "
.text

__start:	
	# Print prompt text
	li a0, 4
	la a1, prompt
	ecall

	# Input integer into register t3
	li a0, 5
	ecall
	mv t3, a0

	# Print output text
	li a0, 4
	la a1, output
	ecall

	# Print hex
	li a0, 22
	mv a1, t3
	ecall
	
	# End program nicely
	li a0, 10
	ecall
