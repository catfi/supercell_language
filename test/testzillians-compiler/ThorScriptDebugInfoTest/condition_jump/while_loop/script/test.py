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
	execute_check("n", 4, i = 5)
	execute_check("n", 6, i = 5, x = 0)
	execute_check("n", 8, i = 5, x = 0)
	execute_check("n", 9, i = 5, x = 1)
	execute_check("n", 11, i = 5, x = 1)
	execute_check("n", 13, i = 4, x = 1)
	execute_check("n", 6, i = 4, x = 1)
	execute_check("n", 8, i = 4, x = 1)
	execute_check("n", 9, i = 4, x = 2)
	execute_check("n", 11, i = 4, x = 2)
	execute_check("n", 13, i = 3, x = 2)
	execute_check("n", 6, i = 3, x = 2)
	execute_check("n", 8, i = 3, x = 2)
	execute_check("n", 9, i = 3, x = 3)
	execute_check("n", 11, i = 3, x = 3)
	execute_check("n", 13, i = 2, x = 3)
	execute_check("n", 6, i = 2, x = 3)
	execute_check("n", 8, i = 2, x = 3)
	execute_check("n", 9, i = 2, x = 4)
	execute_check("n", 11, i = 2, x = 4)
	execute_check("n", 13, i = 1, x = 4)
	execute_check("n", 6, i = 1, x = 4)
	execute_check("n", 8, i = 1, x = 4)
	execute_check("n", 9, i = 1, x = 5)
	execute_check("n", 11, i = 1, x = 5)
	execute_check("n", 13, i = 0, x = 5)
	execute_check("n", 6, i = 0, x = 5)
	execute_check("n", 14, i = 0, x = 5)
	gdb.execute('quit')
except MismatchException, e:
	gdb.execute('quit 1') # quit with error code

