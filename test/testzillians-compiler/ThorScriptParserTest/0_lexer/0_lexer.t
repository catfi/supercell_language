class D<T>
{ }

class C<X,Y>
{ }

class C<X,Y:D<T/*:X*/> >
{ }

class C<X,Y:int32>
{ }

class T0
{ }

function g():void
{
	var c:C<T0,D<T0> >;
}

/*
function print<T>(v:T)
{
	print_value(v);
}

function print<T, ...>(v:T, rest:...)
{
	print_value(v);
	print(rest); 
}

function printImpl(v:_VOID)
{ }

function printImpl(v:int32)
{ }

function printImpl(v:int64)
{ }

function print<T0, T1 = void, T2 = void, T3 = void, T4 = void>(t0:T0, t1:T1 = void(), t2:T2 = void(), t3:T3 = void(), t4:T4 = void()):void
{
	printImpl(t0);
	printImpl(t1);
	printImpl(t2);
	...
}

class Tuple<T0, T1 = void, T2 = void, ..... T10 = void> 
{
	var t0:T0;
	var t1:T1;
	var t2:T2;
	var t3:T3;
	var t4:T4;
	...
	var t10:T10;
}
*/

