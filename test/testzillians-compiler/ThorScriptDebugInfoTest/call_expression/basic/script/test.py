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
	execute_check("run",  8 )
	execute_check("s", 10)
	execute_check("s", 1)
	execute_check("s", 3, a = 100, b = 200)
	execute_check("s", 4, x = 0, a = 100, b = 200)
	execute_check("s", 5, x = 300, a = 100, b = 200)
	execute_check("s", 6, a = 100, b = 200)
	execute_check("s", 10)
	execute_check("s", 11, x = 300)
	execute_check("s", 12)
	gdb.execute('quit')
except MismatchException, e:
	gdb.execute('quit 1') # quit with error code

