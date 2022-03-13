# Filename: arrayindex.s
# Author: Andrejs
# 
# Copyright (c) 2022 Andrejs Krauze
# License: MIT License
#

.globl __start

.rodata
	array_0:	.word	81
	array_1:	.word	217
	array_2:	.word	-675
	array_3:	.word	481
	array_4:	.word	11
	array_5:	.word	-78
	array_6:	.word	1345
	array_7:	.word	-5165
	array_length: .word	8

	prompt: .string "Get number at index "
	output: .string "The number at the given index is "
	error:	.string "Index is out of bounds!\n"
.text

print_array:
	addi s0, a0, 0 # save the array's address
	addi s1, a1, 0 # save the array's length
	li s2, 0 # initialize the current index

	# print opening bracket
	li a0, 11 # print char ecall
	li a1, '['
	ecall # https://jupitersim.gitbook.io/jupiter/assembler/ecalls

print_array_loop:
	# print the integer at the array's adderss
	li a0, 1 # print int ecall
	lw a1, 0(s0)
	ecall
	
	addi s0, s0, 4 # increment the array address by 4 (size of an int/word)
	addi s2, s2, 1 # increment the current index
	
	# if current index is greater than or equals to the array's length go to the end of the subroutine
	bge s2, s1, print_array_end 
	
	# otherwise print a comma and loop
	li a0, 11
	li a1, ','
	ecall
	
	j print_array_loop
	
print_array_end:

	# print closing bracket and a newline
	li a0, 11
	li a1, ']'
	ecall
	li a0, 11
	li a1, '\n'
	ecall
	
	ret
	
	
get_index:
	addi s0, a0, 0 # save the array's address
	addi s1, a1, 0 # save the array's length
	
get_index_loop:
	# print the prompt
	li a0, 4 # print string ecall
	la a1, prompt
	ecall
	
	# get the user's int 
	li a0, 5 # read int ecall
	ecall
	mv t0, a0 # and save it in a temp register
	
	li t1, 0 # temp 0 for the comparison
	blt t0, t1, get_index_error # if the given index is less than 0
	bge t0, s1, get_index_error # or is greater than the array's length, warn the user
	
	li t1, 4 # for the mul
	mul t0, t0, t1 # multiply the index by 4 to get the offset of the element
	add t0, s0, t0 # add the offset and the array's address
	
	# print descriptive text before the output
	li a0, 4
	la a1, output
	ecall
	
	# print the number at the address
	li a0, 1
	lw a1, 0(t0)
	ecall
	
	ret
	
get_index_error:
	# print the error message and ask the user again
	li a0, 4 
	la a1, error
	ecall
	
	j get_index_loop


__start:
	la a0, array_0 # pass the address of the beginning of the array as the first argument
	lw a1, array_length # pass the array length as the second argument
	call print_array # call our custom subroutine to display the array
	
	la a0, array_0
	lw a1, array_length
	call get_index
	
	# exit the program
	li a0, 10
	ecall