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
		VOID_TYPE               = 0,

		BOOL_TYPE               = 1,

		INT8_TYPE               = 2,
		INT16_TYPE              = 3,
		INT32_TYPE              = 4,
		INT64_TYPE              = 5,
		INT128_TYPE             = 6,

		FLOAT32_TYPE            = 7,
		FLOAT64_TYPE            = 8,

		OBJECT_TYPE             = 9,
		FUNCTION_TYPE           = 10,
		STRING_TYPE             = 11,
		VARIADIC_ELLIPSIS_TYPE  = 12,
	};

	static bool isIntegerType(type t)
	{
		return (t >= BOOL_TYPE && t <= INT128_TYPE);
	}

	static bool isArithmeticCapable(type t)
	{
		return (t >= BOOL_TYPE && t <= FLOAT64_TYPE);
	}

	static bool isFloatType(type t)
	{
		return (t == FLOAT32_TYPE || t == FLOAT64_TYPE);
	}

	static type promote(type t0, type t1, bool& precision_loss)
	{
		using namespace std;
		BOOST_ASSERT(isArithmeticCapable(t0));
		BOOST_ASSERT(isArithmeticCapable(t1));
		
		// GOAL:
		// signed + unsigned = signed,
		// unsigned + unsigned = unsigned,
		// signed + signed = signed,
		// float + any = float

		type result = VOID_TYPE;
		if(isFloatType(t0) || isFloatType(t1))
		{
			// either t0 or t1 is floating point type, we should promote the type to the largest floating point representation
			if(t0 == FLOAT32_TYPE || t1 == FLOAT32_TYPE) result = FLOAT32_TYPE;
			if(t0 == FLOAT64_TYPE || t1 == FLOAT64_TYPE) result = FLOAT64_TYPE;
		}
		else
		{
			int minimal_bit_size_t0;
			{
				minimal_bit_size_t0 = (int)t0 - (int)INT8_TYPE;

				if(minimal_bit_size_t0 >= 4)
				{
					// overflowed, causing precision loss
					precision_loss = true;
					minimal_bit_size_t0 = 3;
				}
			}
			int minimal_bit_size_t1;
			{
				minimal_bit_size_t1 = (int)t1 - (int)INT8_TYPE;

				if(minimal_bit_size_t1 >= 4)
				{
					// overflowed, causing precision loss
					precision_loss = true;
					minimal_bit_size_t1 = 3;
				}
			}

			result = (type)((int)INT8_TYPE + max(minimal_bit_size_t0, minimal_bit_size_t1));
		}

		return result;
	}

	static int byteSize(type t)
	{
		switch(t)
		{
		case VOID_TYPE: return 0;
		case BOOL_TYPE: return 1;
		case INT8_TYPE: return 1;
		case INT16_TYPE: return 2;
		case INT32_TYPE: return 4;
		case FLOAT32_TYPE: return 4;
		case INT64_TYPE: return 8;
		case FLOAT64_TYPE: return 8;
		case INT128_TYPE: return 16;
		case OBJECT_TYPE: return 4;
		case FUNCTION_TYPE: return 4;
		case STRING_TYPE: return 4;
		case VARIADIC_ELLIPSIS_TYPE: return -1;
		default: break;
		}
		return -1;
	}

	static bool isImplicitConvertible(type from, type to, bool& precision_loss)
	{
		if(isArithmeticCapable(from) && isArithmeticCapable(to))
		{
			precision_loss = (byteSize(from) > byteSize(to));
			return true;
		}
		else if( (from == OBJECT_TYPE && to == OBJECT_TYPE) || (from == FUNCTION_TYPE && to  == FUNCTION_TYPE) || (from == STRING_TYPE && to == STRING_TYPE) )
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
		case VOID_TYPE:	return L"void";
		case BOOL_TYPE:  return L"bool";
		case INT8_TYPE:  return L"int8";
		case INT16_TYPE: return L"int16";
		case INT32_TYPE: return L"int32";
		case INT64_TYPE: return L"int64";
		case INT128_TYPE: return L"int128";
		case FLOAT32_TYPE: return L"float32";
		case FLOAT64_TYPE: return L"float64";
		case OBJECT_TYPE: return L"object";
		case FUNCTION_TYPE: return L"function";
		case STRING_TYPE: return L"string";
		case VARIADIC_ELLIPSIS_TYPE: return L"...";
		default: UNREACHABLE_CODE(); return NULL;
		}
	}
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_PRIMITIVE_H_ */
