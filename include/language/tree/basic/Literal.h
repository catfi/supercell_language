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
#include "language/tree/basic/PrimitiveType.h"

namespace zillians { namespace language { namespace tree {

struct Expression;

struct Literal : public ASTNode
{
	friend class boost::serialization::access;

	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(Literal, (Literal)(ASTNode));

	virtual std::wstring toString() const = 0;

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & boost::serialization::base_object<ASTNode>(*this);
    }
};

struct ObjectLiteral : public Literal
{
	friend class boost::serialization::access;

	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(ObjectLiteral, (ObjectLiteral)(Literal)(ASTNode));

	struct LiteralType {
		enum type {
			NULL_OBJECT,
			SELF_OBJECT,
			THIS_OBJECT,
			SUPER_OBJECT,
			GLOBAL_OBJECT,
		};

		static const wchar_t* toString(type t)
		{
			switch(t)
			{
			case NULL_OBJECT: return L"null";
			case SELF_OBJECT: return L"self";
			case THIS_OBJECT: return L"this";
			case SUPER_OBJECT: return L"super";
			case GLOBAL_OBJECT: return L"global";
            default : UNREACHABLE_CODE(); return L"<unknown>";
			}
		}
	};

	explicit ObjectLiteral(LiteralType::type t) : type(t)
	{ }

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE()
		COMPARE_MEMBER(type)
		END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	UNUSED_ARGUMENT(from);
    	UNUSED_ARGUMENT(to);
    	UNUSED_ARGUMENT(update_parent);

    	return false;
    }

    virtual ASTNode* clone() const
    {
    	return new ObjectLiteral(type);
    }

	virtual std::wstring toString() const
    {
        return LiteralType::toString(type);
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & boost::serialization::base_object<Literal>(*this);
    	ar & type;
    }

	LiteralType::type type;

protected:
	ObjectLiteral() { }
};

struct NumericLiteral : public Literal
{
	friend class boost::serialization::access;

	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(NumericLiteral, (NumericLiteral)(Literal)(ASTNode));

	explicit NumericLiteral(bool v)	 { type = PrimitiveType::BOOL_TYPE;  value.b = v;  }
	explicit NumericLiteral(int8 v)	 { type = PrimitiveType::INT8_TYPE;  value.i8 = v;  }
	explicit NumericLiteral(int16 v) { type = PrimitiveType::INT16_TYPE; value.i16 = v; }
	explicit NumericLiteral(int32 v) { type = PrimitiveType::INT32_TYPE; value.i32 = v; }
	explicit NumericLiteral(int64 v) { type = PrimitiveType::INT64_TYPE; value.i64 = v; }

	explicit NumericLiteral(float v)  { type = PrimitiveType::FLOAT32_TYPE; value.f32 = v; }
	explicit NumericLiteral(double v) { type = PrimitiveType::FLOAT64_TYPE; value.f64 = v; }

	template<typename T>
	explicit NumericLiteral(PrimitiveType::type t, T v)
	{
        switch(t)
        {
        case PrimitiveType::type::BOOL_TYPE: value.b = (bool)v; break;
        case PrimitiveType::type::INT8_TYPE: value.i8 = (int8)v; break;
        case PrimitiveType::type::INT16_TYPE: value.i16 = (int16)v; break;
        case PrimitiveType::type::INT32_TYPE: value.i32 = (int32)v; break;
        case PrimitiveType::type::INT64_TYPE: value.i64 = (int64)v; break;
        case PrimitiveType::type::FLOAT32_TYPE: value.f32 = (float)v; break;
        case PrimitiveType::type::FLOAT64_TYPE: value.f64 = (double)v; break;
        default: break;
        }
        type = t;
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE()
		COMPARE_MEMBER(type)
        switch(type)
        {
        case PrimitiveType::type::BOOL_TYPE    : COMPARE_MEMBER(value.b  ); break;
        case PrimitiveType::type::INT8_TYPE    : COMPARE_MEMBER(value.i8 ); break;
        case PrimitiveType::type::INT16_TYPE   : COMPARE_MEMBER(value.i16); break;
        case PrimitiveType::type::INT32_TYPE   : COMPARE_MEMBER(value.i32); break;
        case PrimitiveType::type::INT64_TYPE   : COMPARE_MEMBER(value.i64); break;
        case PrimitiveType::type::FLOAT32_TYPE : COMPARE_MEMBER(value.f32); break;
        case PrimitiveType::type::FLOAT64_TYPE : COMPARE_MEMBER(value.f64); break;
        default: break;
        }
    	END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	UNUSED_ARGUMENT(from);
    	UNUSED_ARGUMENT(to);
    	UNUSED_ARGUMENT(update_parent);

    	return false;
    }

