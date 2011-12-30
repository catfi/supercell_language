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

// define the convention type conversion routine
@system 
function cast<X,Y>(value:Y):X;

// define the convention type conversion check routine
@system 
function isa<X,Y>(value:Y):boolean;

@system
class Object
{
	@system
	public function clone():Object;
	
	@system
	public function hash():int64;
	
	@system
	public function domain():int32;	
}

@system
class String : Object
{
	@system
	public function length():int32;
	
	@system
	public function concate(other:String):String;
	
	@system
	public function extract(from:int32, to:int32 = -1):String;
	
	@system
	public function find(candidate:String):int32;
	
	@system
	public function tokenize(s:String):Vector<String>;
}
