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

#ifndef ZILLIANS_LANGUAGE_TREE_ASTNODEHELPER_H_
#define ZILLIANS_LANGUAGE_TREE_ASTNODEHELPER_H_

#include <boost/filesystem.hpp>
#include "language/tree/ASTNodeFactory.h"
#include "language/context/ParserContext.h"
#include "language/context/ResolverContext.h"
#include "language/context/TransformerContext.h"
#include "language/stage/parser/context/SourceInfoContext.h"
#include "language/tree/visitor/NodeInfoVisitor.h"
#include "language/tree/visitor/ASTGraphvizGenerator.h"
#include "language/stage/transformer/context/ManglingStageContext.h"

#include <boost/mpl/vector.hpp>
#include <boost/mpl/size.hpp>
#include <boost/mpl/at.hpp>
#include <boost/mpl/int.hpp>

namespace zillians { namespace language { namespace tree {

namespace detail {

typedef boost::mpl::vector<
    zillians::language::stage::SourceInfoContext,
    zillians::language::stage::NameManglingContext
> ContextToCloneTypeList;

template<typename T, bool RecursiveVisit = true, bool IncludeSelf = true>
struct ForeachVisitor : public visitor::GenericDoubleVisitor
{
    CREATE_INVOKER(foreachInvoker, apply);

	ForeachVisitor(std::function<void(T&)>& functor) : depth(0), functor(functor)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(foreachInvoker)
    }

	void apply(ASTNode& node)
	{
		if(IncludeSelf || depth)
			functor(node);

		if(RecursiveVisit || (!RecursiveVisit && !depth))
		{
			++depth;
			revisit(node);
			--depth;
		}
	}

	int depth;
	std::function<void(T&)>& functor;
};

template<int N, typename ContextTypeList>
struct ContextCloneImpl
{
	typedef typename boost::mpl::at<ContextTypeList, boost::mpl::int_<N-1> >::type ContextT;
	static void clone(ASTNode* from, ASTNode* to)
	{
		if(from->get<ContextT>())
		{
			ContextT* ctx = new ContextT(*from->get<ContextT>());
			to->set<ContextT, ContextOwnership::transfer>(ctx);
		}
		ContextCloneImpl<N-1, ContextTypeList>::clone(from, to);
	}
};

template<typename ContextTypeList>
struct ContextCloneImpl<0, ContextTypeList>
{
	static void clone(ASTNode* from, ASTNode* to)
	{
		UNUSED_ARGUMENT(from);
		UNUSED_ARGUMENT(to);
	}
};

}

struct ASTNodeHelper
{
	template<typename T = ASTNode, bool RecursiveVisit = true, bool IncludeSelf = true>
	static void foreachApply(ASTNode& node, std::function<void(T&)> functor)
	{
		typename detail::ForeachVisitor<T, RecursiveVisit, IncludeSelf> visitor(functor);
		visitor.visit(node);
	}

	template<typename ContextTypeList = detail::ContextToCloneTypeList, bool RecursiveClone = true>
	static ASTNode* clone(ASTNode* from)
	{
		ASTNode* to = from->clone();
		clone<ContextTypeList, RecursiveClone>(from, to);
		return to;
	}

	template<typename ContextTypeList = detail::ContextToCloneTypeList, bool RecursiveClone = true>
	static void clone(ASTNode* from, ASTNode* to)
	{
		if(RecursiveClone)
		{
            // collect from nodes
            std::vector<ASTNode*> fromNodes;
            foreachApply<ASTNode>(*from, [&fromNodes](ASTNode& node){
                fromNodes.push_back(&node);
            });
            // collect to nodes
            std::vector<ASTNode*> toNodes;
            foreachApply<ASTNode>(*to, [&toNodes](ASTNode& node){
                toNodes.push_back(&node);
            });
            BOOST_ASSERT(fromNodes.size() == toNodes.size());
            // copy
            for(size_t i=0; i != fromNodes.size(); ++i)
            {
                ASTNode* fromNode = fromNodes[i];
                ASTNode* toNode   = toNodes[i];
                detail::ContextCloneImpl<boost::mpl::size<ContextTypeList>::value, ContextTypeList>::clone(fromNode, toNode);
            }
		}
		else
		{
			detail::ContextCloneImpl<boost::mpl::size<ContextTypeList>::value, ContextTypeList>::clone(from, to);
		}
	}

