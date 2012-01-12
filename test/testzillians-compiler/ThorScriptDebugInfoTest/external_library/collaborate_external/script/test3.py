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
	execute_check("run",  23 )
	execute_check("s", 17, EXPECT_FILE = "print.cpp")
	execute_check("s", 28, EXPECT_FILE = "test.t", x = 0, y = 0)
	execute_check("s", 29, EXPECT_FILE = "test.t", x = 3, y = 0)
	execute_check("s", 31, EXPECT_FILE = "test.t", x = 3, y = 8)
	execute_check("s", 32, EXPECT_FILE = "test.t")
	execute_check("s", 18, EXPECT_FILE = "print.cpp")
	execute_check("s", 24, EXPECT_FILE = "test.t")
	gdb.execute('quit')
except MismatchException, e:
	gdb.execute('quit 1') # quit with error code

