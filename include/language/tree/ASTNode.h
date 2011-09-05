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
 * @date Aug 2, 2011 sdk - Initial version created.
 */

#ifndef ZILLIANS_LANGUAGE_TREE_ASTNODE_H_
#define ZILLIANS_LANGUAGE_TREE_ASTNODE_H_

#include "core/Prerequisite.h"
#include "core/ContextHub.h"
#include "core/Visitor.h"
#include "language/tree/GarbageCollector.h"
#include "language/logging/Logger.h"
#include <boost/preprocessor.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>

#define DEFINE_HIERARCHY_BASE() \
	static int stype() \
	{ \
		return (int)ASTNodeType::ASTNode; \
	} \
	virtual bool checkType(int type) = 0;

#define DEFINE_HIERARCHY_STATIC_IMPL(r, data, i, elem)	\
    BOOST_PP_IIF(BOOST_PP_EQUAL(i, 0), BOOST_PP_EMPTY(), ||)              \
    (boost::is_same<Base,elem>::value)

#define DEFINE_HIERARCHY_DYNAMIC_IMPL(r, data, i, elem)	\
    BOOST_PP_IIF(BOOST_PP_EQUAL(i, 0), BOOST_PP_EMPTY(), ||)              \
    (type == (int)ASTNodeType::elem)

#define DEFINE_HIERARCHY(self, hierarchy)	\
	static int stype() \
	{ \
		return (int)ASTNodeType::self; \
	} \
	virtual bool checkType(int type) \
	{ \
		return ((BOOST_PP_SEQ_FOR_EACH_I(DEFINE_HIERARCHY_DYNAMIC_IMPL, _, hierarchy))) ? true : false; \
	}

namespace zillians { namespace language { namespace tree {

/**
 * Helper template function to implement static type checking system
 *
 * @param ptr the pointer to check with
 * @return true if the given ptr is an instance of Derived type; false otherwise
 */
template<typename Derived, typename Base>
inline bool isa(Base* ptr)
{
	return ptr->checkType(Derived::stype());
}

template<typename Derived, typename Base>
inline Derived* cast(Base* ptr)
{
	if(isa<Derived, Base>(ptr))
		return static_cast<Derived*>(ptr);
	else
		return NULL;
}

/**
 * Helper template function to implement static type checking system
 *
 * @param ptr the pointer to check with
 * @return true if the given ptr is an instance of Derived type; false otherwise
 */
template<typename Derived, typename Base>
inline bool isa(shared_ptr<Base>& ptr)
{
	return ptr->checkType(Derived::template stype<Base>());
}

template<typename Derived, typename Base>
inline shared_ptr<Derived> cast(shared_ptr<Base>& ptr)
{
	if(isa<Derived, Base>(ptr))
		return static_pointer_cast<Derived>(ptr);
	else
		return shared_ptr<Derived>();
}

enum class ASTNodeType : int
{
	ASTNode,

	Annotation,
	Annotations,
	Program,
	Package,
	Import,
	Block,
	Identifier,
		SimpleIdentifier,
		NestedIdentifier,
		TemplatedIdentifier,
	Literal,
		NumericLiteral,
		StringLiteral,
		ObjectLiteral,
	TypeSpecifier,
	FunctionType,

	Declaration,
		ClassDecl,
		EnumDecl,
		InterfaceDecl,
		TypedefDecl,
		FunctionDecl,
		VariableDecl,

	Statement,
		DeclarativeStmt,
		ExpressionStmt,
		IterativeStmt,
			ForeachStmt,
			WhileStmt,
		SelectionStmt,
			IfElseStmt,
			SwitchStmt,
		BranchStmt,

	Expression,
		PrimaryExpr,
		UnaryExpr,
		BinaryExpr,
		TernaryExpr,
		MemberExpr,
		CallExpr,
		CastExpr,

