@native
function print() : void;

@native
function print_value(x:int16) : void;

@native
function please_callback() : void;


function test1() : void
{
	print();
}

function test2() : void
{
	print_value(3000);
}

function test3() : void
{
	please_callback();
}

function callback() : void
{
	var x : int16 = 3;
	var y : int16 = 8;

	x = x + y;
}
