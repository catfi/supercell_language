import system;

class test_array {

	public static function case0() {
		var array:Array<int32> = new Array<int32>(); // by default constructor, it constructs a zero-length std::vector-like object
		
		array.reserve(3); // reserve memory for 3 elements
		
		check(array.length() == 3);
		
		array[0] = 0; // array index starts from 0
		array[1] = 1;
		array[2] = 2; // array index ends at N-1
		//array[3] = 3; // this should throw exception?
		
		check(array[0] == 0);
		check(array[1] == 1);
		check(array[2] == 2);
		
		array.insert(0, 0);
		
		check(array.length() == 4);
		
		check(array[0] == 0);
		check(array[1] == 0);
		check(array[2] == 1);
		check(array[3] == 2);
		
		check(array.pop() == 2); // pop remove the last element and return it
		check(array.pop() == 1);
		check(array.pop() == 0);
		check(array.pop() == 0);
		
		check(array.push(1) == 1); // push returns the new length of the array
		check(array.push(2) == 2);
		check(array.push(3) == 3);
		
		array.reverse(); // reverse the order of all elements
		
		check(array[0] == 3);
		check(array[1] == 2);
		check(array[2] == 1);
		
		array.sort(function(x:int32,y:int32) { // this would sort in increasing order
			return x - y;
		});
		
		check(array[0] == 1);
		check(array[1] == 2);
		check(array[2] == 3);
	}
	
	public static function case1() {
		var array:Array<int32> = new Array<int32>(10000); // test if we can allocate large array
		
		foreach(var i in 0...10000) {
			array[i] = i;
		}
		
		foreach(var i in 0...10000) {
			check(array[i] == i);
		}
	}
	
	public static function case2() {
		var array:Array<int32> = new Array<int32>(0); // test if we can allocate zero length array
		
		check(array.length() == 0);
		foreach(var i in 0...10000) {
			array.push(i);
		}
		
		foreach(var i in 0...10000) {
			check(array.pop() == 10000 - i);
		}
	}
	
	public static function case3() {
		var array:Array<int32> = new Array<int32>(0);
		
		check(array.length() == 0);
		foreach(var i in 0...10000) {
			array.push(i);
		}
		
		var another_array:Array<int32> = array.copy(); // verify the array copy is working
		
		array.clear(); // remove all elements
		
		foreach(var i in 0...10000) {
			check(another_array.pop() == 10000 - i);
		}
	}
	
	public static function case4() {
		var array0:Array<int32> = new Array<int32>(0);
		var array1:Array<int32> = new Array<int32>(0);
		
		check(array.length() == 0);
		foreach(var i in 0...10000) {
			array0.push(i);
		}
		
		foreach(var i in 10000...20000) {
			array1.push(i);
		}
		
		array0.append(array1); // append the content of array1 into array0
		
		foreach(var i in 0...20000) {
			check(array0.pop() == 20000 - i);
		}
	}	
}