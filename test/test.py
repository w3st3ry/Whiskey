#!/usr/bin/env python3

import os
import subprocess
import sys

tests = '''
class
str
'''

print('Running tests...')
for test in tests.split():
    print(test + ':')
    dir_path = os.path.dirname(os.path.realpath(__file__))
    test_path = os.path.join(dir_path, test)
    if subprocess.call([test_path])  != 0:
        print('fail')
    print()
