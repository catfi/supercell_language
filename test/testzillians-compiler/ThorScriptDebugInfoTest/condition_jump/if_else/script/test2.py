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
	execute_check("run",  48 )
	execute_check("n", 51, x = 10)
	execute_check("n", 57, x = 10)
	execute_check("n", 60, x = 4)
	gdb.execute('quit')
except MismatchException, e:
	gdb.execute('quit 1') # quit with error code

