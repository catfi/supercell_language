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
#include "language/logging/LoggerWrapper.h"
#include <boost/preprocessor.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/mpl/contains.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/or.hpp>
#include <boost/mpl/not.hpp>
#include <boost/type_traits.hpp>

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

// forward declaration of ASTNode
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

typedef __gnu_cxx::hash_set<const ASTNode*> ASTNodeSet;

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

// forward declarations of all ASTNode implementations
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
struct Selection;
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

struct ASTNode : public VisitableBase<ASTNode>, ContextHub<ContextOwnership::transfer>
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY_BASE();
public:

	/**
	 * @brief Compare if two AST tree are equal.
	 * @param rhs another AST to be compared with.
	 * @return @p true if two AST are equal in terms of their value, @p false if not.
	 */
	bool isEqual(const ASTNode& rhs) const {
		ASTNodeSet visited;
		return isEqualImpl(rhs, visited);
	}

	virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const = 0 ;

protected:
	ASTNode() : parent(NULL)
	{
		GarbageCollector<const ASTNode>::instance()->add(this);
	}

public:
	ASTNode* parent;
};

// some internal helper function/templates
namespace internal {

template<typename T>
struct is_std_vector : boost::mpl::false_
{ };

template<typename _Tp, typename _Alloc>
struct is_std_vector<std::vector<_Tp, _Alloc>> : boost::mpl::true_
{ };

template<typename T>
struct is_boost_tuple : boost::mpl::false_
{ };

template <class T0, class T1, class T2, class T3, class T4,
          class T5, class T6, class T7, class T8, class T9>
struct is_boost_tuple<boost::tuple<T0,T1,T2,T3,T4,T5,T6,T7,T8,T9>> : boost::mpl::true_
{ };

template<typename T>
struct is_std_pair : boost::mpl::false_
{ };

template<typename T0, typename T1>
struct is_std_pair<std::pair<T0, T1>> : boost::mpl::true_
{ };

typedef boost::mpl::vector<ASTNode, Annotation, Annotations, Program, Package,
		Import, Block, Identifier, SimpleIdentifier, NestedIdentifier,
		TemplatedIdentifier, Literal, NumericLiteral, StringLiteral,
		ObjectLiteral, TypeSpecifier, FunctionType, Declaration, ClassDecl,
		EnumDecl, InterfaceDecl, TypedefDecl, FunctionDecl, VariableDecl,
		Statement, DeclarativeStmt, ExpressionStmt, IterativeStmt, ForStmt,
		ForeachStmt, WhileStmt, Selection, SelectionStmt, IfElseStmt,
		SwitchStmt, BranchStmt, Expression, PrimaryExpr, UnaryExpr, BinaryExpr,
		TernaryExpr, MemberExpr, CallExpr, CastExpr> ASTNodeTypeVector;

typedef boost::mpl::vector<ASTNode, Identifier, Identifier, Declaration,
		Statement, IterativeStmt, SelectionStmt, Expression> ASTNodeInternalTypeVector;


template<typename T>
inline bool compareDispatch(T& a, T& b, ASTNodeSet& visited)
{
	typedef boost::is_pointer<T> is_pointer_t;
	typedef boost::mpl::contains<internal::ASTNodeTypeVector, typename boost::remove_const<T>::type> is_ast_t;
	typedef internal::is_std_pair<typename boost::remove_const<T>::type> is_std_pair_t;
	typedef internal::is_std_vector<typename boost::remove_const<T>::type> is_std_vector_t;
	typedef internal::is_boost_tuple<typename boost::remove_const<T>::type> is_boost_tuple_t;

	typedef boost::mpl::not_<
			boost::mpl::or_<
				is_pointer_t,
				is_ast_t,
				is_std_pair_t,
				is_std_vector_t,
				is_boost_tuple_t>> is_other_t;

	return compareDispatchImpl(a, b, visited, is_pointer_t(), is_ast_t(), is_std_pair_t(), is_std_vector_t(), is_boost_tuple_t(), is_other_t());
};

template<typename T>
inline bool compareDispatchImpl(
		T& a, T& b, ASTNodeSet& visited,
		boost::mpl::true_ /*is_pointer*/,
		boost::mpl::false_ /*is_ast*/,
		boost::mpl::false_ /*is_pair*/,
		boost::mpl::false_ /*is_vector*/,
		boost::mpl::false_ /*is_tuple*/,
		boost::mpl::false_ /*is_other*/)
{
	if(a == NULL && b == NULL)
		return true;
	else if((a == NULL && b != NULL) || (a != NULL && b == NULL))
		return false;
	else
		return compareDispatch(*a, *b, visited);
}

template<typename T>
inline bool compareDispatchImpl(
		T& a, T& b, ASTNodeSet& visited,
		boost::mpl::false_ /*is_pointer*/,
		boost::mpl::true_ /*is_ast*/,
		boost::mpl::false_ /*is_pair*/,
		boost::mpl::false_ /*is_vector*/,
		boost::mpl::false_ /*is_tuple*/,
		boost::mpl::false_ /*is_other*/)
{
	return a.isEqualImpl(b, visited);
}

template<typename T>
inline bool compareDispatchImpl(
		T& a, T& b, ASTNodeSet& visited,
		boost::mpl::false_ /*is_pointer*/,
		boost::mpl::false_ /*is_ast*/,
		boost::mpl::true_ /*is_pair*/,
		boost::mpl::false_ /*is_vector*/,
		boost::mpl::false_ /*is_tuple*/,
		boost::mpl::false_ /*is_other*/)
{
	if(compareDispatch(a.first, b.first, visited))
	{
		return compareDispatch(a.second, b.second, visited);
	}
	else
	{
		return false;
	}
}

template<typename T>
inline bool compareDispatchImpl(
		T& a, T& b, ASTNodeSet& visited,
		boost::mpl::false_ /*is_pointer*/,
		boost::mpl::false_ /*is_ast*/,
		boost::mpl::false_ /*is_pair*/,
		boost::mpl::true_ /*is_vector*/,
		boost::mpl::false_ /*is_tuple*/,
		boost::mpl::false_ /*is_other*/)
{
	if(a.size() != b.size())
		return false;

	auto iterator_a = a.begin(); auto iterator_end_a = a.end();
	auto iterator_b = b.begin(); auto iterator_end_b = b.end();

	while(iterator_a != iterator_end_a && iterator_b != iterator_end_b)
	{
		bool result = compareDispatch(*iterator_a, *iterator_b, visited);
		if(!result) return false;

		++iterator_a; ++iterator_b;
	}

	return true;
}

template<typename T>
inline bool compareDispatchImpl(
		T& a, T& b, ASTNodeSet& visited,
		boost::mpl::false_ /*is_pointer*/,
		boost::mpl::false_ /*is_ast*/,
		boost::mpl::false_ /*is_pair*/,
		boost::mpl::false_ /*is_vector*/,
		boost::mpl::true_ /*is_tuple*/,
		boost::mpl::false_ /*is_other*/)
{
	// TODO implement boost tuple traversal
	return true;
}

template<typename T>
inline bool compareDispatchImpl(
		T& a, T& b, ASTNodeSet& visited,
		boost::mpl::false_ /*is_pointer*/,
		boost::mpl::false_ /*is_ast*/,
		boost::mpl::false_ /*is_pair*/,
		boost::mpl::false_ /*is_vector*/,
		boost::mpl::false_ /*is_tuple*/,
		boost::mpl::true_ /*is_other*/)
{
	return (a == b);
}

} // internal

#define BEGIN_COMPARE() \
		typedef typename boost::remove_const<typename boost::remove_reference<decltype(*this)>::type>::type self_type; \
		if(!boost::mpl::contains<internal::ASTNodeInternalTypeVector, self_type>::value) \
			if(visited.count(this)) \
				return true; \
			else \
				visited.insert(this); \
		const self_type* p = cast<const self_type>(&rhs); \
		if(p == NULL) \
			return false;

#define BEGIN_COMPARE_WITH_BASE(base_class_name) \
		typedef typename boost::remove_const<typename boost::remove_reference<decltype(*this)>::type>::type self_type; \
		const self_type* p = cast<const self_type>(&rhs); \
		if(p == NULL) \
			return false; \
		if(!base_class_name::isEqualImpl(*p, visited)) \
			return false;

#define COMPARE_MEMBER(member) \
		if(!internal::compareDispatch(member, p->member, visited)) return false;

#define END_COMPARE()	\
		return true;

typedef GarbageCollector<const ASTNode> ASTNodeGC;

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_ASTNODE_H_ */

