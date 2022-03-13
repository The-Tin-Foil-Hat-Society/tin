# Filename: factorial.s
# Author: Alex
# 
# Copyright (c) 2022 Alex Guthrie
# License: MIT License
#

.globl __start

.rodata
	prompt: .string "Factorial of: "
	output: .string "Result: "
.text

__start:	
	# Print prompt text
	li a0, 4 
	la a1, prompt
	ecall # https://jupitersim.gitbook.io/jupiter/assembler/ecalls

	# Input integer into register t3
	li a0, 5
	ecall
	mv t3, a0
	
	# Print newline text
	li a0, 11
	li a1, '\n'
	ecall

	# Copy t3 into t2
	mv t2, t3

fac:
	addi t3, t3, -1 # Decrement t3
	beq t3, zero, end # If t3 is 0, we're done here
	mul t2, t2, t3 # Multiply t2 and t3
	j fac

end:
	# Print output text
	li a0, 4
	la a1, output
	ecall

	# Display result (in t2)
	li a0, 1
	mv a1, t2
	ecall

	# End program nicely
	li a0, 10
	ecall
