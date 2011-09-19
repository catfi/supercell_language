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

#ifndef ZILLIANS_LANGUAGE_TREE_PRIMITIVE_H_
#define ZILLIANS_LANGUAGE_TREE_PRIMITIVE_H_

#include "core/Types.h"
#include "language/tree/ASTNode.h"

namespace zillians { namespace language { namespace tree {

struct PrimitiveType
{
	enum type {
		VOID,

		BOOL,
		UINT8,
		UINT16,
		UINT32,
		UINT64,

		INT8,
		INT16,
		INT32,
		INT64,

		FLOAT32,
		FLOAT64,

		ANONYMOUS_OBJECT,
		ANONYMOUS_FUNCTION,
		VARIADIC_ELLIPSIS,
	};

	static bool isIntegerType(type t)
	{
		return (t >= BOOL && t <= INT64);
	}

	static bool isSignedIntegerType(type t)
	{
		return (t >= INT8 && t <= INT64);
	}

	static bool isUnsignedIntegerType(type t)
	{
		return (t >= BOOL && t <= UINT64);
	}

	static bool isFloatType(type t)
	{
		return (t == FLOAT32 || t == FLOAT64);
	}

	static const wchar_t* toString(type t)
	{
		switch(t)
		{
		case VOID:	return L"void";
		case BOOL:  return L"bool";
		case INT8:  return L"int8";
		case INT16: return L"int16";
		case INT32: return L"int32";
		case INT64: return L"int64";
		case UINT8:  return L"uint8";
		case UINT16: return L"uint16";
		case UINT32: return L"uint32";
		case UINT64: return L"uint64";
		case FLOAT32: return L"float32";
		case FLOAT64: return L"float64";
		case ANONYMOUS_OBJECT: return L"object";
		case ANONYMOUS_FUNCTION: return L"function";
		case VARIADIC_ELLIPSIS: return L"...";
		default: break;
		}
		BOOST_ASSERT(false && "reaching unreachable code");
		return NULL;
	}
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_PRIMITIVE_H_ */
