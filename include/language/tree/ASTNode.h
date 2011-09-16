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
#include "language/logging/LoggingManager.h"
#include <boost/preprocessor.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/mpl/contains.hpp>

#define DEFINE_HIERARCHY_BASE() \
	static int stype() \
	{ \
		return (int)ASTNodeType::ASTNode; \
	} \
	virtual bool checkType(int type) const = 0;

#define DEFINE_HIERARCHY_STATIC_IMPL(r, data, i, elem)	\
	BOOST_PP_IIF(BOOST_PP_EQUAL(i, 0), BOOST_PP_EMPTY(), ||)			  \
	(boost::is_same<Base,elem>::value)

#define DEFINE_HIERARCHY_DYNAMIC_IMPL(r, data, i, elem)	\
	BOOST_PP_IIF(BOOST_PP_EQUAL(i, 0), BOOST_PP_EMPTY(), ||)			  \
	(type == (int)ASTNodeType::elem)

#define DEFINE_HIERARCHY(self, hierarchy)	\
	static int stype() \
	{ \
		return (int)ASTNodeType::self; \
	} \
	virtual bool checkType(int type) const\
	{ \
		return ((BOOST_PP_SEQ_FOR_EACH_I(DEFINE_HIERARCHY_DYNAMIC_IMPL, _, hierarchy))) ? true : false; \
	}

namespace zillians { namespace language { namespace tree {

class ASTNode;

} } }

namespace __gnu_cxx {
	template<>
	struct hash<const zillians::language::tree::ASTNode*>
	{
		size_t operator() (const zillians::language::tree::ASTNode* p) const
		{
			return reinterpret_cast<size_t>(p);
		}
	} ;
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
			ForStmt,
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
};

// forward declarations
struct ASTNode;
struct Annotation;
struct Annotations;
struct Program;
struct Package;
struct Import;
struct Block;
struct Identifier;
struct SimpleIdentifier;
struct NestedIdentifier;
struct TemplatedIdentifier;
struct Literal;
struct NumericLiteral;
struct StringLiteral;
struct ObjectLiteral;
struct TypeSpecifier;
struct FunctionType;
struct Declaration;
struct ClassDecl;
struct EnumDecl;
struct InterfaceDecl;
struct TypedefDecl;
struct FunctionDecl;
struct VariableDecl;
struct Statement;
struct DeclarativeStmt;
struct ExpressionStmt;
struct IterativeStmt;
struct ForStmt;
struct ForeachStmt;
struct WhileStmt;
struct SelectionStmt;
struct IfElseStmt;
struct SwitchStmt;
struct BranchStmt;
struct Expression;
struct PrimaryExpr;
struct UnaryExpr;
struct BinaryExpr;
struct TernaryExpr;
struct MemberExpr;
struct CallExpr;
struct CastExpr;

// boost mpl vector
typedef boost::mpl::vector<
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
	ForStmt,
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
	CastExpr
> ASTNodeMPLVector;

struct ASTNode : public VisitableBase<ASTNode>, ContextHub<ContextOwnership::keep>
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY_BASE();

public:
	typedef __gnu_cxx::hash_set<const ASTNode*> ASTNodeSet;

	/**
	 * @brief Compare is AST equal.
	 * @param rhs Another AST to be compared with.
	 * @return @p true if two AST are value-sementically equal, @p false if not.
	 */
	bool isEqual(const ASTNode& rhs) const {
		ASTNodeSet visited;
		return isEqualImpl(rhs, visited);
	}

	virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const = 0 ;

	// TODO: Unify the two macro.
	// Currently I have some problem of resolve the ASTNode* with other normal type(Selection, int, enums...)
	// It might can be resolved with the boost::enable_if, but I have not done it yet.

	#define COMPARE_ASTNODE_MEMBER(member) if (!(isMemberEqual(member, p->member, visited))) { return false; }
	#define COMPARE_MEMBER(member) if (member != p->member) { return false; }

	/**
	 * @brief Compare is two ASTNode* is equal value-semantically.
	 * @return @p true if two ASTNode* are value-sementically equal, @p false if not.
	 *
	 * If both pointers are @p NULL, return @p true.
	 * If one is @p NULL, another is not, return @p false.
	 * if both are not @p NULL, return the value-semantically compare result.
	 */
	static bool isASTNodePointerEqual(const ASTNode* lhs, const ASTNode* rhs, ASTNodeSet& visited)
	{
		if(lhs == NULL && rhs == NULL)
		{
			return true;
		}
		if(lhs == NULL)
		{
			return false;
		}
		if(rhs == NULL)
		{
			return false;
		}
		return lhs->isEqualImpl(*rhs, visited);
	}

	template<typename T>
	static bool isMemberEqual(const T* const lhs, const T* const rhs, ASTNodeSet& visited)
	{
		return isASTNodePointerEqual(lhs, rhs, visited);
	}

	template<typename T>
	static bool isMemberEqual(const std::vector<T>& lhs, const std::vector<T>& rhs, ASTNodeSet& visited)
	{
		if(lhs.size() != rhs.size())
		{
			return false;
		}
		for(size_t i = 0; i < lhs.size(); ++i)
		{
			if(!isASTNodePointerEqual(lhs[i], rhs[i], visited))
			{
				return false;
			}
		}
		return true ;
	}

	template<typename First, typename Second>
	static bool isMemberEqual(const std::vector<std::pair<First, Second>>& lhs, const std::vector<std::pair<First, Second>>& rhs, ASTNodeSet& visited)
	{
		if(lhs.size() != rhs.size())
		{
			return false;
		}
		for(size_t i = 0; i < lhs.size(); ++i)
		{
			if(!isASTNodePointerEqual(lhs[i].first, rhs[i].first, visited))
			{
				return false;
			}
			if(!isASTNodePointerEqual(lhs[i].second, rhs[i].second, visited))
			{
				return false;
			}
		}
		return true ;
	}

	/**
	 * @brief Compare is two std::vector<boost::tuple<ASTNode*, ASTNode*, ASTNode*>> data member equal.
	 * @param dataMember The data member to be compared.
	 * @param lhs Left hand side object to get the @p pairVec.
	 * @param rhs Right hand side object to get the @p pairVec.
	 * @param visited Recording the compared ASTNode, prevent infinite resursive compare.
	 * @return @p true if two vectors are value-sementically equal, @p false if not.
	 * @todo Enhance the function to compare variadic tuple. Currently it can only work with tuple length is 3.
	 */
	template<typename TupleVectorMember, typename T>
	static bool isTupleVectorMemberEqual(TupleVectorMember pairVec, const T& lhs, const T& rhs, ASTNodeSet& visited)
	{
		auto& leftVec = lhs.*pairVec ;
		auto& rightVec = rhs.*pairVec ;
		if(leftVec.size() != rightVec.size())
		{
			return false;
		}

		for(size_t i = 0; i < leftVec.size(); ++i)
		{
			if(!isASTNodePointerEqual(leftVec[i].get<0>(), rightVec[i].get<0>(), visited))
			{
				return false;
			}
			if(!isASTNodePointerEqual(leftVec[i].get<1>(), rightVec[i].get<1>(), visited))
			{
				return false;
			}
			if(!isASTNodePointerEqual(leftVec[i].get<2>(), rightVec[i].get<2>(), visited))
			{
				return false;
			}
		}

		return true ;
	}

protected:
	ASTNode() : parent(NULL)
	{
		GarbageCollector<const ASTNode>::instance()->add(this);
	}

public:
	ASTNode* parent;
};

typedef GarbageCollector<const ASTNode> ASTNodeGC;

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_ASTNODE_H_ */

