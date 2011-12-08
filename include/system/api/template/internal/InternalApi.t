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

import . = api.system.unmanaged;
 
class ImplicitVariable { }

class ThreadId : ImplicitVariable {
	function init():void {
		@register
		var blockDimX:int32;

		@register
		var blockIdxX:int32;

		@register
		var threadIdxX:int32;

		value = blockDimX * blockIdxX + threadIdxX;
	}
	var value:int32;
}

@global
var __global_debug_service_api_buffer:ptr_<ptr_<int8> >;

class DebugServiceApiBuffer : ImplicitVariable {
	function init():void {
		value = deref<ptr_<ptr_<int8> >, ptr_<int8> >(DebugServiceApiBuffer);
	}
	var value:ptr_<int8>;
};

@global
var __global_game_object_service_api_buffer:ptr_<ptr_<int8> >;

class GameObjectServiceApiBuffer : ImplicitVariable {
	function init():void {
		value = deref<ptr_<ptr_<int8> >, ptr_<int8> >(__global_game_object_service_api_buffer);
	}
	var value:ptr_<int8>;
};
