# Imports
import subprocess
import os
from os import listdir
from os.path import isfile, join
from genericpath import exists

tests_path = "./unit-tests/"
expecteds_path = "./expected-outputs/"

test_list = [f for f in listdir(tests_path) if isfile(join(tests_path, f))]
test_list.sort()
expected_list = [f for f in listdir(expecteds_path) if isfile(join(expecteds_path, f))]
expected_list.sort()

tests_passed = 0
test_amount = len(test_list)

failed_test_list = []
failed_test_outputs = []
failed_test_expected_outputs = []

if len(test_list) != len(expected_list):
    print("Test amount mismatch")
    exit(1)

print(test_list)
print(expected_list)

for x in range(test_amount):
    # Set current test
    current_test = tests_path + test_list[x]

    # Strip the extension off the file name
    test_name = current_test.rsplit('.', 1)[0]

    # Run the test
    test_build = subprocess.run(["../build/tin", current_test], capture_output=True) # Compile the test file
    test_run = subprocess.run(["qemu-riscv64", test_name], capture_output=True, text=True, encoding="ascii") # Run the compiled test file
    test_output = test_run.stdout.encode("ascii") # Store output of test file

    expected_open = open(expecteds_path + expected_list[x], "r") # Open the expected output file
    expected_output = expected_open.read().encode("ascii") # Store the expected output
    
    if exists(test_name): # If the test file exists
        os.remove(test_name) # Cleanup the compiled file

    # Check if the test passed
    if test_output == expected_output:
        tests_passed += 1 # Increment tests passed
    else:
        failed_test_list.append(current_test) # Add the failed test to the failed list
        failed_test_outputs.append(test_output) # Add the output of the failed test to the failed list
        failed_test_expected_outputs.append(expected_output) # Add the expected output of the failed test to the failed list

    
print("Tests passed: " + str(tests_passed) + "/" + str(test_amount))
if tests_passed != test_amount:
    print("Failed tests:")
    for x in range(len(failed_test_list)):
        print("\t" + failed_test_list[x])
        print("\t\t" + "Expected: " + str(failed_test_expected_outputs[x].decode("ascii")))
        print("\t\t" + "Got: " + str(failed_test_outputs[x].decode("ascii")))

