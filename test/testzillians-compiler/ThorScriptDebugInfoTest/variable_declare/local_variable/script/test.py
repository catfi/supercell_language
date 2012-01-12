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
	execute_check("n", 4, x = 34)
	execute_check("n", 5, x = 34, y = 3)
	execute_check("n", 7, x = 34, y = 3, z = 11)
	gdb.execute('quit')
except MismatchException, e:
	gdb.execute('quit 1') # quit with error code