	static void propogateSourceInfo(ASTNode& to, ASTNode& from)
	{
		stage::SourceInfoContext* from_src_info = from.get<stage::SourceInfoContext>();
		to.set<stage::SourceInfoContext>(new stage::SourceInfoContext(*from_src_info));
	}

	static ASTNode* findUniqueTypeResolution(ASTNode* node)
	{
		if(!node)
			return NULL;

		if(TypenameDecl* typenameDecl = cast<TypenameDecl>(node))
        {
            if(typenameDecl->specialized_type)
            {
                return findUniqueTypeResolution(typenameDecl->specialized_type);
            }
            else
            {
                return node;
            }
        }

		if(isa<FunctionType>(node))
			return node;

		if(isa<TypeSpecifier>(node))
		{
			TypeSpecifier* specifier = cast<TypeSpecifier>(node);
			if(specifier->type == TypeSpecifier::ReferredType::UNSPECIFIED)
			{
				return findUniqueTypeResolution(ResolvedType::get(specifier));
			}
			if(specifier->type == TypeSpecifier::ReferredType::FUNCTION_TYPE)
			{
				return specifier->referred.function_type;
			}
			else
			{
				return specifier;
			}
		}

		if(isa<Identifier>(node))
		{
			return findUniqueTypeResolution(ResolvedType::get(node));
		}

		BOOST_ASSERT(ResolvedType::isValidResolvedType(node) && "invalid type resolution");

		return node;
	}

	static ASTNode* findUniqueSymbolResolution(ASTNode* node)
	{
		if(!node)
			return NULL;

		BOOST_ASSERT(ResolvedSymbol::isValidResolvedSymbol(node) && "invalid symbol resolution");

		return node;
	}

	static FunctionType* createFunctionTypeFromFunctionDecl(FunctionDecl* node)
	{
		BOOST_ASSERT(node && "null pointer exception");

		FunctionType* function_type = new FunctionType();

		function_type->return_type = node->type;

		if(isa<TemplatedIdentifier>(node->name))
		{
			TemplatedIdentifier* templated_name = cast<TemplatedIdentifier>(node->name);
			foreach(i, templated_name->templated_type_list)
			{
				// TODO is this correct?
//				function_type->appendTemplateParameter(i->id);
			}
		}

		foreach(i, node->parameters)
		{
			function_type->appendParameterType((*i)->type);
		}

		return function_type;
	}

	static bool compareFunctionType(FunctionType* a, FunctionType* b)
	{
		BOOST_ASSERT(a && b && "null pointer exception");

		if(a->parameter_types.size() != b->parameter_types.size()) return false;

		auto it_a = make_begin(a->parameter_types), it_a_end = make_end(a->parameter_types);
		auto it_b = make_begin(b->parameter_types), it_b_end = make_end(b->parameter_types);

		for(; it_a != it_a_end && it_b != it_b_end; ++it_a, ++it_b)
		{
			if(!compareTypeSpecifier(*it_a, *it_b))
				return false;
		}

		if(!compareTypeSpecifier(a->return_type, b->return_type))
			return false;

		return true;
	}

