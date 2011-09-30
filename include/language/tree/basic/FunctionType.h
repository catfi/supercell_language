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

#ifndef ZILLIANS_LANGUAGE_TREE_FUNCTIONTYPE_H_
#define ZILLIANS_LANGUAGE_TREE_FUNCTIONTYPE_H_

#include "language/tree/declaration/Declaration.h"
#include "language/tree/basic/Identifier.h"

namespace zillians { namespace language { namespace tree {

// forward declaration of TypeSpecifier to avoid mutual header inclusion
struct TypeSpecifier;

struct FunctionType: public ASTNode
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(FunctionType, (FunctionType)(ASTNode));

	FunctionType() : return_type(NULL)
	{ }

	void appendTemplateParameter(Identifier* parameter);
	void appendParameterType(TypeSpecifier* type);

	void setReturnType(TypeSpecifier* type)
	{
		return_type = type;
	}

	std::wstring toString() const;

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE()
		COMPARE_MEMBER(templated_parameters)
		COMPARE_MEMBER(argument_types)
		COMPARE_MEMBER(return_type)
		END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE()
		REPLACE_USE_WITH(templated_parameters)
		REPLACE_USE_WITH(argument_types)
		REPLACE_USE_WITH(return_type)
    	END_REPLACE()
    }

	std::vector<Identifier*> templated_parameters;
	std::vector<TypeSpecifier*> argument_types;
	TypeSpecifier* return_type;
};

} } }


#endif /* ZILLIANS_LANGUAGE_TREE_FUNCTIONTYPE_H_ */
