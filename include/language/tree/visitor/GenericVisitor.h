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
 * @date Aug 12, 2011 sdk - Initial version created.
 */

#ifndef ZILLIANS_LANGUAGE_TREE_VISITOR_GENERICVISITOR_H_
#define ZILLIANS_LANGUAGE_TREE_VISITOR_GENERICVISITOR_H_

#include "core/Prerequisite.h"
#include "core/Visitor.h"
#include "language/tree/ASTNodeFactory.h"

#define INDIRECT_REGISTER_ALL_VISITABLE_ASTNODE(impl, invoker) \
		INDIRECT_REGISTER_VISITABLE(impl, invoker, \
				/* basic */ \
				ASTNode, \
				Annotations, \
				Annotation, \
				Block, \
				Identifier, \
					SimpleIdentifier, \
					NestedIdentifier, \
					TemplatedIdentifier, \
				Literal, \
					NumericLiteral, \
					StringLiteral, \
					ObjectLiteral, \
				TypeSpecifier, \
				FunctionType, \
				/* module */ \
				Internal, \
				Tangle, \
				Source, \
				Package, \
				Import, \
				/* declaration */ \
				Declaration, \
					ClassDecl, \
					EnumDecl, \
					FunctionDecl, \
					InterfaceDecl, \
					VariableDecl, \
					TypedefDecl, \
					TypenameDecl, \
				/* statement */ \
				Statement, \
					DeclarativeStmt, \
					ExpressionStmt, \
					IterativeStmt, \
						ForStmt, \
						ForeachStmt, \
						WhileStmt, \
					SelectionStmt, \
						IfElseStmt, \
						SwitchStmt, \
					BranchStmt, \
				/* expression */ \
				Expression, \
					PrimaryExpr, \
					UnaryExpr, \
					BinaryExpr, \
					TernaryExpr, \
					MemberExpr, \
					CallExpr, \
					CastExpr \
				)

#define REGISTER_ALL_VISITABLE_ASTNODE(invoker) \
		REGISTER_VISITABLE(invoker, \
				/* basic */ \
				ASTNode, \
				Annotations, \
				Annotation, \
				Block, \
				Identifier, \
					SimpleIdentifier, \
					NestedIdentifier, \
					TemplatedIdentifier, \
				Literal, \
					NumericLiteral, \
					StringLiteral, \
					ObjectLiteral, \
				TypeSpecifier, \
				FunctionType, \
				/* module */ \
				Internal, \
				Tangle, \
				Source, \
				Package, \
				Import, \
				/* declaration */ \
				Declaration, \
					ClassDecl, \
					EnumDecl, \
					FunctionDecl, \
					InterfaceDecl, \
					VariableDecl, \
					TypedefDecl, \
					TypenameDecl, \
				/* statement */ \
				Statement, \
					DeclarativeStmt, \
					ExpressionStmt, \
					IterativeStmt, \
						ForStmt, \
						ForeachStmt, \
						WhileStmt, \
					SelectionStmt, \
						IfElseStmt, \
						SwitchStmt, \
					BranchStmt, \
				/* expression */ \
				Expression, \
					PrimaryExpr, \
					UnaryExpr, \
					BinaryExpr, \
					TernaryExpr, \
					MemberExpr, \
					CallExpr, \
					CastExpr \
				)

namespace zillians { namespace language { namespace tree { namespace visitor {

struct GenericVisitor : Visitor<ASTNode, void, VisitorImplementation::recursive_dfs>
{
	template<typename Impl, typename Invoker>
	void registerInvoker(Impl* impl)
	{
		INDIRECT_REGISTER_ALL_VISITABLE_ASTNODE(impl, Invoker);
	}

	//////////////////////////////////////////////////////////////////////
	/// Basic

	void apply(ASTNode& node)
	{
		UNUSED_ARGUMENT(node);
	}

	void apply(Annotation& node)
	{
		if(node.name) visit(*node.name);
		foreach(i, node.attribute_list)
		{
			if(i->first)  visit(*i->first);
			if(i->second) visit(*i->second);
		}
	}

