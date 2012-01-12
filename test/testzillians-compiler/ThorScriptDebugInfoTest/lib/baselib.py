import gdb
import os 


#########################################################
#
# Customized Exception
#
#########################################################
class MismatchException(Exception):
	def __init__(self):
		pass

#########################################################
#
# Utility Function
#
#########################################################
def get_current_line():
	try:
		frame = gdb.selected_frame()
		sym = frame.find_sal()
		return sym.line
	except:
		return None

def get_current_file():
	try:
		frame = gdb.selected_frame()
		sym = frame.find_sal()
		return os.path.basename(sym.symtab.filename)
	except:
		return None

#########################################################
#
# Expose API
#
#########################################################
def lazy_generate_mode(*vars, **flags):
	"""
		When to use it:
			Sometimes, we are too lazy to examine each step and filled out the variables we want to examine
			when generating the test script. An alternative way is to first attach the target program, and 
			make sure the tracing is correct. Then use this function to generate the script.
	"""
	gdb.execute("run")
	print 'execute_check("run", ', get_current_line(), ')'
	
	max_steps = 30
	if flags.has_key('max_step'):
		max_steps = flags['max_step']

	step = 0
	while step < max_steps:
		gdb.execute("s")
		buf = 'execute_check("s", ' + str(get_current_line()) + ', EXPECT_FILE = "' + str(get_current_file()) +'"'
		for var in vars:
			try:
				value = gdb.parse_and_eval(var)
				print var, value
				buf += ', ' + str(var) + ' = ' + str(value)
			except:
				continue
		buf += ')'
		print buf
		step += 1


def execute_check(command, *args, **eval_expr):
	"""
		FORMAT: eval_expr = {'expr': expect value}
	
		execute and check whether the operation meets the expected value
	"""
	gdb.execute(command)

	if not args and not eval_expr:
		return

	expect_line = None
	if args:
		expect_line = args[0]

	# Check expected line
	current_line = get_current_line()
	result = True 
	if expect_line and current_line != expect_line:
		result = False
		print 'Line mismatched : ', current_line, ' expected: ', expect_line

	# Check evaluation of expression
	if eval_expr and result:
		for expr, expected in eval_expr.items():
			if expr == 'EXPECT_FILE':
				value = get_current_file()
			else:
				value = gdb.parse_and_eval(expr)

			if value != expected:
				result = False
				print current_line, ': Mismatched: ', expr, ' = ', value, ' expected: ', expected
				break

	if not result:
		raise MismatchException()

