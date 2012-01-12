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
	execute_check("run",  3 )
	execute_check("s", 7, x = 3)
	execute_check("s", 8, x = 4, y = 4)
	execute_check("s", 10, x = 5, y = 5)
	execute_check("s", 11, x = 4, y = 5, z = 4)
	execute_check("s", 14, x = 3, y = 5, z = 3)
	execute_check("s", 15, x = 3, y = 5, z = 3, b = -4)
	gdb.execute('quit')
except MismatchException, e:
	gdb.execute('quit 1') # quit with error code

