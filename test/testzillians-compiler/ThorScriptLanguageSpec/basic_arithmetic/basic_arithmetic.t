package com.zillians.langspec.basic_arithmetic;

import system;

class test_arithmetic {
	public static function case0() {
		var a:int32 = 0;
		
		a = 1+2*3; 
		check(a == 7);
		a = 1*2+3; 
		check(a == 5);
		
		a = 1+2*3*4/2; 
		check(a == 13);
		
		a = 1/1+2*3*4/2; 
		check(a == 13);
		
		a = (1+2)*3*4/2; 
		check(a == 18);
		
		a = ((1+2)*3) + 4) * 2; 
		check(a == 26);
		
		a = ((-1+2)*3) + 4) * 2; 
		check(a == 14);
		
		a = 1 + -((1+2)*3) + 4) * -1; 
		check(a == 14);
		
		a = ((1+2) * (3+4)) / 3; 
		check(a == 7);
				
		a = ((((((((((((2)))))))))))) * (3+4) / 2; 
		check(a == 7);		
	}
	
	public static function case1() {
		var a:int32 = 1;
		var b:int32 = 2;
		var c:int32 = 3;
		var d:int32 = 4;
		var e:int32 = 5;
		
		var r:int32;
		
		r = a + b;
		check(r == 3);
		
		r = a + b * c;
		check(r == 7);
		
		r = a * b + c;
		check(r == 5);
		
		r = a * b * c;
		check(r == 6);
		
		r = a * b + c * d;
		check(r == 14);
		
		r = a * (b + c) * d;
		check(r == 20);
		
		r = a * (b + c * d);
		check(r == 14);
		
		r = a * (b + c * d) / e;
		check(r == 2);
		
		r = a * (b + c / d) * e;
		check(r == 10);
	}
	
	public static function case2() {
		// test case for mixing float and integer (test the floating point promotion rule)
		/*
		var a:int32 = 1;
		var b:float32 = 2.0f;
		var c:int32 = 3;
		var d:int32 = 4;
		var e:int32 = 5;
		
		var r:int32;
		
		r = a + b;
		check(r == 3);
		
		r = a + b * c;
		check(r == 7);
		
		r = a * b + c;
		check(r == 5);
		
		r = a * b * c;
		check(r == 6);
		
		r = a * b + c * d;
		check(r == 14);
		
		r = a * (b + c) * d;
		check(r == 20);
		
		r = a * (b + c * d);
		check(r == 14);
		
		r = a * (b + c * d) / e;
		check(r == 2);
		
		r = a * (b + c / d) * e;
		check(r == 10);
		*/
	}		
}