	void apply(Annotations& node)
	{
		foreach(i, node.annotation_list) visit(**i);
	}

	void apply(Block& node)
	{
		foreach(i, node.objects) visit(**i);
	}

	void apply(Identifier& node)
	{
		UNUSED_ARGUMENT(node);
		UNREACHABLE_CODE();
	}

	void apply(SimpleIdentifier& node)
	{
		UNUSED_ARGUMENT(node);
	}

	void apply(NestedIdentifier& node)
	{
		foreach(i, node.identifier_list) visit(**i);
	}

	void apply(TemplatedIdentifier& node)
	{
		if(node.id) visit(*node.id);

		foreach(i, node.templated_type_list)
		{
			visit(**i);
		}
	}

	void apply(TypeSpecifier& node)
	{
		switch(node.type)
		{
		case TypeSpecifier::ReferredType::FUNCTION_TYPE: if(node.referred.function_type) visit(*node.referred.function_type); break;
		case TypeSpecifier::ReferredType::UNSPECIFIED:   if(node.referred.unspecified)   visit(*node.referred.unspecified); break;
		default: break;
		}
	}

	void apply(FunctionType& node)
	{
		foreach(i, node.templated_parameters) visit(**i);
		foreach(i, node.argument_types)       visit(**i);
	}

	//////////////////////////////////////////////////////////////////////
	/// Module
	void apply(Internal& node)
	{
		if(node.VoidTy)     visit(*node.VoidTy);
		if(node.BooleanTy)  visit(*node.BooleanTy);
		if(node.Int8Ty)     visit(*node.Int8Ty);
		if(node.Int16Ty)    visit(*node.Int16Ty);
		if(node.Int32Ty)    visit(*node.Int32Ty);
		if(node.Int64Ty)    visit(*node.Int64Ty);
		if(node.Float32Ty)  visit(*node.Float32Ty);
		if(node.Float64Ty)  visit(*node.Float64Ty);
		if(node.ObjectTy)   visit(*node.ObjectTy);
		if(node.FunctionTy) visit(*node.FunctionTy);
		if(node.StringTy)   visit(*node.StringTy);

		foreach(i, node.others) visit(**i);
	}

	void apply(Tangle& node)
	{
		if(node.internal) visit(*node.internal);

		foreach(i, node.sources)
		{
			if(i->first) visit(*(i->first));
			if(i->second) visit(*(i->second));
		}
	}

	void apply(Source& node)
	{
		foreach(i, node.imports) visit(**i);

		if(node.root) visit(*node.root);
	}

	void apply(Package& node)
	{
		if(node.annotations) visit(*node.annotations);

		if(node.id) visit(*node.id);
		foreach(i, node.children) visit(**i);
		foreach(i, node.objects)  visit(**i);
		if(node.annotations) visit(*node.annotations);
	}

	void apply(Import& node)
	{
		if(node.ns) visit(*node.ns);
	}

	//////////////////////////////////////////////////////////////////////
	/// Declaration
	void apply(Declaration& node)
	{
		UNUSED_ARGUMENT(node);
		UNREACHABLE_CODE();
	}

	void apply(ClassDecl& node)
	{
		if(node.annotations) visit(*node.annotations);
		if(node.name) visit(*node.name);

		if(node.base) visit(*node.base);
		foreach(i, node.implements)       visit(**i);
		foreach(i, node.member_functions) visit(**i);
		foreach(i, node.member_variables) visit(**i);
	}

	void apply(EnumDecl& node)
	{
		if(node.annotations) visit(*node.annotations);
		if(node.name) visit(*node.name);

		foreach(i, node.values)
		{
			visit(**i);
		}
	}

	void apply(FunctionDecl& node)
	{
		if(node.annotations) visit(*node.annotations);
		if(node.name) visit(*node.name);

		foreach(i, node.parameters)
		{
			visit(**i);
		}
		if(node.type)  visit(*node.type);
		if(node.block) visit(*node.block);
	}

	void apply(InterfaceDecl& node)
	{
		if(node.annotations) visit(*node.annotations);
		if(node.name) visit(*node.name);

		foreach(i, node.member_functions) visit(**i);
	}

