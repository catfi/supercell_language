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
#include "language/tree/basic/Primitive.h"

namespace zillians { namespace language { namespace tree {

struct Expression;

struct Literal : public ASTNode
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(Literal, (Literal)(ASTNode));

    virtual bool isEqual(const ASTNode& rhs, ASTNodeSet& visited) const
    {
        if (visited.count(this)) return true ;
        const Literal* p = cast<const Literal>(&rhs);
        if (p == NULL) return false;
        // compare base class
        // base is ASTNode, no need to compare

        // compare data member
        // no data member

        // add this to the visited table.
        visited.insert(this);
        return true;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ::boost::serialization::base_object<ASTNode>(*this);
    }
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

    virtual bool isEqual(const ASTNode& rhs, ASTNodeSet& visited) const
    {
        if (visited.count(this)) return true ;
        const ObjectLiteral* p = cast<const ObjectLiteral>(&rhs);
        if (p == NULL) return false;
        // compare base class
        if (!Literal::isEqual(*p, visited)) return false;

        // compare data member
        if (this->type != p->type) return false;

        // add this to the visited table.
        visited.insert(this);
        return true;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ::boost::serialization::base_object<Literal>(*this);
    }

	LiteralType::type type;
};

struct NumericLiteral : public Literal
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(NumericLiteral, (NumericLiteral)(Literal)(ASTNode));

	explicit NumericLiteral(bool v)	 { type = PrimitiveType::BOOL;  value.b = v;  }
	explicit NumericLiteral(int8 v)	 { type = PrimitiveType::INT8;  value.i8 = v;  }
	explicit NumericLiteral(int16 v) { type = PrimitiveType::INT16; value.i16 = v; }
	explicit NumericLiteral(int32 v) { type = PrimitiveType::INT32; value.i32 = v; }
	explicit NumericLiteral(int64 v) { type = PrimitiveType::INT64; value.i64 = v; }

	explicit NumericLiteral(uint8 v)  { type = PrimitiveType::UINT8;  value.u8 = v;  }
	explicit NumericLiteral(uint16 v) { type = PrimitiveType::UINT16; value.u16 = v; }
	explicit NumericLiteral(uint32 v) { type = PrimitiveType::UINT32; value.u32 = v; }
	explicit NumericLiteral(uint64 v) { type = PrimitiveType::UINT64; value.u64 = v; }

	explicit NumericLiteral(float v)  { type = PrimitiveType::FLOAT32; value.f32 = v; }
	explicit NumericLiteral(double v) { type = PrimitiveType::FLOAT64; value.f64 = v; }

    virtual bool isEqual(const ASTNode& rhs, ASTNodeSet& visited) const
    {
        if (visited.count(this)) return true ;
        const NumericLiteral* p = cast<const NumericLiteral>(&rhs);
        if (p == NULL) return false;
        // compare base class
        if (!Literal::isEqual(*p, visited)) return false;

        // compare data member
        if (this->type != p->type) return false;
        switch (type) {
        case PrimitiveType::type::BOOL    : if(this->value.b   != p->value.b  ) return false; break;
        case PrimitiveType::type::UINT8   : if(this->value.i8  != p->value.i8 ) return false; break;
        case PrimitiveType::type::UINT16  : if(this->value.i16 != p->value.i16) return false; break;
        case PrimitiveType::type::UINT32  : if(this->value.i32 != p->value.i32) return false; break;
        case PrimitiveType::type::UINT64  : if(this->value.i64 != p->value.i64) return false; break;
        case PrimitiveType::type::INT8    : if(this->value.u8  != p->value.u8 ) return false; break;
        case PrimitiveType::type::INT16   : if(this->value.u16 != p->value.u16) return false; break;
        case PrimitiveType::type::INT32   : if(this->value.u32 != p->value.u32) return false; break;
        case PrimitiveType::type::INT64   : if(this->value.u64 != p->value.u64) return false; break;
        case PrimitiveType::type::FLOAT32 : if(this->value.f32 != p->value.f32) return false; break;
        case PrimitiveType::type::FLOAT64 : if(this->value.f64 != p->value.f64) return false; break;
        }

        // add this to the visited table.
        visited.insert(this);
        return true;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ::boost::serialization::base_object<Literal>(*this);
    }

	PrimitiveType::type type;

