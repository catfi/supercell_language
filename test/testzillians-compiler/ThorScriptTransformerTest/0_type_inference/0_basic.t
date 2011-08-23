package com.zillians.compiler_test;

class some_class_decl
{
}

function f0(x:some_class_decl, y:some_class_decl):some_class_decl
{
	var temp:some_class_decl = x;
	return temp;
}

function f1(x, y)
{
	var temp = x;
	return temp;
}

function f1_caller()
{
	//var x = new some_class_decl();
	//var y = new some_class_decl();
	//var result = f1(x, y);
}
