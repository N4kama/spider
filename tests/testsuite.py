import os
import signal
from subprocess import Popen, PIPE
import requests
import socket

def test_configs(path):
    p = Popen(["./spider", "-t", "./tests/configs/{}".format(path)],
            stdout=PIPE, stderr=PIPE)
    p.communicate()
    print("Testing {}: server return with value {} (expected {})".format(
        path.split('/')[-1], p.returncode, int(path[0] == 'b')))


def test_statuscode(url, expect, info):
    try:
        r = requests.get(url)
        print("GET REQUEST on : {} {}\nStatus code  : {} ({} was expected)".format(
        url, info, r.status_code, expect))
    except requests.exceptions.ConnectionError as e:
        print("GET REQUEST on : {} {}\nStatus code  : {} ({} was expected)".format(
        url, info, 404, expect))
    except requests.exceptions.RequestException as e:
        print(e)
        exit(1)

# MAIN

print("Starting testsuite :\n")

'''
        TESTING JSON CONFIG
'''
print("Testing server configurations\n")

jsons = ["conf1.json", "conf2.json", "conf3.json", "conf4.json", "conf5.json",
"buggyconf1.json", "buggyconf2.json", "buggyconf3.json", "buggyconf4.json",
"buggyconf5.json"]

for config in jsons:
    test_configs(config)
print()

'''
        TESTING STATUS CODES
'''
print("Testing status codes:\n")

p = Popen(["./spider", "./tests/configs/test1.txt"],
            stdout=PIPE, stderr=PIPE)

#From files
test_statuscode("http://localhost:8000/tests/configs/hello.html", 200, "(valid request)")
test_statuscode("http://localhost:8000/tests/configs/perm_error.html", 403, "(permission error : file cannot be read)")
test_statuscode("http://localhost8000/index.html", 404, "(file does not exists)")

#from manually forged requests
try:
    url = "http://localhost:8000/configs/hello.html"

    #Testing method not implemented (405)
    r = requests.put(url, data = {'fact':'naruto < dbz'})
    print("PUT REQUEST on : {} (PUT method not implemented)\nStatus code  : {} (405 was expected)".format(
            url, r.status_code))

    #Testing invalid http version (426)
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.connect(("localhost", 8000))
    sock.send(b"HEAD /tests/configs/hello.html HTTP/1.0\r\n\r\n")
    response = str(sock.recv(100), 'utf-8').split(' ')
    print("HEAD REQUEST on : {} (invalid HTTP version)\nStatus code  : {} (426 was expected)".format(
            url, response[1]))
    sock.close()

    #Testing Bad request (400)
except requests.exceptions.RequestException as e:
    exit(1)

print()

p.kill()

print("Testsuite ended")
