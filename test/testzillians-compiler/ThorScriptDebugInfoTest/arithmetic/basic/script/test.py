#!/usr/bin/python
import sys
sys.path.insert(0, "../../")
sys.path.insert(0, "./script/")

import lib 
from lib.baselib import execute_check, MismatchException

#
# Run and check
#
gdb.Breakpoint("test.t:test_function")

try:
	execute_check("run", 3)
	execute_check("n", 4, x = 32)
	execute_check("n", 7, x = 32, y = 42)
	execute_check("n", 8, x = 10, y = 42)
	execute_check("n", 11, x = 420, y = 42)
	execute_check("n", 12)
	gdb.execute('quit')
except MismatchException, e:
	gdb.execute('quit 1') # quit with error code

