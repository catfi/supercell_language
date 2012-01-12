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
	execute_check("run",  13 )
	execute_check("s", 5, EXPECT_FILE = "print.cpp")
	execute_check("s", 6, EXPECT_FILE = "print.cpp")
	execute_check("s", 14, EXPECT_FILE = "test.t")
	gdb.execute('quit')
except MismatchException, e:
	gdb.execute('quit 1') # quit with error code

