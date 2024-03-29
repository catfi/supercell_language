function test():void {
	var a:int8 = 0;
	var b:int16 = 0;
	var c:int32 = 0;
	var d:int64 = 0;
	
	var e:float32 = 0;
	var f:float64 = 0;
	
	// these will all result in precision loss, so warnings will be thrown here
	/*
	a = b;
	b = c;
	c = d;
	
	a = e;
	b = e;
	
	a = f;
	b = f;
	c = f;
	*/
	// these are fine as we cast a smaller type to a larger type
	/*
	d = c;
	c = b;
	b = a;
	*/
	
	// float to integer can be implicitly casted
	/*
	a = e;
	a = f;
	b = e;
	b = f;
	c = e;
	c = f;
	d = e;
	d = f;
	*/
	
	// and vice versa
	/*
	e = a;
	e = b;
	e = c;
	e = d;
	f = a;
	f = b;
	f = c;
	f = d;
	*/
	
	// double can be implicit casted as float (and vice versa)
	/*
	e = f;
	f = e;
	*/
	
	// implicit cast from integer to boolean
	/*
	if(a) { }
	elif(b) { }
	elif(c) { }
	elif(d) { }
	elif(e) { }
	elif(f) { }
	else { }
	*/
	
	//a = b + c + d;
	
}

