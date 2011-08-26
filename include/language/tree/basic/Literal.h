/**
 * Zillians MMO
 * Copyright (C) 2007-2010 Zillians.com, Inc.
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
/**
 * @date Aug 5, 2011 sdk - Initial version created.
 */

#ifndef ZILLIANS_LANGUAGE_TREE_LITERAL_H_
#define ZILLIANS_LANGUAGE_TREE_LITERAL_H_

#include "core/Types.h"
#include "language/tree/ASTNode.h"

namespace zillians { namespace language { namespace tree {

struct Expression;

struct Literal : public ASTNode
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(Literal, (Literal)(ASTNode));
};

struct ObjectLiteral : public Literal
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(ObjectLiteral, (ObjectLiteral)(Literal)(ASTNode));

	struct LiteralType {
		enum type {
			NULL_OBJECT,
			SELF_OBJECT,
			GLOBAL_OBJECT,
		};

		static const wchar_t* toString(type t)
		{
			switch(t)
			{
			case NULL_OBJECT: return L"null";
			case SELF_OBJECT: return L"self";
			case GLOBAL_OBJECT: return L"global";
			}
		}
	};

	explicit ObjectLiteral(LiteralType::type t) : type(t)
	{ }

	LiteralType::type type;
};

struct NumericLiteral : public Literal
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(NumericLiteral, (NumericLiteral)(Literal)(ASTNode));

	struct LiteralType {
		enum type {
			BOOL,
			I8,
			I16,
			I32,
			I64,
			U8,
			U16,
			U32,
			U64,
			F32,
			F64,
		};

		static const wchar_t* toString(type t)
		{
			switch(t)
			{
			case BOOL: return L"bool";
			case I8:  return L"int8";
			case I16: return L"int16";
			case I32: return L"int32";
			case I64: return L"int64";
			case U8:  return L"uint8";
			case U16: return L"uint16";
			case U32: return L"uint32";
			case U64: return L"uint64";
			case F32: return L"float32";
			case F64: return L"float64";
			}
		}
	};

	explicit NumericLiteral(bool v)	 { type = LiteralType::BOOL;  value.b = v;  }
	explicit NumericLiteral(int8 v)	 { type = LiteralType::I8;  value.i8 = v;  }
	explicit NumericLiteral(int16 v) { type = LiteralType::I16; value.i16 = v; }
	explicit NumericLiteral(int32 v) { type = LiteralType::I32; value.i32 = v; }
	explicit NumericLiteral(int64 v) { type = LiteralType::I64; value.i64 = v; }

	explicit NumericLiteral(uint8 v)  { type = LiteralType::U8;  value.u8 = v;  }
	explicit NumericLiteral(uint16 v) { type = LiteralType::U16; value.u16 = v; }
	explicit NumericLiteral(uint32 v) { type = LiteralType::U32; value.u32 = v; }
	explicit NumericLiteral(uint64 v) { type = LiteralType::U64; value.u64 = v; }

	explicit NumericLiteral(float v)  { type = LiteralType::F32; value.f32 = v; }
	explicit NumericLiteral(double v) { type = LiteralType::F64; value.f64 = v; }

	LiteralType::type type;

	union
	{
		bool  b;
		int8  i8;
		int16 i16;
		int32 i32;
		int64 i64;

		uint8  u8;
		uint16 u16;
		uint32 u32;
		uint64 u64;

		float  f32;
		double f64;
	} value;
};

struct StringLiteral : public Literal
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(StringLiteral, (StringLiteral)(Literal)(ASTNode));

	explicit StringLiteral(const std::wstring& s) : value(s)
	{ }

	template<typename Iterator>
	explicit StringLiteral(Iterator begin, Iterator end)
	{
		const std::size_t length = end - begin;
		value.resize(length + 1);
		for(std::size_t i = 0; i != length; ++i)
			value.push_back(*begin++);
	}

	std::wstring value;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_LITERAL_H_ */
