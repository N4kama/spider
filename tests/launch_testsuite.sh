#!/bin/bash

virtualenv venv
source venv/bin/activate
pip install -r ./tests/requirements.txt
clear

python3 ./tests/testsuite.py

rm -rf venv
cd tests
deactivate
cd -