	static bool compareTypeSpecifier(TypeSpecifier* a, TypeSpecifier* b)
	{
		BOOST_ASSERT(a && b && "null pointer exception");

		if(a->type != b->type)
			return false;

		if(a->type == TypeSpecifier::ReferredType::FUNCTION_TYPE)
		{
			return compareFunctionType(a->referred.function_type, b->referred.function_type);
		}
		else if(a->type == TypeSpecifier::ReferredType::PRIMITIVE)
		{
			return (a->referred.primitive == b->referred.primitive);
		}
		else if(a->type == TypeSpecifier::ReferredType::UNSPECIFIED)
		{
			ASTNode* resolved_type_a = ResolvedType::get(a);
			ASTNode* resolved_type_b = ResolvedType::get(b);

			if(!resolved_type_a || !resolved_type_b)
				return false;

			if(isa<TypeSpecifier>(resolved_type_a) && isa<TypeSpecifier>(resolved_type_b))
				return compareTypeSpecifier(cast<TypeSpecifier>(resolved_type_a), cast<TypeSpecifier>(resolved_type_b));
			else if(isa<ClassDecl>(resolved_type_a) && isa<ClassDecl>(resolved_type_b))
				return (resolved_type_a == resolved_type_b);
			else if(isa<InterfaceDecl>(resolved_type_a) && isa<InterfaceDecl>(resolved_type_b))
				return (resolved_type_a == resolved_type_b);
			else if(isa<EnumDecl>(resolved_type_a) && isa<EnumDecl>(resolved_type_b))
				return (resolved_type_a == resolved_type_b);
		}

		return false;
	}

	static bool isInheritedFrom(ClassDecl* node_derived, ClassDecl* node_base)
	{
		BOOST_ASSERT(node_derived && node_base && "null pointer exception");
		ASTNode* current = node_derived;
		do
		{
			if(current == node_base)
				return true;
			if(!isa<ClassDecl>(current))
				return false;
		} while((current = ResolvedType::get(cast<ClassDecl>(current)->base)));
		return false;
	}

	static bool isInheritedFrom(InterfaceDecl* node_derived, InterfaceDecl* node_base)
	{
        foreach(i, node_derived->extend_interfaces)
        {
            ASTNode* resolvedNode = ResolvedType::get(*i);
            InterfaceDecl* resolvedInterface = cast<InterfaceDecl>(resolvedNode);
            BOOST_ASSERT(resolvedInterface != NULL);

            if(resolvedInterface == node_base) return true;
            if(isInheritedFrom(resolvedInterface, node_base)) return true;
        }
        return false;
	}

	static bool isInheritedFrom(ClassDecl* node_derived, InterfaceDecl* node_base)
	{
        // recursive check base class
        if(node_derived->base)
        {
            ASTNode* resolvedNode = ResolvedType::get(node_derived->base);
            ClassDecl* resolvedBaseClass = cast<ClassDecl>(resolvedNode);
            BOOST_ASSERT(resolvedBaseClass != NULL);
            if(isInheritedFrom(resolvedBaseClass, node_base)) return true;
        }
        else
        {
            // check all interface
            foreach(i, node_derived->implements)
            {
                ASTNode* resolvedNode = ResolvedType::get(*i);
                InterfaceDecl* resolvedInterface = cast<InterfaceDecl>(resolvedNode);
                BOOST_ASSERT(resolvedInterface != NULL);

                if(resolvedInterface == node_base) return true;
                if(isInheritedFrom(resolvedInterface, node_base)) return true;
            }
        }
        return false;
	}

	static bool isInheritedFrom(Declaration* node_derived, Declaration* node_base)
	{
		BOOST_ASSERT(node_derived && node_base && "null pointer exception");
        BOOST_ASSERT((isa<ClassDecl    >(node_derived) && isa<ClassDecl    >(node_base)) ||
                     (isa<ClassDecl    >(node_derived) && isa<InterfaceDecl>(node_base)) ||
                     (isa<InterfaceDecl>(node_derived) && isa<InterfaceDecl>(node_base)));

        if(ClassDecl* derived_class = cast<ClassDecl>(node_derived))
        {
            if(ClassDecl* base_class = cast<ClassDecl>(node_base))
            {
                return isInheritedFrom(derived_class, base_class);
            }
            else
            {
                InterfaceDecl* base_interface = cast<InterfaceDecl>(node_base);
                return isInheritedFrom(derived_class, base_interface);
            }
        }
        else
        {
            InterfaceDecl* derived_interface = cast<InterfaceDecl>(node_derived);
            InterfaceDecl* base_interface    = cast<InterfaceDecl>(node_base);
            return isInheritedFrom(derived_interface, base_interface); 
        }
	}