	void apply(TypedefDecl& node)
	{
		if(node.annotations) visit(*node.annotations);
		if(node.name) visit(*node.name);

		if(node.type) visit(*node.type);
	}

	void apply(VariableDecl& node)
	{
		if(node.annotations) visit(*node.annotations);
		if(node.name)        visit(*node.name);

		if(node.initializer) visit(*node.initializer);
		if(node.type)        visit(*node.type);
	}

	void apply(const TypenameDecl& node)
	{
		if(node.annotations) visit(*node.annotations);
		if(node.name)        visit(*node.name);

		if(node.specialized_type) visit(*node.specialized_type);
		if(node.default_type)     visit(*node.default_type);
	}

	//////////////////////////////////////////////////////////////////////
	/// Statement
	void apply(Statement& node)
	{
		UNUSED_ARGUMENT(node);
		UNREACHABLE_CODE();
	}

	void apply(DeclarativeStmt& node)
	{
		if(node.annotations) visit(*node.annotations);

		if(node.declaration) visit(*node.declaration);
	}

	void apply(ExpressionStmt& node)
	{
		if(node.annotations) visit(*node.annotations);

		if(node.expr) visit(*node.expr);
	}

	void apply(ForeachStmt& node)
	{
		if(node.annotations) visit(*node.annotations);

		if(node.iterator) visit(*node.iterator);
		if(node.range)    visit(*node.range);
		if(node.block)    visit(*node.block);
	}

	void apply(ForStmt& node)
	{
		if(node.annotations) visit(*node.annotations);

		if(node.init)  visit(*node.init);
		if(node.cond)  visit(*node.cond);
		if(node.block) visit(*node.block);
		if(node.step)  visit(*node.step);
	}

	void apply(WhileStmt& node)
	{
		if(node.annotations) visit(*node.annotations);

		if(node.cond)  visit(*node.cond);
		if(node.block) visit(*node.block);
	}

	void apply(IfElseStmt& node)
	{
		if(node.annotations) visit(*node.annotations);

		if(node.if_branch.cond)  visit(*node.if_branch.cond);
		if(node.if_branch.block) visit(*node.if_branch.block);
		foreach(i, node.elseif_branches)
		{
			if(i->cond)  visit(*i->cond);
			if(i->block) visit(*i->block);
		}
		if(node.else_block) visit(*node.else_block);
	}

	void apply(SwitchStmt& node)
	{
		if(node.annotations) visit(*node.annotations);

		if(node.node) visit(*node.node);
		foreach(i, node.cases)
		{
			if(i->cond)  visit(*i->cond);
			if(i->block) visit(*i->block);
		}
		if(node.default_block) visit(*node.default_block);
	}

	void apply(BranchStmt& node)
	{
		if(node.annotations) visit(*node.annotations);
	}

	//////////////////////////////////////////////////////////////////////
	/// Expression
	void apply(Expression& node)
	{
		UNUSED_ARGUMENT(node);
		UNREACHABLE_CODE();
	}

	void apply(PrimaryExpr& node)
	{
		switch(node.catagory)
		{
		case PrimaryExpr::Catagory::IDENTIFIER: if(node.value.identifier) visit(*node.value.identifier); break;
		case PrimaryExpr::Catagory::LITERAL:    if(node.value.literal)    visit(*node.value.literal); break;
		case PrimaryExpr::Catagory::LAMBDA:     if(node.value.lambda)     visit(*node.value.lambda); break;
		}
	}

	void apply(UnaryExpr& node)
	{
		if(node.node) visit(*node.node);
	}

	void apply(BinaryExpr& node)
	{
		if(node.isRighAssociative())
		{
			if(node.right) visit(*node.right);
			if(node.left)  visit(*node.left);
		}
		else
		{
			if(node.left)  visit(*node.left);
			if(node.right) visit(*node.right);
		}
	}

	void apply(TernaryExpr& node)
	{
		if(node.cond)       visit(*node.cond);
		if(node.true_node)  visit(*node.true_node);
		if(node.false_node) visit(*node.false_node);
	}

