#!/usr/bin/python
import sys
sys.path.insert(0, "../../")
sys.path.insert(0, "./script/")

import lib 
from lib.baselib import execute_check, MismatchException

#
# Run and check
#
gdb.Breakpoint("test.t:test1")

try:
	execute_check("run",  3 )
	execute_check("n", 4, x = 32)
	execute_check("n", 5, x = 32, y = 42)
	execute_check("n", 6, x = 32, y = 42, a = 10)
	execute_check("n", 7, x = 32, y = 42, a = 10, b = 9)
	execute_check("n", 9, x = 32, y = 42, a = 10, b = 9, c = 8)
	execute_check("n", 17, x = 32, y = 42, a = 10, b = 9, c = 8)
	execute_check("n", 18, x = 32, y = 42, a = 32, b = 9, c = 8)
	execute_check("n", 22, x = 32, y = 42, a = 30, b = 9, c = 8)
	execute_check("n", 23, x = 32, y = 42, a = 30, b = 9, c = 8)
	execute_check("n", 29, x = 32, y = 42, a = 42, b = 9, c = 8)
	execute_check("n", 34, x = 32, y = 42, a = 42, b = 9, c = 8)
	execute_check("n", 36, x = 32, y = 42, a = 42, b = 9, c = 8)
	execute_check("n", 37, x = 32, y = 3, a = 42, b = 9, c = 8)
	execute_check("n", 43, x = 32, y = 3, a = 42, b = 9, c = 8)
	gdb.execute('quit')
except MismatchException, e:
	gdb.execute('quit 1') # quit with error code

