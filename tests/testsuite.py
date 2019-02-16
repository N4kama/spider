#!/usr/bin/python3

import os
from subprocess import Popen, PIPE

def test_file(file):
    print("Testing " + file + ": ", end='')

    bash_res = Popen(["./unitary_tests/" + file],
    stdin=PIPE, stdout=PIPE, stderr=PIPE)
    bash_out, bash_err = bash_res.communicate()
    ret_val = bash_res.returncode

    if not ret_val:
        print("OK")
    else:
        print("KO")

def run_unitaryTests():
    print("Running Unitary Tests:")
    for file in os.listdir("unitary_tests"):
        if "." not in file:
            test_file(file)

def run_globalTests():
    print("Running Tests:")
    print("Oops, nothing seems to be testing spider in it's globality")


"""
MAIN :
    -> Execute unitary tests
    -> Execute global tests
"""

print("\tStarting TestSuite:\n")

run_unitaryTests()
print()
run_globalTests()

print("\n\tTestSuite stopped.")