    virtual ASTNode* clone() const
    {
        switch(type)
        {
        case PrimitiveType::type::BOOL_TYPE    : return new NumericLiteral(value.b);
        case PrimitiveType::type::INT8_TYPE    : return new NumericLiteral(value.i8);
        case PrimitiveType::type::INT16_TYPE   : return new NumericLiteral(value.i16);
        case PrimitiveType::type::INT32_TYPE   : return new NumericLiteral(value.i32);
        case PrimitiveType::type::INT64_TYPE   : return new NumericLiteral(value.i64);
        case PrimitiveType::type::FLOAT32_TYPE : return new NumericLiteral(value.f32);
        case PrimitiveType::type::FLOAT64_TYPE : return new NumericLiteral(value.f64);
        default: break;
        }
        return NULL;
    }

	virtual std::wstring toString() const
    {
        std::wostringstream oss;
		switch(type)
		{
		case PrimitiveType::BOOL_TYPE   : oss <<        value.b  ; break;
		case PrimitiveType::INT8_TYPE   : oss << (int32)value.i8 ; break;
		case PrimitiveType::INT16_TYPE  : oss << (int32)value.i16; break;
		case PrimitiveType::INT32_TYPE  : oss <<        value.i32; break;
		case PrimitiveType::INT64_TYPE  : oss <<        value.i64; break;
		case PrimitiveType::FLOAT32_TYPE: oss <<        value.f32; break;
		case PrimitiveType::FLOAT64_TYPE: oss <<        value.f64; break;
		default                    :                          break;
		}
        return oss.str();
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & boost::serialization::base_object<Literal>(*this);
    	ar & (int&)type;
    	switch(type)
    	{
        case PrimitiveType::type::BOOL_TYPE    : ar & value.b; break;
        case PrimitiveType::type::INT8_TYPE    : ar & value.i8; break;
        case PrimitiveType::type::INT16_TYPE   : ar & value.i16; break;
        case PrimitiveType::type::INT32_TYPE   : ar & value.i32; break;
        case PrimitiveType::type::INT64_TYPE   : ar & value.i64; break;
        case PrimitiveType::type::FLOAT32_TYPE : ar & value.f32; break;
        case PrimitiveType::type::FLOAT64_TYPE : ar & value.f64; break;
        default: break;
    	}
    }

	PrimitiveType::type type;

	union ValueUnion
	{
		bool  b;
		int8  i8;
		int16 i16;
		int32 i32;
		int64 i64;

		float  f32;
		double f64;

		uint64 raw;
	} value;

protected:
	NumericLiteral() { }
};

struct StringLiteral : public Literal
{
	friend class boost::serialization::access;

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

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE()
		COMPARE_MEMBER(value)
    	END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	UNUSED_ARGUMENT(from);
    	UNUSED_ARGUMENT(to);
    	UNUSED_ARGUMENT(update_parent);

    	return false;
    }

    virtual ASTNode* clone() const
    {
    	return new StringLiteral(value);
    }

	virtual std::wstring toString() const
    {
        std::wostringstream oss;
        oss << L'"' << value << L'"';
        return oss.str();
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & boost::serialization::base_object<Literal>(*this);
    	ar & value;
    }

	std::wstring value;

protected:
	StringLiteral() { }
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_LITERAL_H_ */
