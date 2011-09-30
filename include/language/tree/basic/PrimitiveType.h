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
		VOID               = 0,

		BOOL               = 1,

		INT8               = 2,
		INT16              = 3,
		INT32              = 4,
		INT64              = 5,
		INT128             = 6,

		FLOAT32            = 7,
		FLOAT64            = 8,

		OBJECT             = 9,
		FUNCTION           = 10,
		STRING             = 11,
		VARIADIC_ELLIPSIS  = 12,
	};

	static bool isIntegerType(type t)
	{
		return (t >= BOOL && t <= INT128);
	}

	static bool isArithmeticCapable(type t)
	{
		return (t >= BOOL && t <= FLOAT64);
	}

	static bool isFloatType(type t)
	{
		return (t == FLOAT32 || t == FLOAT64);
	}

	static type promote(type t0, type t1, bool& precision_loss)
	{
		BOOST_ASSERT(isArithmeticCapable(t0));
		BOOST_ASSERT(isArithmeticCapable(t1));

		// GOAL:
		// signed + unsigned = signed,
		// unsigned + unsigned = unsigned,
		// signed + signed = signed,
		// float + any = float

		type result = VOID;
		if(isFloatType(t0) || isFloatType(t1))
		{
			// either t0 or t1 is floating point type, we should promote the type to the largest floating point representation
			if(t0 == FLOAT32 || t1 == FLOAT32) result = FLOAT32;
			if(t0 == FLOAT64 || t1 == FLOAT64) result = FLOAT64;
		}
		else
		{
			int minimal_bit_size_t0;
			{
				minimal_bit_size_t0 = (int)t0 - (int)INT8;

				if(minimal_bit_size_t0 >= 4)
				{
					// overflowed, causing precision loss
					precision_loss = true;
					minimal_bit_size_t0 = 3;
				}
			}
			int minimal_bit_size_t1;
			{
				minimal_bit_size_t1 = (int)t1 - (int)INT8;

				if(minimal_bit_size_t1 >= 4)
				{
					// overflowed, causing precision loss
					precision_loss = true;
					minimal_bit_size_t1 = 3;
				}
			}

			result = (type)((int)INT8 + std::max(minimal_bit_size_t0, minimal_bit_size_t1));
		}

		return result;
	}

	static int bitSize(type t)
	{
		switch(t)
		{
		case VOID: return 0;
		case BOOL: return 1;
		case INT8: return 8;
		case INT16: return 16;
		case INT32: return 32;
		case FLOAT32: return 32;
		case INT64: return 64;
		case FLOAT64: return 64;
		case INT128: return 128;
		case OBJECT: return 32;
		case FUNCTION: return 32;
		case STRING: return 32;
		case VARIADIC_ELLIPSIS: return -1;
		default: break;
		}
		return -1;
	}

	static bool isImplicitConvertible(type from, type to, bool& precision_loss)
	{
		if(isArithmeticCapable(from) && isArithmeticCapable(to))
		{
			precision_loss = (bitSize(from) > bitSize(to));
			return true;
		}
		else if( (from == OBJECT && to == OBJECT) || (from == FUNCTION && to  == FUNCTION) || (from == STRING && to == STRING) )
		{
			precision_loss = false;
			return true;
		}
		return false;
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
		case INT128: return L"int128";
		case FLOAT32: return L"float32";
		case FLOAT64: return L"float64";
		case OBJECT: return L"object";
		case FUNCTION: return L"function";
		case STRING: return L"string";
		case VARIADIC_ELLIPSIS: return L"...";
		default: break;
		}
		BOOST_ASSERT(false && "reaching unreachable code");
		return NULL;
	}
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_PRIMITIVE_H_ */