	union ValueUnion
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

    virtual bool isEqual(const ASTNode& rhs, ASTNodeSet& visited) const
    {
        if (visited.count(this)) return true ;
        const StringLiteral* p = cast<const StringLiteral>(&rhs);
        if (p == NULL) return false;
        // compare base class
        if (!Literal::isEqual(*p, visited)) return false;

        // compare data member
        if (this->value != p->value) return false;
        // add this to the visited table.
        visited.insert(this);
        return true;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ::boost::serialization::base_object<Literal>(*this);
    }

	std::wstring value;
};

} } }

namespace boost { namespace serialization {

// ObjectLiteral
template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::ObjectLiteral* p, const unsigned int file_version)
{
    ar << (int&)p->type;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::ObjectLiteral* p, const unsigned int file_version)
{
    using namespace zillians::language::tree;
    int type;
    ar >> type;
    new(p) ObjectLiteral(static_cast<ObjectLiteral::LiteralType::type>(type));
}

// NumericLiteral
template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::NumericLiteral* p, const unsigned int file_version)
{
    using namespace zillians::language::tree;

    ar << (int&)p->type;
    switch(p->type)
    {
    case PrimitiveType::type::BOOL     : ar << p->value.b  ; break;
    case PrimitiveType::type::UINT8    : ar << p->value.u8 ; break;
    case PrimitiveType::type::UINT16   : ar << p->value.u16; break;
    case PrimitiveType::type::UINT32   : ar << p->value.u32; break;
    case PrimitiveType::type::UINT64   : ar << p->value.u64; break;
    case PrimitiveType::type::INT8     : ar << p->value.i8 ; break;
    case PrimitiveType::type::INT16    : ar << p->value.i16; break;
    case PrimitiveType::type::INT32    : ar << p->value.i32; break;
    case PrimitiveType::type::INT64    : ar << p->value.i64; break;
    case PrimitiveType::type::FLOAT32  : ar << p->value.f32; break;
    case PrimitiveType::type::FLOAT64  : ar << p->value.f64; break;
    }
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::NumericLiteral* p, const unsigned int file_version)
{
    using namespace zillians::language::tree;

    int type;
    ar >> type;

    bool              b   ;
    zillians::int8    i8  ;
    zillians::int16   i16 ;
    zillians::int32   i32 ;
    zillians::int64   i64 ;
    zillians::uint8   u8  ;
    zillians::uint16  u16 ;
    zillians::uint32  u32 ;
    zillians::uint64  u64 ;
    float             f32 ;
    double            f64 ;

    switch(type)
    {
    case PrimitiveType::type::BOOL     : ar >> b  ; ::new(p) NumericLiteral(b  ); break;
    case PrimitiveType::type::UINT8    : ar >> u8 ; ::new(p) NumericLiteral(u8 ); break;
    case PrimitiveType::type::UINT16   : ar >> u16; ::new(p) NumericLiteral(u16); break;
    case PrimitiveType::type::UINT32   : ar >> u32; ::new(p) NumericLiteral(u32); break;
    case PrimitiveType::type::UINT64   : ar >> u64; ::new(p) NumericLiteral(u64); break;
    case PrimitiveType::type::INT8     : ar >> i8 ; ::new(p) NumericLiteral(i8 ); break;
    case PrimitiveType::type::INT16    : ar >> i16; ::new(p) NumericLiteral(i16); break;
    case PrimitiveType::type::INT32    : ar >> i32; ::new(p) NumericLiteral(i32); break;
    case PrimitiveType::type::INT64    : ar >> i64; ::new(p) NumericLiteral(i64); break;
    case PrimitiveType::type::FLOAT32  : ar >> f32; ::new(p) NumericLiteral(f32); break;
    case PrimitiveType::type::FLOAT64  : ar >> f64; ::new(p) NumericLiteral(f64); break;
    }
}

// StringLiteral
template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::StringLiteral* p, const unsigned int file_version)
{
    ar << p->value;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::StringLiteral* p, const unsigned int file_version)
{
    using namespace zillians::language::tree;
    std::wstring value;
    ar >> value;
    new(p) StringLiteral(value);
}

}} // namespace boost::serialization

#endif /* ZILLIANS_LANGUAGE_TREE_LITERAL_H_ */
