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

#include "language/tree/basic/TypeSpecifier.h"
#include "language/tree/basic/Identifier.h"
#include "language/tree/basic/FunctionType.h"

namespace zillians { namespace language { namespace tree {

TypeSpecifier::TypeSpecifier()
{ }

TypeSpecifier::TypeSpecifier(FunctionType* function_proto)
{
	update(function_proto);
}

TypeSpecifier::TypeSpecifier(PrimitiveType::type primitive)
{
	update(primitive);
}

TypeSpecifier::TypeSpecifier(Identifier* unspecified)
{
	update(unspecified);
}

std::wstring TypeSpecifier::toString() const
{
	switch(type)
	{
	case TypeSpecifier::ReferredType::FUNCTION_TYPE: return referred.function_type->toString();
	case TypeSpecifier::ReferredType::PRIMITIVE: return PrimitiveType::toString(referred.primitive);
	case TypeSpecifier::ReferredType::UNSPECIFIED: return referred.unspecified->toString();
	default: UNREACHABLE_CODE(); return L"";
	}
}

void TypeSpecifier::update(FunctionType* function_type)
{
	type = ReferredType::FUNCTION_TYPE;
	referred.function_type = function_type;
}

void TypeSpecifier::update(PrimitiveType::type primitive)
{
	type = ReferredType::PRIMITIVE;
	referred.primitive = primitive;
}

void TypeSpecifier::update(Identifier* unspecified)
{
	type = ReferredType::UNSPECIFIED;
	referred.unspecified = unspecified;
}

bool TypeSpecifier::isFunctionType() const
{
	return (type == ReferredType::FUNCTION_TYPE);
}

bool TypeSpecifier::isPrimitiveType() const
{
	return (type == ReferredType::PRIMITIVE);
}

bool TypeSpecifier::isUnspecified() const
{
	return (type == ReferredType::UNSPECIFIED);
}

bool TypeSpecifier::isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
{
	BEGIN_COMPARE()
	COMPARE_MEMBER(type)
	switch(type)
	{
	case TypeSpecifier::ReferredType::FUNCTION_TYPE  : COMPARE_MEMBER(referred.function_type )             ; break;
	case TypeSpecifier::ReferredType::PRIMITIVE      : if(referred.primitive != __p->referred.primitive) return false; break;
	case TypeSpecifier::ReferredType::UNSPECIFIED    : COMPARE_MEMBER(referred.unspecified   )             ; break;
	default: UNREACHABLE_CODE(); break;
	}
	END_COMPARE()
}

bool TypeSpecifier::replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
{
	BEGIN_REPLACE()
	switch(type)
	{
	case TypeSpecifier::ReferredType::FUNCTION_TYPE: REPLACE_USE_WITH(referred.function_type); break;
	case TypeSpecifier::ReferredType::UNSPECIFIED: REPLACE_USE_WITH(referred.unspecified); break;
	default: UNREACHABLE_CODE(); break;
	}
	END_REPLACE()
}

ASTNode* TypeSpecifier::clone() const
{
	switch(type)
	{
	case TypeSpecifier::ReferredType::FUNCTION_TYPE  : return new TypeSpecifier(cast<FunctionType>(referred.function_type->clone()));
	case TypeSpecifier::ReferredType::PRIMITIVE      : return new TypeSpecifier(referred.primitive);
	case TypeSpecifier::ReferredType::UNSPECIFIED    : return new TypeSpecifier(cast<Identifier>(referred.unspecified->clone()));
	default: UNREACHABLE_CODE(); break;
	}
	return NULL;
}

} } }
