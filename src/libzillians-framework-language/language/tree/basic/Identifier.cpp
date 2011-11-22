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

#include "language/tree/basic/Identifier.h"
#include "language/tree/basic/TypeSpecifier.h"

namespace zillians { namespace language { namespace tree {

SimpleIdentifier::SimpleIdentifier()
{ }

SimpleIdentifier::SimpleIdentifier(const std::wstring& s) : name(s)
{ }

std::wstring SimpleIdentifier::toString() const
{
	return name;
}

bool SimpleIdentifier::isEmpty() const
{
	return (name.length() == 0);
}

bool SimpleIdentifier::isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
{
	BEGIN_COMPARE()
	COMPARE_MEMBER(name)
	END_COMPARE()
}

bool SimpleIdentifier::replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
{
	BEGIN_REPLACE()
	REPLACE_USE_WITH(name)
	END_REPLACE()
}

ASTNode* SimpleIdentifier::clone() const
{
	return new SimpleIdentifier(name);
}




NestedIdentifier::NestedIdentifier()
{ }

std::wstring NestedIdentifier::toString() const
{
	std::wstring t;

	foreach(i, identifier_list)
	{
		t += (*i)->toString();
		if(!is_end_of_foreach(i, identifier_list))
			t += L".";
	}

	return t;
}

bool NestedIdentifier::isEmpty() const
{
	foreach(i, identifier_list)
	{
		if(!(*i)->isEmpty())
			return false;
	}

	return true;
}

void NestedIdentifier::appendIdentifier(Identifier* id)
{
	id->parent = this;
	identifier_list.push_back(id);
}

bool NestedIdentifier::isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
{
	BEGIN_COMPARE()
	COMPARE_MEMBER(identifier_list)
	END_COMPARE()
}

bool NestedIdentifier::replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
{
	BEGIN_REPLACE()
	REPLACE_USE_WITH(identifier_list)
	END_REPLACE()
}

ASTNode* NestedIdentifier::clone() const
{
	NestedIdentifier* cloned = new NestedIdentifier();
	foreach(i, identifier_list) cloned->appendIdentifier(cast<Identifier>((*i)->clone()));
	return cloned;
}



TemplateType::TemplateType(SimpleIdentifier* id, TypeSpecifier* specialized_type = NULL, ASTNode* default_type = NULL) : id(id), specialized_type(specialized_type), default_type(default_type)
{
	BOOST_ASSERT(id != NULL);
}

TemplateType& TemplateType::operator= (const TemplateType& s)
{
	id = s.id;
	specialized_type = s.specialized_type;
	default_type = s.default_type;
	return *this;
}

bool TemplateType::isEqualImpl(const TemplateType& rhs, ASTNodeSet& visited) const
{
	const TemplateType* __p = &rhs;
	COMPARE_MEMBER(id)
	COMPARE_MEMBER(specialized_type)
	COMPARE_MEMBER(default_type)
	return true;
}

bool TemplateType::replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
{
	BEGIN_REPLACE()
	REPLACE_USE_WITH(id)
	REPLACE_USE_WITH(specialized_type)
	REPLACE_USE_WITH(default_type)
	END_REPLACE()
}

std::wstring TemplateType::toString() const
{
	std::wstring t;

	t += id->toString();
	if(specialized_type)
	{
		t += L":";
		t += specialized_type->toString();
	}

	return t;
}

TemplateType TemplateType::clone() const
{
	TemplateType s((id) ? cast<SimpleIdentifier>(id->clone()) : NULL, (specialized_type) ? cast<TypeSpecifier>(specialized_type->clone()) : NULL, (default_type) ? default_type->clone() : NULL);
	return s;
}



TemplatedIdentifier::TemplatedIdentifier()
{ }

TemplatedIdentifier::TemplatedIdentifier(Usage::type type, Identifier* id) : type(type), id(id)
{
	id->parent = this;
}

std::wstring TemplatedIdentifier::toString() const
{
	std::wstring t;

	if(type == Usage::FORMAL_PARAMETER)
	{
		t += id->toString();
		t += L"<";
		foreach(i, templated_type_list)
		{
			t += i->toString();
			if(!is_end_of_foreach(i, templated_type_list))
				t += L",";
		}
		t += L">";
	}
	else
	{
		// TODO how to dump type specifier without having its header?
	}

	return t;
}

bool TemplatedIdentifier::isEmpty() const
{
	if(id->isEmpty() && templated_type_list.size() == 0)
		return true;
	else
		return false;
}

void TemplatedIdentifier::setIdentifier(Identifier* identifier)
{
	if(id) id->parent = NULL;
	id = identifier;
	id->parent = this;
}

void TemplatedIdentifier::append(TemplateType type)
{
	type.id->parent = this;
	if(type.specialized_type) type.specialized_type->parent = this;
	if(type.default_type) type.default_type = this;

	templated_type_list.push_back(type);
}

bool TemplatedIdentifier::isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
{
	BEGIN_COMPARE()
	COMPARE_MEMBER(type)
	COMPARE_MEMBER(id)
	COMPARE_MEMBER(templated_type_list)
	END_COMPARE()
}

bool TemplatedIdentifier::replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
{
	BEGIN_REPLACE()
	REPLACE_USE_WITH(type)
	REPLACE_USE_WITH(id)
	REPLACE_USE_WITH(templated_type_list)
	END_REPLACE()
}

ASTNode* TemplatedIdentifier::clone() const
{
	TemplatedIdentifier* cloned = new TemplatedIdentifier(type, (id) ? cast<Identifier>(id->clone()) : NULL);
	foreach(i, templated_type_list) cloned->append(i->clone());
	return cloned;
}

} } }
