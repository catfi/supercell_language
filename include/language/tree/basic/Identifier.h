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

#ifndef ZILLIANS_LANGUAGE_TREE_IDENTIFIER_H_
#define ZILLIANS_LANGUAGE_TREE_IDENTIFIER_H_

#include "language/tree/ASTNode.h"
#include "utility/Foreach.h"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>

namespace zillians { namespace language { namespace tree {

struct Identifier : public ASTNode
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(Identifier, (Identifier)(ASTNode));

	Identifier()// : resolved(NULL)
	{ }

//	void resolveTo(ASTNode* node)
//	{
//		if(resolved) resolved->parent = NULL;
//		resolved = node;
//		if(resolved) resolved->parent = this;
//	}

	virtual const std::wstring& toString() const = 0;
	virtual bool isEmpty() const = 0;

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ::boost::serialization::base_object<ASTNode>(*this);
    }

//	ASTNode* resolved;
};

struct SimpleIdentifier : public Identifier
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(SimpleIdentifier, (SimpleIdentifier)(Identifier)(ASTNode));

	explicit SimpleIdentifier(const std::wstring& s) : name(s)
	{ }

	virtual const std::wstring& toString() const
	{
		static std::wstring t;
		if(name.length() == 0)
		{
			t = L"<empty>";
			return t;
		}
		else
			return name;
	}

	virtual bool isEmpty() const
	{
		return (name.length() == 0);
	}

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ::boost::serialization::base_object<Identifier>(*this);
    }

	const std::wstring name;
};

struct NestedIdentifier : public Identifier
{
	DEFINE_VISITABLE()
	DEFINE_HIERARCHY(NestedIdentifier, (NestedIdentifier)(Identifier)(ASTNode));

	NestedIdentifier()
	{ }

	virtual const std::wstring& toString() const
	{
		static std::wstring t;
		t.clear();

		foreach(i, identifier_list)
		{
			t += (*i)->toString();
			if(!is_end_of_foreach(i, identifier_list))
				t += L".";
		}

		if(t.length() == 0)
			t = L"<empty>";

		return t;
	}

	virtual bool isEmpty() const
	{
		foreach(i, identifier_list)
		{
			if(!(*i)->isEmpty())
				return false;
		}

		return true;
	}

	void appendIdentifier(Identifier* id)
	{
		id->parent = this;
		identifier_list.push_back(id);
	}

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ::boost::serialization::base_object<Identifier>(*this);
        ar & identifier_list;
    }

	std::vector<Identifier*> identifier_list;
};

struct TemplatedIdentifier : public Identifier
{
	DEFINE_VISITABLE()
	DEFINE_HIERARCHY(TemplatedIdentifier, (TemplatedIdentifier)(Identifier)(ASTNode));

	struct Usage
	{
		enum type {
			FORMAL_PARAMETER,
			ACTUAL_ARGUMENT,
		};

		static const wchar_t* toString(type t)
		{
			switch(t)
			{
			case FORMAL_PARAMETER: return L"param";
			case ACTUAL_ARGUMENT: return L"arg";
			}
		}
	};

	explicit TemplatedIdentifier(Usage::type type, Identifier* id) : type(type), id(id)
	{
		id->parent = this;
	}

	virtual const std::wstring& toString() const
	{
		static std::wstring t;
		t.clear();

		if(type == Usage::FORMAL_PARAMETER)
		{
			t += id->toString();
			t += L"<";
			foreach(i, templated_type_list)
			{
				t += cast<Identifier>((*i))->toString();
				if(!is_end_of_foreach(i, templated_type_list))
					t += L",";
			}
			t += L">";
		}
		else
		{
			// TODO how to dump type specifier without having its header?
		}

		if(t.length() == 0)
			t = L"<empty>";

		return t;
	}

	virtual bool isEmpty() const
	{
		if(id->isEmpty() && templated_type_list.size() == 0)
			return true;
		else
			return false;
	}

	void setIdentifier(Identifier* identifier)
	{
		if(id) id->parent = NULL;
		id = identifier;
		id->parent = this;
	}

	void appendParameter(ASTNode* parameter)
	{
		BOOST_ASSERT(type == Usage::FORMAL_PARAMETER);

		parameter->parent = this;
		templated_type_list.push_back(parameter);
	}

	void appendArgument(ASTNode* argument)
	{
		BOOST_ASSERT(type == Usage::ACTUAL_ARGUMENT);

		argument->parent = this;
		templated_type_list.push_back(argument);
	}

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ::boost::serialization::base_object<Identifier>(*this);
        ar & templated_type_list;
    }

	Usage::type type;
	Identifier* id;
	std::vector<ASTNode*> templated_type_list;
};

} } }

namespace boost { namespace serialization {
template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::SimpleIdentifier* p, const unsigned int file_version)
{
    ar << p->name;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::SimpleIdentifier* p, const unsigned int file_version)
{
    using namespace zillians::language::tree;

	std::wstring name;
    ar >> name;
	::new(p) SimpleIdentifier(name);
}

template<class Archive>
inline void save_construct_data(Archive& ar, const zillians::language::tree::TemplatedIdentifier* p, const unsigned int file_version)
{
    ar << (int&)p->type;
    ar << p->id;
}

template<class Archive>
inline void load_construct_data(Archive& ar, zillians::language::tree::TemplatedIdentifier* p, const unsigned int file_version)
{
    using namespace zillians::language::tree;

	int type;
	Identifier* id;
    ar >> type;
    ar >> id;

	::new(p) TemplatedIdentifier(static_cast<TemplatedIdentifier::Usage::type>(type), id);
}
}} // namespace boost::serialization

#endif /* ZILLIANS_LANGUAGE_TREE_IDENTIFIER_H_ */
