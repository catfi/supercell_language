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

#ifndef ZILLIANS_LANGUAGE_STAGE_GENERATOR_DETAIL_LLVMFOREACH_H_
#define ZILLIANS_LANGUAGE_STAGE_GENERATOR_DETAIL_LLVMFOREACH_H_

#include "utility/Foreach.h"
#include "language/stage/generator/detail/LLVMHeaders.h"

namespace zillians {

template<>
struct foreach_trait<llvm::Function>
{
	typedef llvm::Function container_type;
	typedef container_type::iterator iterator_type;
	typedef container_type::const_iterator const_iterator_type;

	static inline iterator_type beginof(container_type& a)
	{ return a.begin(); }

	static inline iterator_type endof(container_type& a)
	{ return a.end(); }

	static inline const_iterator_type beginof(const container_type& a)
	{ return a.begin(); }

	static inline const_iterator_type endof(const container_type& a)
	{ return a.end(); }
};

template<>
struct foreach_trait<llvm::BasicBlock>
{
	typedef llvm::BasicBlock container_type;
	typedef container_type::iterator iterator_type;
	typedef container_type::const_iterator const_iterator_type;

	static inline iterator_type beginof(container_type& a)
	{ return a.begin(); }

	static inline iterator_type endof(container_type& a)
	{ return a.end(); }

	static inline const_iterator_type beginof(const container_type& a)
	{ return a.begin(); }

	static inline const_iterator_type endof(const container_type& a)
	{ return a.end(); }
};

template<>
struct foreach_trait<llvm::Module>
{
	typedef llvm::Module container_type;
	typedef container_type::iterator iterator_type;
	typedef container_type::const_iterator const_iterator_type;

	static inline iterator_type beginof(container_type& a)
	{ return a.begin(); }

	static inline iterator_type endof(container_type& a)
	{ return a.end(); }

	static inline const_iterator_type beginof(const container_type& a)
	{ return a.begin(); }

	static inline const_iterator_type endof(const container_type& a)
	{ return a.end(); }
};

}

#endif /* ZILLIANS_LANGUAGE_STAGE_GENERATOR_LLVMFOREACH_H_ */