	void apply(MemberExpr& node)
	{
		if(node.node)   visit(*node.node);
		if(node.member) visit(*node.member);
	}

	void apply(CallExpr& node)
	{
		if(node.node) visit(*node.node);
		foreach(i, node.parameters) visit(**i);
	}

	void apply(CastExpr& node)
	{
		if(node.node) visit(*node.node);
		if(node.type) visit(*node.type);
	}
};

} } } } // zillians::language::tree::visitor

namespace zillians { namespace language { namespace stage { namespace visitor {

//////////////////////////////////////////////////////////////////////////////
// Implementation of is_call_possible:
//////////////////////////////////////////////////////////////////////////////
// see: http://groups.google.com/group/comp.lang.c++.moderated/msg/e5fbc9305539f699 (for acutally work code)
//      http://stackoverflow.com/questions/2122319/c-type-traits-to-check-if-class-has-operator-member (for the original concept)
//      http://www.rsdn.ru/forum/cpp/2759773.1.aspx (for further discussion)
template <typename Type>
class has_member
{
    class yes { char m;};
    class no { yes m[2];};

    struct BaseMixin
    {
        void apply(){}
    };

    struct Base : public Type, public BaseMixin {};

    template <typename T, T t>  class Helper{};

    template <typename U>
        static no deduce(U*, Helper<void (BaseMixin::*)(), &U::apply>* = 0);
    static yes deduce(...);

    public:
    static const bool result = sizeof(yes) == sizeof(deduce((Base*) (0)));

};

namespace details
{
    template <typename type>
    class void_exp_result
    {};

    template <typename type, typename U>
    U const& operator,(U const&, void_exp_result<type>);

    template <typename type, typename U>
    U& operator,(U&, void_exp_result<type>);

    template <typename src_type, typename dest_type>
    struct clone_constness
    {
        typedef dest_type type;
    };

    template <typename src_type, typename dest_type>
    struct clone_constness<const src_type, dest_type>
    {
        typedef const dest_type type;
    };

}

template <typename type, typename call_details>
struct is_call_possible
{
private:
    class yes {};
    class no { yes m[2]; };

    struct derived : public type
    {
        using type::apply;
        no apply(...) const;
    };

    typedef typename details::clone_constness<type, derived>::type
        derived_type;

    template <typename T, typename due_type>
    struct return_value_check
    {
        static yes deduce(due_type);
        static no deduce(...);
        static no deduce(no);
        static no deduce(details::void_exp_result<type>);
    };

    template <typename T>
    struct return_value_check<T, void>
    {
        static yes deduce(...);
        static no deduce(no);
    };

    template <bool has, typename F>
    struct impl
    {
        static const bool value = false;
    };

    template <typename arg1, typename r>
    struct impl<true, r(arg1)>
    {
        static const bool value = sizeof( return_value_check<type, r>::deduce( (((derived_type*)0)->apply(*(arg1*)0), details::void_exp_result<type>()))) == sizeof(yes);
    };

    // specializations of impl for 2 args, 3 args,..
public:
    static const bool value = impl<has_member<type>::result, call_details>::value;

};

} } } }

#define CREATE_GENERIC_INVOKER(invoker)	\
		typedef struct { 															\
			template<typename VisitorImpl, typename Visitable>						\
			static ReturnT invoke(VisitorImpl& visitor, Visitable& visitable)		\
			{																		\
				return visitor.apply(visitable);							        \
			}																		\
		} invoker;                                                                  \
		//using GenericVisitor::apply;

#define CREATE_CONDITIONAL_INVOKER(DerivedVisitorClass, invoker) \
    typedef struct {                                             \
        template<typename VisitorImpl, typename Visitable>       \
        static ReturnT invoke(VisitorImpl& visitor, Visitable& visitable) \
        { \
            typedef typename std::conditional<zillians::language::stage::visitor::is_call_possible<DerivedVisitorClass, void(Visitable)>::value, DerivedVisitorClass, GenericVisitor>::type CallClass; \
            visitor.CallClass::apply(visitable); \
        } \
    } invoker;


#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_GENERICVISITOR_H_ */