	template<class T> static bool hasOwner(ASTNode* node) { return getOwner<T>(node); }
	template<class T>
	static T* getOwner(ASTNode* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		for(ASTNode* p = node->parent; !!p ; p = p->parent)
			if(isa<T>(p))
				return cast<T>(p);
		return NULL;
	}

	static bool hasOwnerNamedScope(ASTNode* node) { return getOwnerNamedScope(node); }
	static ASTNode* getOwnerNamedScope(ASTNode* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		for(ASTNode* p = node->parent; p; p = p->parent)
			if(isNamedScope(p))
				return p;
		return NULL;
	}

	static bool hasNativeLinkage(ASTNode* node)
	{
		if(findAnnotation(node, L"native"))
			return true;

		if(findAnnotation(node, L"system"))
			return true;

		return false;
	}

	static bool hasAnnotation(ASTNode* node, const std::wstring& tag) { return findAnnotation(node, tag); }
	static Annotation* findAnnotation(ASTNode* node, const std::wstring& tag)
	{
		BOOST_ASSERT(node && "null pointer exception");
		BOOST_ASSERT(!tag.empty() && "empty annotation tag");
		Annotations* annotations = NULL;
		if(isa<Declaration>(node))
			annotations = cast<Declaration>(node)->annotations;
		else if(isa<Statement>(node))
			annotations = cast<Statement>(node)->annotations;
		else
			return NULL;
		if(!annotations)
			return NULL;
		foreach(i, annotations->annotation_list)
			if((*i)->name->toString() == tag)
				return *i;
		return NULL;
	}

	static ASTNode* getDebugAnnotationAttachPoint(ASTNode* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		ASTNode* current = NULL;
		ASTNode* next = node;
		do
		{
			current = next;
			if(!(next = getSplitReferenceAttachPoint(current))) // NOTE: check split-reference first
				break;
			next = getOwnerDebugAnnotationAttachPoint(next);
		} while(next && next != current);
		return current;
	}

	static bool isFuncParam(VariableDecl* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		return isa<FunctionDecl>(node->parent);
	}

	static bool isDeclStmtVarDecl(VariableDecl* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		return isa<DeclarativeStmt>(node->parent);
	}

	static bool isRootPackage(ASTNode* node)
	{
		return isa<Package>(node) && cast<Package>(node)->id->toString().empty();
	}

	static bool isInvalidDebugAnnotationAttachPoint(ASTNode* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		if(isa<VariableDecl>(node))
		{
			VariableDecl* var_decl = cast<VariableDecl>(node);
			return isFuncParam(var_decl) || isDeclStmtVarDecl(var_decl);
		}
		return false;
	}

    static bool isCallIdentifier(Identifier* node)
    {
        // primary form
        // f();
        if(isa<PrimaryExpr>(node->parent) &&
           isa<CallExpr>(node->parent->parent) &&
           node->parent == cast<CallExpr>(node->parent->parent)->node)
        {
            return true;
        }

        // member form
        // a.b.c.f();
        ASTNode* n = node;
        while(isa<MemberExpr>(n->parent))
        {
            n = n->parent;
        }
        if(isa<CallExpr>(n->parent) && cast<CallExpr>(n->parent)->node == n)
        {
            return true;
        }
        return false;
    }

	static std::wstring getNodeName(ASTNode* node, bool FQN = false)
	{
		BOOST_ASSERT(node && "null pointer exception");
		const int32 max_depth = 20;
		static tree::visitor::NodeInfoVisitor v(FQN ? max_depth : 1);
		v.reset();
		v.visit(*node);
		return v.stream.str();
	}

	static bool isUnspecifiedType(TypeSpecifier* node)
	{
		return node->type == TypeSpecifier::ReferredType::UNSPECIFIED;
	}

	static bool isPrimitiveType(TypeSpecifier* node)
	{
		return node->type == TypeSpecifier::ReferredType::PRIMITIVE;
	}

	static int32 getPrimitiveTypeSize(TypeSpecifier* node)
	{
		return PrimitiveType::byteSize(node->referred.primitive);
	}

