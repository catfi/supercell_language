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
	from lib.baselib import lazy_generate_mode
	lazy_generate_mode('x', 'y', 'result', 'index', 'i', 'ans', 'pre1', 'pre2')
	gdb.execute('quit')
except MismatchException, e:
	gdb.execute('quit 1') # quit with error code

