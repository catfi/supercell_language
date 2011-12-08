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

class Iterator<T>
{
	public function new(e:T)
	{
		element = e;
	}
	
	public function get():T
	{
		return element;
	}
	
	public function set(e:T):void
	{
		element = e;
	}
	
	public function next():Iterator<T>
	{
		return ++element;
	} 
	
	private var element:T;
}

// whenever a foreach is used with "..." syntax, a range object is automatically constructed
@native
class Range<T>
{
	// TODO we need to check whether T is of iterator concept
	// TODO or we have to redefine T as Iterator<T>
	@native
	public function begin():Iterator<T>;
	
	@native
	public function end():Iterator<T>;
}