    static void visualize(ASTNode* node, const boost::filesystem::path& path)
    {
        boost::filesystem::create_directories(path.parent_path());
        std::wofstream fout(path.string());
        if(!fout.is_open())
        {
            std::cerr << "Can not open file" << path.string() << std::endl;
            return;
        }

        fout << L"digraph G {" << std::endl;
        fout << L"    rankdir=LR;" << std::endl;

        fout << L"    // nodes" << std::endl;
        zillians::language::stage::visitor::ASTGraphvizNodeGenerator nodeGen(fout);
        nodeGen.visit(*node);

        fout << std::endl;
        fout << L"    // children to parent edges" << std::endl;
        zillians::language::stage::visitor::ASTGraphvizParentEdgeGenerator parentEdgeGen(fout);
        parentEdgeGen.visit(*node);

        fout << std::endl;
        fout << L"    // parent to children edge" << std::endl;
        zillians::language::stage::visitor::ASTGraphvizChildEdgeGenerator childEdgeGen(fout);
        childEdgeGen.visit(*node);
        fout << "}" << std::endl;
    }

    static TypeSpecifier* buildResolvableTypeSpecifier(Declaration* decl)
	{
    	TypeSpecifier* type_specifier = new TypeSpecifier(cast<Identifier>(decl->name->clone()));
    	ResolvedType::set(type_specifier, decl);
		return type_specifier;
	}

    /**
      * @brief Create type specifier from type declaration (or another type specifier)
      * @param node given declaration node
      * @return type specifier pointing to the given declaration node
      */
     static TypeSpecifier* createTypeSpecifierFrom(ASTNode* node)
     {
    	 // TODO handle FunctionType?
         BOOST_ASSERT(isa<TypeSpecifier>(node) || isa<Declaration>(node));

         if(isa<TypeSpecifier>(node))
         {
             TypeSpecifier* result = cast<TypeSpecifier>(clone(node));
             ResolvedType::set(result, ResolvedType::get(node));
             return result;
         }
         else if(Declaration* decl = cast<Declaration>(node))
         {
             TypeSpecifier* result = new TypeSpecifier(cast<Identifier>(clone(decl->name)));
             ResolvedType::set(result, decl);
             return result;
         }

         UNREACHABLE_CODE();
         return NULL;
     }

    static bool sameResolvedType(TypeSpecifier* a, TypeSpecifier* b)
	{
        BOOST_ASSERT(a && b && "bad input");
		if(a->type == b->type)
		{
			if(a->type == TypeSpecifier::ReferredType::UNSPECIFIED)
			{
				ASTNode* resolved_type_a = findUniqueTypeResolution(a);
				ASTNode* resolved_type_b = findUniqueTypeResolution(b);
				BOOST_ASSERT(resolved_type_a && resolved_type_b && "failed to resolve type");
				return (resolved_type_a == resolved_type_b);
			}
			else
				return a->isEqual(*b);
		}
		else
			return false;
	}

private:
	static bool isNamedScope(ASTNode* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		return isa<FunctionDecl>(node)
				|| isa<ClassDecl>(node)
				|| isa<InterfaceDecl>(node)
				|| (isa<Package>(node) && !cast<Package>(node)->id->toString().empty());
	}

	static bool isDebugAnnotationAttachPoint(ASTNode* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		return (isa<Statement>(node)
				|| isa<Declaration>(node)
				|| isa<Package>(node))
						&& !isInvalidDebugAnnotationAttachPoint(node);
	}

	static ASTNode* getSplitReferenceAttachPoint(ASTNode* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		ASTNode* current = NULL;
		ASTNode* next = node;
		do
		{
			current = next;
			next = SplitReferenceContext::get(current);
		} while(next && next != current);
		return current;
	}

	static ASTNode* getOwnerDebugAnnotationAttachPoint(ASTNode* node)
	{
		BOOST_ASSERT(node && "null pointer exception");
		for(ASTNode* p = node; p && !isa<Package>(p); p = p->parent)
			if(isDebugAnnotationAttachPoint(p))
				return p;
		return NULL;
	}

	ASTNodeHelper() { }
	~ASTNodeHelper() { }
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_ASTNODEHELPER_H_ */
