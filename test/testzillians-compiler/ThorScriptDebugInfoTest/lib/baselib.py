import gdb

class MismatchException(Exception):
	def __init__(self):
		pass

def get_current_line():
	try:
		frame = gdb.selected_frame()
		sym = frame.find_sal()
		return sym.line
	except:
		return None

def lazy_generate_mode(*vars):
	gdb.execute("run")
	print 'execute_check("run", ', get_current_line(), ')'

	while True:
		gdb.execute("n")
		buf = 'execute_check("s", ' + str(get_current_line())
		for var in vars:
			value = gdb.parse_and_eval(var)
			print var, value
			buf += ', ' + str(var) + ' = ' + str(value)
		buf += ')'
		print buf

#
# FORMAT: eval_expr = {'expr': expect value}
#
def execute_check(command, *args, **eval_expr):
	gdb.execute(command)

	if not args and not eval_expr:
		return

	expect_line = None
	if args:
		expect_line = args[0]

	current_line = get_current_line()
	result = True 
	if expect_line and current_line != expect_line:
		result = False
		print 'Line mismatched : ', current_line, ' expected: ', expect_line

	if eval_expr and result:
		for expr, expected in eval_expr.items():
			value = gdb.parse_and_eval(expr)
			if (value != expected):
				result = False
				print current_line, ': Expr mismatched: ', expr, ' = ', value, ' expected: ', expected
				break

	if not result:
		raise MismatchException()

