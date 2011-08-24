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
#include "utility/TemplateTricks.h"

namespace zillians { namespace language { namespace tree {

struct Identifier : public ASTNode
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(Identifier, (Identifier)(ASTNode));

	Identifier()
	{ }

	virtual const std::wstring& toString() const = 0;
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
			if((i+1) != endof(identifier_list))
				t += L".";
		}

		if(t.length() == 0)
			t = L"<empty>";

		return t;
	}

	void appendIdentifier(Identifier* id)
	{
		id->parent = this;
		identifier_list.push_back(id);
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
				if((i+1) != endof(templated_type_list))
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

	Usage::type type;
	Identifier* id;
	std::vector<ASTNode*> templated_type_list;
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_IDENTIFIER_H_ */