	InvalidType,
};

static void printASTNodeDebugInfo()
{
	LOG4CXX_DEBUG(Logger::Compiler, L"[ASTNode Type Id Debugging Table]");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::ASTNode) << L"] = " << L"ASTNodeType::ASTNode");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::Annotation) << L"] = " << L"ASTNodeType::Annotation");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::Annotations) << L"] = " << L"ASTNodeType::Annotations");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::Program) << L"] = " << L"ASTNodeType::Program");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::Package) << L"] = " << L"ASTNodeType::Package");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::Import) << L"] = " << L"ASTNodeType::Import");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::Block) << L"] = " << L"ASTNodeType::Block");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::Identifier) << L"] = " << L"ASTNodeType::Identifier");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::SimpleIdentifier) << L"] = " << L"ASTNodeType::SimpleIdentifier");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::NestedIdentifier) << L"] = " << L"ASTNodeType::NestedIdentifier");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::TemplatedIdentifier) << L"] = " << L"ASTNodeType::TemplatedIdentifier");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::Literal) << L"] = " << L"ASTNodeType::Literal");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::NumericLiteral) << L"] = " << L"ASTNodeType::NumericLiteral");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::StringLiteral) << L"] = " << L"ASTNodeType::StringLiteral");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::ObjectLiteral) << L"] = " << L"ASTNodeType::ObjectLiteral");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::TypeSpecifier) << L"] = " << L"ASTNodeType::TypeSpecifier");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::FunctionType) << L"] = " << L"ASTNodeType::FunctionType");

	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::Declaration) << L"] = " << L"ASTNodeType::Declaration");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::ClassDecl) << L"] = " << L"ASTNodeType::ClassDecl");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::EnumDecl) << L"] = " << L"ASTNodeType::EnumDecl");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::InterfaceDecl) << L"] = " << L"ASTNodeType::InterfaceDecl");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::TypedefDecl) << L"] = " << L"ASTNodeType::TypedefDecl");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::FunctionDecl) << L"] = " << L"ASTNodeType::FunctionDecl");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::VariableDecl) << L"] = " << L"ASTNodeType::VariableDecl");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::Statement) << L"] = " << L"ASTNodeType::Statement");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::DeclarativeStmt) << L"] = " << L"ASTNodeType::DeclarativeStmt");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::ExpressionStmt) << L"] = " << L"ASTNodeType::ExpressionStmt");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::IterativeStmt) << L"] = " << L"ASTNodeType::IterativeStmt");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::ForeachStmt) << L"] = " << L"ASTNodeType::ForeachStmt");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::WhileStmt) << L"] = " << L"ASTNodeType::WhileStmt");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::SelectionStmt) << L"] = " << L"ASTNodeType::SelectionStmt");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::IfElseStmt) << L"] = " << L"ASTNodeType::IfElseStmt");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::SwitchStmt) << L"] = " << L"ASTNodeType::SwitchStmt");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::BranchStmt) << L"] = " << L"ASTNodeType::BranchStmt");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::Expression) << L"] = " << L"ASTNodeType::Expression");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::PrimaryExpr) << L"] = " << L"ASTNodeType::PrimaryExpr");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::UnaryExpr) << L"] = " << L"ASTNodeType::UnaryExpr");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::BinaryExpr) << L"] = " << L"ASTNodeType::BinaryExpr");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::TernaryExpr) << L"] = " << L"ASTNodeType::TernaryExpr");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::MemberExpr) << L"] = " << L"ASTNodeType::MemberExpr");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::CallExpr) << L"] = " << L"ASTNodeType::CallExpr");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::CastExpr) << L"] = " << L"ASTNodeType::CastExpr");
	LOG4CXX_DEBUG(Logger::Compiler, L"[#" << (int)(ASTNodeType::InvalidType) << L"] = " << L"ASTNodeType::InvalidType");
}

struct ASTNode : public VisitableBase<ASTNode>, ContextHub<ContextOwnership::transfer>
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY_BASE();

	ASTNode() : parent(NULL)
	{
		GarbageCollector<const ASTNode>::instance()->add(this);
	}

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version) {
    	// TODO: serialize ContextHub.
    	//
    	// Currently the ContextHub can not be serialized because of the lackness of type information
    	// ContextHub uses a special trick to map type to a integer in, and uses std::vector<shared_ptr<void>> to store data.
    	// Survey boost::any and modify the implementation to make it possible to be serialized.
        boost::serialization::base_object<ContextHub<ContextOwnership::transfer>>(*this);
        ar & parent;
    }

	ASTNode* parent;
};

typedef GarbageCollector<const ASTNode> ASTNodeGC;

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_ASTNODE_H_ */

