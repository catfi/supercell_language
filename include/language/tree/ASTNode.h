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
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>

#define DEFINE_HIERARCHY_BASE() \
	static int stype() \
	{ \
		return (int)ASTNodeType::ASTNode; \
	} \
	virtual bool checkType(int type) const = 0;

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
	virtual bool checkType(int type) const\
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

struct ASTNode : public VisitableBase<ASTNode>, ContextHub<ContextOwnership::transfer>
{
	DEFINE_VISITABLE();
	DEFINE_HIERARCHY_BASE();

protected:
	ASTNode() : parent(NULL)
	{
		GarbageCollector<const ASTNode>::instance()->add(this);
	}

public:
    typedef __gnu_cxx::hash_set<const ASTNode*> ASTNodeSet;

    /**
     * @brief Compare is two ASTNode data member equal.
     * @param dataMember The data member to be compared.
     * @param lhs Left hand side object to get the @p dataMember.
     * @param rhs Right hand side object to get the @p dataMember.
     * @param visited Recording the compared ASTNode, prevent infinite resursive compare.
     */
    template <typename ASTMember, typename T>
    bool isASTNodeMemberEqual(ASTMember dataMember, const T& lhs, const T& rhs, ASTNodeSet& visited) const
    {
        if (lhs.*dataMember == NULL && rhs.*dataMember == NULL) return true;
        if (lhs.*dataMember == NULL) return false;
        if (rhs.*dataMember == NULL) return false;
        return (lhs.*dataMember)->isEqual(*(rhs.*dataMember), visited);
    }

    /**
     * @brief Compare is two std::vector<ASTNode*> data member equal.
     * @param dataMember The data member to be compared.
     * @param lhs Left hand side object to get the @p vec.
     * @param rhs Right hand side object to get the @p vec.
     * @param visited Recording the compared ASTNode, prevent infinite resursive compare.
     */
    template <typename VectorMember, typename T>
    bool isVectorMemberEqual(VectorMember vec, const T& lhs, const T& rhs, ASTNodeSet& visited) const
    {
        auto& leftVec = lhs.*vec ;
        auto& rightVec = rhs.*vec ;
        if (leftVec.size() != rightVec.size()) {
            return false;
        }
        for (size_t i = 0 ; i < leftVec.size() ; ++i) {
            if (leftVec[i] == NULL && rightVec[i] == NULL) continue ;
            if (leftVec[i] == NULL) return false ;
            if (rightVec[i] == NULL) return false ;
            if (!leftVec[i]->isEqual(*rightVec[i], visited)) {
                return false;
            }
        }
        return true ;
    }

    /**
     * @brief Compare is two std::vector<std::pair<ASTNode*, ASTNode*>> data member equal.
     * @param dataMember The data member to be compared.
     * @param lhs Left hand side object to get the @p pairVec.
     * @param rhs Right hand side object to get the @p pairVec.
     * @param visited Recording the compared ASTNode, prevent infinite resursive compare.
     */
    template <typename PairVectorMember, typename T>
    bool isPairVectorMemberEqual(PairVectorMember pairVec, const T& lhs, const T& rhs, ASTNodeSet& visited) const
    {
        auto& leftVec = lhs.*pairVec ;
        auto& rightVec = rhs.*pairVec ;
        if (leftVec.size() != rightVec.size()) {
            return false;
        }
        for (size_t i = 0 ; i < leftVec.size() ; ++i) {
            if (!leftVec[i].first ->isEqual(*rightVec[i].first , visited)) return false;
            if (!leftVec[i].second->isEqual(*rightVec[i].second, visited)) return false;
        }
        return true ;
    }

    virtual bool isEqual(const ASTNode& rhs, ASTNodeSet& visited) const = 0 ;

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
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

namespace __gnu_cxx {
    template<> struct hash<const zillians::language::tree::ASTNode*>
    {
        size_t operator() (const zillians::language::tree::ASTNode* p)
        {
            return reinterpret_cast<size_t>(p);
        }
    } ;
}

#endif /* ZILLIANS_LANGUAGE_TREE_ASTNODE_H_ */

