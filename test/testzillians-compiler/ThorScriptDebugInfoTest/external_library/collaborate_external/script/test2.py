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
	execute_check("run",  16 )
	execute_check("s", 18, EXPECT_FILE = "test.t")
	execute_check("s", 10, EXPECT_FILE = "print.cpp", x = 3000)
	execute_check("s", 11, EXPECT_FILE = "print.cpp", x = 3000)
	execute_check("s", 19, EXPECT_FILE = "test.t")
	gdb.execute('quit')
except MismatchException, e:
	gdb.execute('quit 1') # quit with error code

