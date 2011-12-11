/**
 * Zillians MMO
 * Copyright (C) 2007-2011 Zillians.com, Inc.
 * For more information see http://www.zillians.com
 *
 * Zillians MMO is the library and runtime for massive multiplayer online game
 * development in utility computing model, which runs as a service for every
 * developer to build their virtual world running on our GPU-assisted machines.
 *
 * This is a close source library intended to be used solely within Zillians.com
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
 
@system
class Array<T> extends Object
{
	@system
	private function new():Array<T>;

	@system
	public function new(ref:Array<T>):Array<T>;
	
	@system
	public function new(size:int32):Array<T>;
	
	@system
	public function get(index:int32):T;

	@system
	public function set(index:int32, value:T):void;
	
	@system
	public function size():int32;
}

@system
class Vector<T> extends Object
{
	// TODO complete all method definitions
	@system 
	public function size():int32;

	@system
	public clear():void;
	
	@system
	public function front():T

	@system
	public function back():T
	
	@system 
	public function pushBack(element:T):void
	
	@system
	public function popBack():void
}

@system
class List<T> extends Object
{ 
	@system 
	public function size():int32;
	
	@system
	public clear():void;

	@system
	public function front():T;

	@system
	public function back():T;
	
	@system 
	public function pushFront(element:T):void;
	
	@system
	public function popFront():void;
	
	@system 
	public function pushBack(element:T):void;
	
	@system
	public function popBack():void;
}

@system
class Queue<T> extends Object
{ 
	@system 
	public function size():int32;
	
	@system
	public clear():void;
	
	@system
	public function head():T;

	@system
	public function tail():T;
	
	@system 
	public function push(element:T):void;
	
	@system
	public function pop():void;
}

@system
class HashMap<K,V> extends Object
{ 
	@system 
	public function size():int32;

	@system
	public clear():void;
	
	@system
	public set(key:K, value:V):void;
	
	@system
	public get(key:K):V;
}

@system
class HashSet<V> extends Object
{ 
	@system 
	public function size():int32;

	@system
	public clear():void;
	
	@system
	public add(value:V):void;

	@system
	public remove(value:V):boolean;
	
	@system
	public has(value:V):boolean;
}

@system
class Iterator<T>
{
	@system
	public function get():T;
	
	@system
	public function set(e:T):void;
	
	@system
	public function next():Iterator<T>;
}

// whenever a foreach is used with "..." syntax, a range object is automatically constructed
@system
class Range<T>
{
	@system
	public function begin():Iterator<T>;
	
	@system
	public function end():Iterator<T>;
}


