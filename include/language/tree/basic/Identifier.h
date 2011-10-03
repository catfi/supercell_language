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

	Identifier()
	{ }

	virtual const std::wstring& toString() const = 0;
	virtual bool isEmpty() const = 0;

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
        return true;
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	return false;
    }
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

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE_WITH_BASE(Identifier)
		COMPARE_MEMBER(name)
		END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE_WITH_BASE(Identifier)
    	REPLACE_USE_WITH(name)
    	END_REPLACE()
    }

    virtual ASTNode* clone() const
    {
    	return new SimpleIdentifier(name);
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

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE_WITH_BASE(Identifier)
		COMPARE_MEMBER(identifier_list)
    	END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE_WITH_BASE(Identifier)
    	REPLACE_USE_WITH(identifier_list)
    	END_REPLACE()
    }

	virtual ASTNode* clone() const
	{
		NestedIdentifier* cloned = new NestedIdentifier();
		foreach(i, identifier_list) cloned->appendIdentifier(cast<Identifier>((*i)->clone()));
		return cloned;
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

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE_WITH_BASE(Identifier)
		COMPARE_MEMBER(type)
		COMPARE_MEMBER(id)
		COMPARE_MEMBER(templated_type_list)
		END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE_WITH_BASE(Identifier)
    	REPLACE_USE_WITH(type)
    	REPLACE_USE_WITH(id)
    	REPLACE_USE_WITH(templated_type_list)
    	END_REPLACE()
    }

	virtual ASTNode* clone() const
	{
		TemplatedIdentifier* cloned = new TemplatedIdentifier(type, (id) ? cast<Identifier>(id->clone()) : NULL);
		foreach(i, templated_type_list) cloned->templated_type_list.push_back((*i) ? (*i)->clone() : NULL);
		return cloned;
	}

	Usage::type type;
	Identifier* id;
	std::vector<ASTNode*> templated_type_list;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_IDENTIFIER_H_ */
