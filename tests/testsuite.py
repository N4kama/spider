#!/usr/bin/python3

import os
from subprocess import Popen, PIPE

def test_file(folder, file):
    print("Testing " + file + ": ", end='')

    bash_res = Popen(["./" + folder + file],
    stdin=PIPE, stdout=PIPE, stderr=PIPE)
    bash_out, bash_err = bash_res.communicate()
    ret_val = bash_res.returncode

    if not ret_val:
        print("OK")
    else:
        print("KO")

#Take a string 'folder' as parameter
def run_unitaryTests(folder):
    print("Running Unitary Tests:")
    for file in os.listdir(folder):
        if "." not in file:
            test_file(folder, file)

def run_globalTests():
    print("Running Tests:")
    print("Oops, nothing seems to be testing spider in it's globality")


"""
MAIN :
    -> Execute unitary tests
    -> Execute global tests
"""

print("\tStarting TestSuite:\n")

run_unitaryTests("unitary_tests/")
print()
run_globalTests()

print("\n\tTestSuite stopped.")