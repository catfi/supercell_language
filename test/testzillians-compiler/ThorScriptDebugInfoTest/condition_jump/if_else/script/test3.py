#!/usr/bin/python
import sys
sys.path.insert(0, "../../")
sys.path.insert(0, "./script/")

import lib 
from lib.baselib import execute_check, MismatchException

#
# Run and check
#
gdb.Breakpoint("test.t:test3")

try:
	execute_check("run",  65 )
	execute_check("n", 66, x = 10)
	execute_check("n", 68, x = 10, y = 10)
	execute_check("n", 70, x = 10, y = 10)
	execute_check("n", 71, x = 5, y = 10)
	execute_check("n", 73, x = 5, y = 10)
	execute_check("n", 77, x = 5, y = 15)
	execute_check("n", 78, x = 5, y = 15, z = 20)
	gdb.execute('quit')
except MismatchException, e:
	gdb.execute('quit 1') # quit with error code

