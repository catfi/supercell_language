#!/usr/bin/python
import sys
sys.path.insert(0, "../../")
sys.path.insert(0, "./script/")

import lib 
from lib.baselib import execute_check, MismatchException

#
# Run and check
#
gdb.Breakpoint("test.t:test2")

try:
	execute_check("run", 16)
	execute_check("n", 17, i = 0)
	execute_check("n", 18, i = 0, j = 0)
	execute_check("n", 21, i = 0, j = 0, x = 0)
	execute_check("n", 22)
	execute_check("n", 26)
	execute_check("n", 27)
	execute_check("n", 30)
	execute_check("n", 31, x = 1)
	execute_check("n", 28)
	execute_check("n", 27, j = 1)
	execute_check("n", 30)
	execute_check("n", 31, x = 2)
	execute_check("n", 28)
	execute_check("n", 27, j = 2)
	execute_check("n", 32)
	execute_check("n", 23)
	execute_check("n", 22, i = 1)
	execute_check("n", 26)
	execute_check("n", 27, j = 0)
	execute_check("n", 30)
	execute_check("n", 31, x = 3)
	execute_check("n", 28)
	execute_check("n", 27, j = 1)
	execute_check("n", 30)
	execute_check("n", 31, x = 4)
	execute_check("n", 28)
	execute_check("n", 27, j = 2)
	execute_check("n", 32)
	execute_check("n", 23)
	execute_check("n", 22, i = 2)
	execute_check("n", 34, i = 2, j = 2, x = 4)
	execute_check("n", 35)

	gdb.execute('quit')
except MismatchException, e:
	gdb.execute('quit 1') # quit with error code

