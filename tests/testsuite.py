import os
import signal
from subprocess import Popen, PIPE
import requests

def test_statuscode(url, expect):
    r = requests.get(url)
    print("GET REQUEST on : {}\nStatus code  : {} ({} was expected)\n".format(
        url, r.status_code, expect))

# MAIN

print("Starting testsuite :\n")

p = Popen(["./../spider", "configs/test1.txt"],
            stdout=PIPE, stderr=PIPE)

try:
    test_statuscode("http://localhost:8000/configs/hello.html", 200)
    test_statuscode("http://localhost:8000/configs/perm_error.html", 403)
except requests.exceptions.RequestException as e:
    print(e)
    exit(1)



p.kill()

print("Testsuite ended")
