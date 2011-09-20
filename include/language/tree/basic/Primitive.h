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

	static bool isArithmeticCapable(type t)
	{
		return (t >= UINT8 && t <= FLOAT64);
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

		type result;
		if(isFloatType(t0) || isFloatType(t1))
		{
			// either t0 or t1 is floating point type, we should promote the type to the largest floating point representation
			if(t0 == FLOAT32 || t1 == FLOAT32) result = FLOAT32;
			if(t0 == FLOAT64 || t1 == FLOAT64) result = FLOAT64;
		}
		else
		{
			// all integer, we should find the largest integer representation that can contain both t0 and t1
			if(isSignedIntegerType(t0) || isSignedIntegerType(t1))
			{
				// if either t0 or t1 is signed integer, the result must be signed integer
				int minimal_bit_size_t0;
				{
					if(isSignedIntegerType(t0)) minimal_bit_size_t0 = (int)t0 - (int)INT8;
					else                        minimal_bit_size_t0 = (int)t0 - (int)UINT8 + 1;

					if(minimal_bit_size_t0 >= 4)
					{
						// overflowed, causing precision loss
						precision_loss = true;
						minimal_bit_size_t0 = 3;
					}
				}
				int minimal_bit_size_t1;
				{
					if(isSignedIntegerType(t1)) minimal_bit_size_t1 = (int)t1 - (int)INT8;
					else                        minimal_bit_size_t1 = (int)t1 - (int)UINT8 + 1;

					if(minimal_bit_size_t1 >= 4)
					{
						// overflowed, causing precision loss
						precision_loss = true;
						minimal_bit_size_t1 = 3;
					}
				}
				result = (type)((int)INT8 + std::max(minimal_bit_size_t0, minimal_bit_size_t1));
			}
		}

		return result;
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
