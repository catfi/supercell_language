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

#ifndef ZILLIANS_LANGUAGE_TREE_TYPENAMEDECL_H_
#define ZILLIANS_LANGUAGE_TREE_TYPENAMEDECL_H_

#include "language/tree/declaration/Declaration.h"
#include "language/tree/basic/Identifier.h"
#include "language/tree/basic/TypeSpecifier.h"
#include "language/tree/expression/Expression.h"

namespace zillians { namespace language { namespace tree {

struct TypenameDecl : public Declaration
{
	friend class boost::serialization::access;

	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(TypenameDecl, (TypenameDecl)(Declaration)(ASTNode));

	explicit TypenameDecl(Identifier* name, TypeSpecifier* specialized_type = NULL, Expression* default_type = NULL) : Declaration(name), specialized_type(specialized_type), default_type(default_type)
	{
		BOOST_ASSERT(name && "null variable name is not allowed");

		if(specialized_type) specialized_type->parent = this;
		if(default_type) default_type->parent = this;
	}

	void setSpecializdType(TypeSpecifier* s)
	{
		if(specialized_type) specialized_type->parent = NULL;
		if(s) s->parent = this;
		specialized_type = s;
	}

	void setDefaultType(Expression* d)
	{
		if(default_type) default_type->parent = NULL;
		if(d) d->parent = this;
		default_type = d;
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE_WITH_BASE(Declaration)
		COMPARE_MEMBER(specialized_type)
		COMPARE_MEMBER(default_type)
		END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE_WITH_BASE(Declaration)
		REPLACE_USE_WITH(specialized_type)
		REPLACE_USE_WITH(default_type)
    	END_REPLACE()
    }

    virtual ASTNode* clone() const
    {
    	return new TypenameDecl(
    			(name) ? cast<Identifier>(name->clone()) : NULL,
    			(specialized_type) ? cast<TypeSpecifier>(specialized_type->clone()) : NULL,
    			(default_type) ? cast<Expression>(default_type->clone()) : NULL);
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & boost::serialization::base_object<Declaration>(*this);
    	ar & specialized_type;
    	ar & default_type;
    }

	TypeSpecifier* specialized_type;
	Expression* default_type;

protected:
	TypenameDecl() { }
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_TYPENAMEDECL_H_ */
