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

#ifndef ZILLIANS_LANGUAGE_TREE_INTERFACEDECL_H_
#define ZILLIANS_LANGUAGE_TREE_INTERFACEDECL_H_

#include "language/tree/declaration/Declaration.h"
#include "language/tree/declaration/FunctionDecl.h"

namespace zillians { namespace language { namespace tree {

struct InterfaceDecl : public Declaration
{
	friend class boost::serialization::access;

	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(InterfaceDecl, (InterfaceDecl)(Declaration)(ASTNode));

	explicit InterfaceDecl(Identifier* name) : Declaration(name)
	{
		BOOST_ASSERT(name && "null interface name is not allowed");
	}

	void addExtendInterface(TypeSpecifier* interface_)
	{
		BOOST_ASSERT(interface_ && "null interface is not allowed");

		interface_->parent = this;
		extend_interfaces.push_back(interface_);
	}

	void addFunction(FunctionDecl* func)
	{
		func->parent = this;
		member_functions.push_back(func);
	}

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const
    {
    	BEGIN_COMPARE_WITH_BASE(Declaration)
		COMPARE_MEMBER(member_functions)
		END_COMPARE()
    }

    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true)
    {
    	BEGIN_REPLACE_WITH_BASE(Declaration)
		REPLACE_USE_WITH(member_functions)
    	END_REPLACE()
    }

    virtual ASTNode* clone() const
    {
    	InterfaceDecl* cloned = new InterfaceDecl((name) ? cast<Identifier>(name->clone()) : NULL);

        if(annotations != NULL)
        {
            Annotations* anno = cast<Annotations>(annotations->clone());
            cloned->setAnnotations(anno);
        }

    	foreach(i, member_functions)
    		cloned->addFunction((*i) ? cast<FunctionDecl>((*i)->clone()) : NULL);

    	return cloned;
    }

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & boost::serialization::base_object<Declaration>(*this);
    	ar & member_functions;
    }

	std::vector<FunctionDecl*> member_functions;
    std::vector<TypeSpecifier*> extend_interfaces;

protected:
	InterfaceDecl() { }
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_INTERFACEDECL_H_ */
