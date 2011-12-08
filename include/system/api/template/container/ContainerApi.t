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
 
package api.system;

@system
class Array<T> : Object
{
	// TODO complete all method definitions
}

@system
class Vector<T> : Object
{
	// TODO complete all method definitions
	@system 
	public function size():uint32;
	
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
class Vector<T> : Object
{ 
	// TODO complete all method definitions
}

@system
class List<T> : Object
{ 
	// TODO complete all method definitions
}

@system
class Queue<T> : Object
{ 
	// TODO complete all method definitions
}

@system
class HashMap<K,V> : Object
{ 
	// TODO complete all method definitions
}

@system
class HashSet<V> : Object
{ 
	// TODO complete all method definitions
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


