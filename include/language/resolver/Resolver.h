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

#ifndef ZILLIANS_LANGUAGE_RESOLVER_H_
#define ZILLIANS_LANGUAGE_RESOLVER_H_

#include "language/tree/ASTNodeFactory.h"
#include "language/tree/visitor/ResolutionVisitor.h"
#include "language/tree/visitor/PrettyPrintVisitor.h"
#include "language/tree/visitor/NodeInfoVisitor.h"
#include "utility/Foreach.h"
#include "language/context/ResolverContext.h"

namespace __gnu_cxx {

template<typename T>
struct hash<T*>
{
	std::size_t operator() (T* value)
	{
		return reinterpret_cast<std::size_t>(value);
	}
};

}

namespace zillians { namespace language {

class Resolver
{
public:
	Resolver()
	{ }

public:
	/**
	 * Add a scope for search
	 */
	void enterScope(tree::ASTNode& node)
	{
		tree::visitor::NodeInfoVisitor node_info_visitor;
		node_info_visitor.visit(node);
		//LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"entering scope: \"" << node_info_visitor.stream.str() << L"\"");

		//__gnu_cxx::hash_set<tree::ASTNode*>::iterator scope = current_scopes.find(&node);
		//if(scope == current_scopes.end())
		//{
		//	current_scopes.insert(&node);
		//}
        if(std::find(current_scopes.begin(), current_scopes.end(), &node) == current_scopes.end())
        {
            current_scopes.push_back(&node);
        }
		else
		{
			LOG4CXX_ERROR(LoggerWrapper::Resolver, L"enter duplicated scope: \"" << node_info_visitor.stream.str() << L"\"");
		}
	}

	/**
	 * Remove a scope from search
	 */
	void leaveScope(tree::ASTNode& node)
	{
		tree::visitor::NodeInfoVisitor node_info_visitor;
		node_info_visitor.visit(node);
		//LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"leaving scope: \"" << node_info_visitor.stream.str() << L"\"");

		//__gnu_cxx::hash_set<tree::ASTNode*>::iterator scope = current_scopes.find(&node);
		//if(scope != current_scopes.end())
		//{
		//	current_scopes.erase(scope);
		//}
        if(*current_scopes.rbegin() == &node)
        {
            current_scopes.pop_back();
        }
		else
		{
			LOG4CXX_ERROR(LoggerWrapper::Resolver, L"leaving unknown scope: \"" << node_info_visitor.stream.str() << L"\"");
		}
	}

public:
	/**
	 * Resolve the symbol node from a specific 'scope' node and store that symbol resolution on 'attach' node
	 *
	 * @param attach the node to attach symbol resolution result
	 * @param scope the specific scope node that in which we perform the search
	 * @param node the identifier of the symbol
	 * @param no_action if no_action is specified, the resolution result will not be stored in the attach node
	 * @return true on successful resolution, false otherwise
	 *
	 * @see ResolvedSymbol
	 */
	bool resolveSymbol(tree::ASTNode& attach, tree::ASTNode& scope, tree::Identifier& node, bool no_action = false)
	{
		using namespace zillians::language::tree;

		tree::visitor::NodeInfoVisitor node_info_visitor;
		node_info_visitor.visit(scope);
		LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"trying to resolve symbol: \"" << node.toString() << L"\" from scope \"" << node_info_visitor.stream.str() << "\"");
		node_info_visitor.reset();

		if(!ResolvedSymbol::get(&attach))
		{
			resolution_visitor.reset();
			resolution_visitor.candidate(&node);
			resolution_visitor.filter(visitor::ResolutionVisitor::Filter::SYMBOL);

			{
				tree::visitor::NodeInfoVisitor node_info_visitor;
				node_info_visitor.visit(scope);
				LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"looking at scope: " << node_info_visitor.stream.str());
			}

			resolution_visitor.tryVisit(scope);

			return checkResolvedSymbol(attach, node, no_action);
		}
		else
		{
			return true;
		}
	}

	/**
	 * Resolve the symbol node from all search nodes and store that symbol resolution on 'attach' node
	 *
	 * @param attach the node to attach symbol resolution result
	 * @param node the identifier of the symbol
	 * @param no_action if no_action is specified, the resolution result will not be stored in the attach node
	 * @return true on successful resolution, false otherwise
	 *
	 * @see ResolvedSymbol
	 */
	bool resolveSymbol(tree::ASTNode& attach, tree::Identifier& node, bool no_action = false)
	{
		using namespace zillians::language::tree;

		LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"trying to resolve symbol: \"" << node.toString() << L"\"");

		if(!ResolvedSymbol::get(&attach))
		{
			// set the look-for target
			resolution_visitor.reset();
			resolution_visitor.candidate(&node);
			resolution_visitor.filter(visitor::ResolutionVisitor::Filter::SYMBOL);

			for(auto scope = current_scopes.rbegin(); scope != current_scopes.rend(); ++scope)
			{
				tree::visitor::NodeInfoVisitor node_info_visitor;
				node_info_visitor.visit(**scope);
				LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"looking at scope: \"" << node_info_visitor.stream.str() << L"\"");

				resolution_visitor.tryVisit(**scope);
			}

			return checkResolvedSymbol(attach, node, no_action);
		}
		else
		{
			return true;
		}
	}

    // TODO take default parameter value into consideration
    void filterViableByParameterNumber(ASTNode* node, std::vector<ASTNode*>& candidates)
    {
        CallExpr* call = ASTNodeHelper::getOwner<CallExpr>(node);
        size_t numberParameters = call->parameters.size();

        auto last = std::remove_if(
                        candidates.begin(),
                        candidates.end(),
                        [numberParameters](ASTNode* node) {
                            FunctionDecl* funcDecl = cast<FunctionDecl>(node);
                            return funcDecl->parameters.size() != numberParameters;
                        });
        candidates.erase(last, candidates.end());
    }

    struct ConversionRank
    {
        enum type {
            ExactMatch, // 0
            Promotion, // 1
            StandardConversion, // 2
            NotMatch, // 3
            UnknownYet, // 4
        } ;
    } ;

    static ConversionRank::type getConversionRank(Expression* from, VariableDecl* to)
    {
        //BOOST_ASSERT(isa<ExpressionStmt>(from));
        //BOOST_ASSERT(isa<VariableDecl>(to));
        // TODO integer literal need special process!!

        // if identical, return full match
        TypeSpecifier* fromType = cast<TypeSpecifier>(ResolvedType::get(from));
        TypeSpecifier* toType   = to->type;

        if(fromType == NULL)
        {
            return ConversionRank::UnknownYet;
        }

        // primitive, function, unspecified(user_defined) can not be convert to each other
        if(fromType->type != toType->type)
        {
            return ConversionRank::NotMatch;
        }

        // so... ReferredType must be the same after here
        BOOST_ASSERT(fromType->type == toType->type);

        switch(fromType->type)
        {
        case TypeSpecifier::ReferredType::PRIMITIVE:
            // full match
            if(fromType->referred.primitive == toType->referred.primitive)
            {
                return ConversionRank::ExactMatch;
            }
            // promotion
            if(fromType->referred.primitive >= PrimitiveType::BOOL &&
                 toType->referred.primitive <= PrimitiveType::FLOAT64 &&
               fromType->referred.primitive < toType->referred.primitive)
            {
                return ConversionRank::Promotion;
            }
            // standard conversion
            if(  toType->referred.primitive >= PrimitiveType::BOOL &&
               fromType->referred.primitive <= PrimitiveType::FLOAT64 &&
                 toType->referred.primitive < fromType->referred.primitive)
            {
                return ConversionRank::Promotion;
            }
            return ConversionRank::NotMatch;
            break;

        case TypeSpecifier::ReferredType::FUNCTION_TYPE:
            UNIMPLEMENTED_CODE();
            return ConversionRank::NotMatch;
            break;

        case TypeSpecifier::ReferredType::UNSPECIFIED:
            UNIMPLEMENTED_CODE();
            return ConversionRank::NotMatch;
            break;

        default:
            UNREACHABLE_CODE();
        }

    }

    struct DeductResult
    {
        enum type
        {
            Success,
            Fail,
            UnknownYet
        } ;
    } ;

    DeductResult::type tryExactMatchedNonTemplateCandidate(ASTNode& attach, ASTNode& node, std::vector<ASTNode*>& candidates, bool no_action)
    {
        std::vector<FunctionDecl*> exactMatchedCandidate;
        CallExpr* callExpr = ASTNodeHelper::getOwner<CallExpr>(&node);
        foreach(c, candidates)
        {
            FunctionDecl* func = cast<FunctionDecl>(*c);
            if(isa<TemplatedIdentifier>(func->name))
            {
                continue;
            }

            // if all parameter exact match
            for(size_t i=0; i != func->parameters.size(); ++i)
            {
                VariableDecl* decl = func->parameters[i];
                Expression* use = callExpr->parameters[i];
                if(getConversionRank(use, decl) != ConversionRank::ExactMatch)
                {
                    continue;
                }
            }
            exactMatchedCandidate.push_back(func);
        }
        if(exactMatchedCandidate.size() == 1)
        {
            if(!no_action)
            {
                ResolvedSymbol::set(&attach, exactMatchedCandidate[0]);
                ResolvedType::set(&attach, exactMatchedCandidate[0]);
            }
            return DeductResult::Success;
        }
        else if(exactMatchedCandidate.size() > 1)
        {
            // TODO output error message to list all exact match functions
            std::cerr << "More than one exact matched non-template function" << std::endl;
            return DeductResult::Fail;
        }
        else
        {
            return DeductResult::Fail;
        }
    }

    /**
     * @brief Deduct template types from callee's arguments
     * @param call CallExpr of callee
     * @param funcDecl FunctionDecl of the function declaration
     * @return @c DeductResult::Success if deduct success,
     *         @c DeductResult::Fail if @p funcDecl is non-template,
     *         or is template but can not be deduced from arguments.(conflict)
     *         @c DeductResult::UnknownYet if some parameter is not resolved.
     *
     */
    DeductResult::type deduceToSpecializdTemplateFunction(CallExpr* call, FunctionDecl* funcDecl)
    {
        BOOST_ASSERT(isa<TemplatedIdentifier>(funcDecl->name));

        std::map<TypenameDecl*, TypeSpecifier*> deducedTypes;
        for(size_t i=0; i != call->parameters.size(); ++i)
        {
            Expression* argExpr = call->parameters[i];
            VariableDecl* paramVarDecl = funcDecl->parameters[i];

            ASTNode* argType   = ResolvedType::get(argExpr);
            ASTNode* paramType = ASTNodeHelper::findUniqueTypeResolution(paramVarDecl->type);
            if(argType == NULL || paramType == NULL)
            {
                return DeductResult::UnknownYet;
            }

            BOOST_ASSERT(argType   != NULL);
            BOOST_ASSERT(paramType != NULL);

            if(!argType->isEqual(*paramType))
            {
                return DeductResult::Fail;
            }
        }

        return DeductResult::Success;
    }

    void filterMatchedCandidateByDegreeOfFreedom(std::vector<FunctionDecl*>& exactMatchedCandidate)
    {
        if(exactMatchedCandidate.empty())
        {
            return;
        }

        auto iterMax = std::min_element(exactMatchedCandidate.begin(), exactMatchedCandidate.end(), [](FunctionDecl* a, FunctionDecl* b){
            TemplatedIdentifier* templatedIdA = cast<TemplatedIdentifier>(a->name);
            TemplatedIdentifier* templatedIdB = cast<TemplatedIdentifier>(b->name);
            return templatedIdA->templated_type_list.size() < templatedIdB->templated_type_list.size();
        });

        FunctionDecl* minFunc = cast<FunctionDecl>(*iterMax);
        TemplatedIdentifier* minTemplate = cast<TemplatedIdentifier>(minFunc->name);
        size_t minDegreeOfFreedom = minTemplate->templated_type_list.size();

        auto last = std::remove_if(exactMatchedCandidate.begin(), exactMatchedCandidate.end(), [minDegreeOfFreedom](FunctionDecl* a){
            TemplatedIdentifier* templatedId = cast<TemplatedIdentifier>(a->name);
            return templatedId->templated_type_list.size() != minDegreeOfFreedom;
        });

        exactMatchedCandidate.erase(last, exactMatchedCandidate.end());
    }

    DeductResult::type tryExactMatchedSpecializedTemplateCandidate(ASTNode& attach, ASTNode& node, std::vector<ASTNode*>& candidates, bool no_action)
    {
        std::vector<FunctionDecl*> exactMatchedCandidate;
        CallExpr* callExpr = ASTNodeHelper::getOwner<CallExpr>(&node);
        foreach(c, candidates)
        {
            FunctionDecl* func = cast<FunctionDecl>(*c);
            if(!isFullySpecializedTemplatedIdentifier(func->name))
            {
                continue;
            }

            DeductResult::type deductResult = deduceToSpecializdTemplateFunction(callExpr, func);
            switch(deductResult)
            {
            case DeductResult::Success:
                exactMatchedCandidate.push_back(func);
                break;
            case DeductResult::Fail:
                break;
            case DeductResult::UnknownYet:
                return DeductResult::UnknownYet;
                break;
            }
        }
        filterMatchedCandidateByDegreeOfFreedom(exactMatchedCandidate);
        if(exactMatchedCandidate.size() == 1)
        {
            if(!no_action)
            {
                ResolvedSymbol::set(&attach, exactMatchedCandidate[0]);
                ResolvedType::set(&attach, exactMatchedCandidate[0]);
            }
            return DeductResult::Success;
        }
        else if(exactMatchedCandidate.size() > 1)
        {
            // TODO output error message to list all exact match functions
            std::cerr << "More than one exact matched non-template function" << std::endl;
            return DeductResult::Fail;
        }
        else
        {
            return DeductResult::Fail;
        }
        UNREACHABLE_CODE();
        return DeductResult::Fail;
    }

    /**
     * @brief Deduct template types from callee's arguments
     * @param call CallExpr of callee
     * @param funcDecl FunctionDecl of the function declaration
     * @return @c DeductResult::Success if deduct success,
     *         @c DeductResult::Fail if @p funcDecl is non-template,
     *         or is template but can not be deduced from arguments.(conflict)
     *         @c DeductResult::UnknownYet if some parameter is not resolved.
     *
     */
    DeductResult::type deduceToGeneralTemplateFunction(CallExpr* call, FunctionDecl* funcDecl, std::map<std::wstring, TypeSpecifier*>& deducedTypes)
    {
        BOOST_ASSERT(isa<TemplatedIdentifier>(funcDecl->name));

        for(size_t i=0; i != call->parameters.size(); ++i)
        {
            Expression* argExpr = call->parameters[i];
            VariableDecl* paramVarDecl = funcDecl->parameters[i];

            ASTNode* argType   = ResolvedType::get(argExpr);
            ASTNode* paramType = ASTNodeHelper::findUniqueTypeResolution(paramVarDecl->type);
            BOOST_ASSERT(argType   != NULL);
            BOOST_ASSERT(paramType != NULL);

            // if none template type, check equal
            if(!isa<TypenameDecl>(paramType))
            {
                if(!paramType->isEqual(*argType)) return DeductResult::Fail;
            }

            // if template type, deduce and check equal
            else
            {
                TypenameDecl* paramTypename = cast<TypenameDecl>(paramType);
                BOOST_ASSERT(paramTypename != NULL);
                auto iterDeduced = deducedTypes.find(paramTypename->name->toString());
                if(iterDeduced == deducedTypes.end())
                {
                    deducedTypes.insert(std::make_pair(paramTypename->name->toString(), ASTNodeHelper::createTypeSpecifierFrom(argType)));
                }
                else
                {
                    TypeSpecifier* prevDeducedType = iterDeduced->second;
                    if(!argType->isEqual(*ASTNodeHelper::findUniqueTypeResolution(prevDeducedType)))
                    {
                        return DeductResult::Fail;
                    }
                }
            }
        }

        return DeductResult::Success;
    }

    DeductResult::type tryDeduceToGeneralTemplateCandidate(ASTNode& attach, ASTNode& node, std::vector<ASTNode*>& candidates, bool no_action)
    {
        std::vector<FunctionDecl*> exactMatchedCandidate;
        std::map<FunctionDecl*, std::map<std::wstring, TypeSpecifier*>> candidatesDeducedTypes;
        CallExpr* callExpr = ASTNodeHelper::getOwner<CallExpr>(&node);

        foreach(c, candidates)
        {
            FunctionDecl* func = cast<FunctionDecl>(*c);
            if(!isa<TemplatedIdentifier>(func->name) || isFullySpecializedTemplatedIdentifier(func->name))
            {
                continue;
            }

            std::map<std::wstring, TypeSpecifier*> deducedTypes;
            DeductResult::type deductResult = deduceToGeneralTemplateFunction(callExpr, func, deducedTypes);
            switch(deductResult)
            {
            case DeductResult::Success:
                exactMatchedCandidate.push_back(func);
                candidatesDeducedTypes.insert(std::make_pair(func, deducedTypes));
                break;
            case DeductResult::Fail:
                break;
            case DeductResult::UnknownYet:
                return DeductResult::UnknownYet;
                break;
            }
        }
        filterMatchedCandidateByDegreeOfFreedom(exactMatchedCandidate);
        if(exactMatchedCandidate.size() == 1)
        {
            // instantiate
            if(!no_action)
            {
                // (which is delayed to later applyTrasnform() because we don't want to change the tree while traversing it)
                auto ranges = function_instantiations.equal_range(exactMatchedCandidate[0]);
                bool found = false;
                for(auto i = ranges.first;i != ranges.second; ++i)
                {
                    // if there's already function instantiation requested
                    zillians::language::tree::visitor::ResolutionVisitor v;
                    if(candidatesDeducedTypes[exactMatchedCandidate[0]] == i->second.deduced_types)
                    {
                        // just append the attach node to the attach list
                        i->second.to_attach.push_back(&attach);
                        return DeductResult::Success;
                    }
                }

                // if there's no function instantiation requested, create one
                if(!found)
                {
                    const std::map<std::wstring, TypeSpecifier*>& deducedTypes = candidatesDeducedTypes[exactMatchedCandidate[0]];
                    function_instantiations.insert(std::make_pair(exactMatchedCandidate[0], FunctionInstantiationInfo(deducedTypes, &attach)));
                }
            }

            return DeductResult::Success;
        }
        else if(exactMatchedCandidate.size() > 1)
        {
            // TODO output error message to list all exact match functions
            std::cerr << "More than one exact matched non-template function" << std::endl;
            return DeductResult::Fail;
        }
        else
        {
            return DeductResult::Fail;
        }
        return DeductResult::Fail;
    }

    bool getBestViable(ASTNode& attach, ASTNode& node, std::vector<ASTNode*>& candidates, bool no_action)
    {
        //if(isTypeParameterQualified(resolution_visitor))
        {
            DeductResult::type result = DeductResult::UnknownYet;

            result = tryExactMatchedNonTemplateCandidate(attach, node, candidates, no_action);
            if(result == DeductResult::UnknownYet) return false;
            if(result == DeductResult::Success) return true;

            result = tryExactMatchedSpecializedTemplateCandidate(attach, node, candidates, no_action);
            if(result == DeductResult::UnknownYet) return false;
            if(result == DeductResult::Success) return true;

            result = tryDeduceToGeneralTemplateCandidate(attach, node, candidates, no_action);
            if(result == DeductResult::UnknownYet) return false;
            if(result == DeductResult::Success) return true;

            //result = tryConvertToNonTemplateCandidate(attach, node, candidates, no_action);
            //if(result == DeductResult::UnknownYet) return false;
            //if(result == DeductResult::Success) return true;

            return false;
        }
        //else
        //{
        //    if(getConvertToSpecializedTemplateCandidate(attach, node, candidates, no_action))
        //        return true;

        //    if(getTryInstantiateGeneralTemplateCandidate(attach, node, candidates, no_action))
        //        return true;

        //    return NULL;
        //}
    }

    bool isAllArgumentsResolved(CallExpr* call)
    {
        foreach(i, call->parameters)
        {
            if(ResolvedType::get(*i) == NULL)
            {
                return false;
            }
        }
        return true;
    }

    // Three steps:
    // 1. visiable and the same name
    // 2. vialble / callable
    //      a) Number of parameter match. If callee is less, rest parameters must have default values
    //      b) **NOT** take type match into consideration, which will be considered in step 3
    // 3. best viable: (4 priorities)
    //      if callee is not 'type-parameter-qualified'
    //          a) 'EXACT MATCHED' non-template version is best
    //          b) 'EXACT MATCHED' specialized version is better
    //          c) Try deduction to general template version (because if type deduction success, it exact match! without conversion)
    //          d) Try converse to non-template version (2 priorities)
    //              i)  Exact match is better than promotion
    //              ii) Promotion is better than standard conversion
    //          e) **NOT** try to conversion to specialized version
    //      if callee is 'type-parameter-qualified'
    //          a) 'EXACT MATCHED' specialized version is better (will has only one candidate)
    //          b) Try instantiate a general template version (will has only one candidate)
    bool tryInstantiateFunctionTemplate(tree::ASTNode& attach, tree::Identifier& node, bool no_action)
    {
        // if not all arguments had been resolved,
        // don't have resolve this function call, meaningless.
        CallExpr* call = ASTNodeHelper::getOwner<CallExpr>(&node);
        if(!isAllArgumentsResolved(call))
        {
            return false;
        }

    	// if there's no candidates, of course we cannot find class template to instantiate
        if(resolution_visitor.candidates.size() == 0)
        {
            return false;
        }

        // up to here, all candidates are 'visiable' and 'the same name'
        filterViableByParameterNumber(&node, resolution_visitor.candidates);

        // now the parameter number match
        return getBestViable(attach, node, resolution_visitor.candidates, no_action);
    }

private:
	/**
	 * Called by resolveSymbol() to check and store collected symbol resolutions
	 *
	 * If there's only one symbol resolution, we just store it using ResolvedSymbol::set(), otherwise resolver would throw out error
	 *
	 * @param attach
	 * @param node
	 * @param no_action
	 * @return true on successful resolution, false otherwise
	 */
	bool checkResolvedSymbol(tree::ASTNode& attach, tree::Identifier& node, bool no_action)
	{
		using namespace zillians::language::tree;

        // check if it's function template and need instantiation first
        if(ASTNodeHelper::isCallIdentifier(&node))
        {
            if(tryInstantiateFunctionTemplate(attach, node, no_action))
            {
                return true;
            }
            else
            {
                return false;
            }
        }
        // otherwise, we go for standard resolution
        else
        {
            if(resolution_visitor.candidates.size() == 1)
            {
                ASTNode* ref = resolution_visitor.candidates[0];

                tree::visitor::NodeInfoVisitor node_info_visitor;
                node_info_visitor.visit(*ref);
                LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"symbol \"" << node.toString() << L"\" is resolved to: \"" << node_info_visitor.stream.str() << L"\"");
                node_info_visitor.reset();

                bool valid = true;
                if(isa<VariableDecl>(ref)) // declared variable (as class member variable or local variable or function parameter)
                {
                    if(!no_action)
                    {
                        ResolvedSymbol::set(&attach, ref);
                        BOOST_ASSERT(ASTNodeHelper::findUniqueTypeResolution(cast<VariableDecl>(ref)->type) != NULL);
                        ResolvedType::set(&attach, ASTNodeHelper::findUniqueTypeResolution(cast<VariableDecl>(ref)->type));
                    }
                }
                else if(isa<FunctionDecl>(ref)) // declared function (as class member function or global function)
                {
                    if(!no_action)
                    {
                        ResolvedSymbol::set(&attach, ref);
                        ResolvedType::set(&attach, ref);
                    }
                }
                else if(isa<EnumDecl>(ref))
                {
                    if(!no_action)
                    {
                        ResolvedSymbol::set(&attach, ref);
                        ResolvedType::set(&attach, ref);
                    }
                }
                else if(isa<ClassDecl>(ref) || isa<InterfaceDecl>(ref)) // declared class/interface
                {
                    if(!no_action)
                    {
                        ResolvedSymbol::set(&attach, ref);
                        ResolvedType::set(&attach, ref);
                    }
                }
                else if(isa<TypedefDecl>(ref))
                {
                    if(!no_action)
                    {
                        UNREACHABLE_CODE();
                        // TODO will this happen?
                    }
                }
                else
                {
                    LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"resolve symbol \"" << node.toString() << L"\" to unkown symbol");
                    valid = false;
                }

                resolution_visitor.reset();
                return valid;
            }
            else
            {
                if(resolution_visitor.candidates.size() > 1)
                {
                    // mode than one candidate
                    LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"ambiguous symbol \"" << node.toString() << L"\"");

                    tree::visitor::NodeInfoVisitor node_info_visitor;
                    foreach(i, resolution_visitor.candidates)
                    {
                        node_info_visitor.visit(**i);
                        LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"symbol can be resolved to: \"" << node_info_visitor.stream.str());
                        node_info_visitor.reset();
                    }
                }
                else
                {
                    // no candidate
                    LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"unresolved symbol \"" << node.toString() << L"\"");
                }

                resolution_visitor.reset();
                return false;
            }
        }
	}

public:
	bool resolveType(tree::ASTNode& attach, tree::ASTNode& scope, tree::TypeSpecifier& node, bool no_action = false)
	{
		using namespace zillians::language::tree;

		tree::visitor::NodeInfoVisitor node_info_visitor;
		node_info_visitor.visit(scope);
		LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"trying to resolve type: \"" << node.referred.unspecified->toString() << L"\" from scope \"" << node_info_visitor.stream.str() << L"\"");
		node_info_visitor.reset();

		if(node.type == TypeSpecifier::ReferredType::UNSPECIFIED)
		{
			resolution_visitor.reset();
			resolution_visitor.candidate(node.referred.unspecified);
			resolution_visitor.filter(visitor::ResolutionVisitor::Filter::TYPE);

			resolution_visitor.tryVisit(scope);

			return checkResolvedType(attach, node, no_action);
		}
		else
		{
			return true;
		}
	}

	bool resolveType(tree::ASTNode& attach, tree::TypeSpecifier& node, bool no_action = false)
	{
		using namespace zillians::language::tree;

		LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"trying to resolve type: \"" << node.referred.unspecified->toString() << L"\"");

		if(node.type == TypeSpecifier::ReferredType::UNSPECIFIED)
		{
			// set the look-for target
			resolution_visitor.reset();
			resolution_visitor.candidate(node.referred.unspecified);
			resolution_visitor.filter(visitor::ResolutionVisitor::Filter::TYPE);

            // TODO FIX search candidate in scopes by inner-outer order
			//for(__gnu_cxx::hash_set<ASTNode*>::const_iterator scope = current_scopes.begin(); scope != current_scopes.end(); ++scope)
			for(auto scope = current_scopes.rbegin(); scope != current_scopes.rend(); ++scope)
			{
				resolution_visitor.tryVisit(**scope);
			}

			return checkResolvedType(attach, node, no_action);
		}
		else
		{
			return true;
		}
	}

private:
    static bool isFullySpecializedTemplatedIdentifier(tree::Identifier* id)
    {
    	using namespace zillians::language::tree;
    	if(isa<TemplatedIdentifier>(id))
    	{
    		if(cast<TemplatedIdentifier>(id)->isFullySpecialized())
    			return true;
    	}
    	return false;
    }

    static bool isPartiallySpecializedTemplatedIdentifier(tree::Identifier* id)
    {
    	using namespace zillians::language::tree;
    	if(isa<TemplatedIdentifier>(id))
    	{
    		if(!cast<TemplatedIdentifier>(id)->isFullySpecialized())
    			return true;
    	}
    	return false;
    }

    /**
     * Compare two TypenameDecl object to determine if "a" is a equally or more specialized than "b"
     *
     * @param a the "a" TypenameDecl object
     * @param b the "b" TypenameDecl object
     * @return true if a is equally or more specialized than b
     */
    static bool isEquallyOrMoreSpecialized(tree::TypenameDecl* a, tree::TypenameDecl* b)
    {
    	using namespace zillians::language::tree;

		// if b is a generalized type (not a specialized type), then a must be equally or more specific than b
    	if(!b->specialized_type)
    	{
    		return true;
    	}
    	// otherwise, b is a specialized type, then
    	else
    	{
    		// a must be also a specialized type, otherwise b will be more specific than a
    		if(!a->specialized_type)
    		{
    			return false;
    		}
    		// if a is also a specialized type, then
    		else
    		{
    			// if both a and b are type specifiers of primitive types, than it must be equal
    			if(a->specialized_type->type == TypeSpecifier::ReferredType::PRIMITIVE &&
    			   b->specialized_type->type == TypeSpecifier::ReferredType::PRIMITIVE)
    			{
    				BOOST_ASSERT(a->specialized_type->referred.primitive == b->specialized_type->referred.primitive);
    				return true;
    			}
    			// if both a and b are type specifiers of unspecified types, than
    			else if(a->specialized_type->type == TypeSpecifier::ReferredType::UNSPECIFIED &&
    					b->specialized_type->type == TypeSpecifier::ReferredType::UNSPECIFIED)
    			{
    				// if both of them are templated identifier, compare them recursively
    				if(isPartiallySpecializedTemplatedIdentifier(a->specialized_type->referred.unspecified) &&
    				   isPartiallySpecializedTemplatedIdentifier(b->specialized_type->referred.unspecified))
    				{
        				return isEquallyOrMoreSpecialized(cast<TemplatedIdentifier>(a->specialized_type->referred.unspecified), cast<TemplatedIdentifier>(b->specialized_type->referred.unspecified));
    				}
    				// if both of them are not templated identifier, which could be either simple identifier or nested identifier,
    				// they should be the same because we have ruled out all incompatible types in ResolutionVisitor
    				else if(!isPartiallySpecializedTemplatedIdentifier(a->specialized_type->referred.unspecified) &&
    	    		        !isPartiallySpecializedTemplatedIdentifier(b->specialized_type->referred.unspecified))
    				{
    					//BOOST_ASSERT(a->specialized_type->referred.unspecified->isEqual(*b->specialized_type->referred.unspecified));
                        zillians::language::tree::visitor::ResolutionVisitor v;
                        bool partialmatch;
                        BOOST_ASSERT(v.compare(a->specialized_type->referred.unspecified, b->specialized_type->referred.unspecified, partialmatch));
    					return true;
    				}
    				// otherwise, either a or b are templated identifier and the other one is not
    				else
    				{
    					// if a is not a partially specialized templated identifier and b is not, then a is more specific than b
    					if(!isPartiallySpecializedTemplatedIdentifier(a->specialized_type->referred.unspecified) &&
    					   isPartiallySpecializedTemplatedIdentifier(b->specialized_type->referred.unspecified))
    					{
    						return true;
    					}
    					else
    					{
    						return false;
    					}
    				}
    			}
    			else if(a->specialized_type->type == TypeSpecifier::ReferredType::FUNCTION_TYPE &&
    					b->specialized_type->type == TypeSpecifier::ReferredType::FUNCTION_TYPE)
    			{
    				// TODO handle function type specialization
    				return false;
    			}
    			else
    			{
    				UNREACHABLE_CODE();
    				return false;
    			}
    		}
    	}
    }

    /**
     * Compare two template identifier to determine if "a" is a equally or more specialized template identifier than "b"
     *
     * We try to compare the template identifier by comparing every corresponding TypenameDecl pair,
     * and if any one of these pair, the TypenameDecl object from a is not more specialized than the one from b,
     * then b must be more specialized than a, so we return false
     *
     * @param a the "a" template identifier
     * @param b the "b" template identifier
     * @return true if a is equally or more specialized than b
     */
    static bool isEquallyOrMoreSpecialized(tree::TemplatedIdentifier* a, tree::TemplatedIdentifier* b)
    {
    	for(size_t i = 0; i < a->templated_type_list.size(); ++i)
    	{
    		if(!isEquallyOrMoreSpecialized(a->templated_type_list[i], b->templated_type_list[i]))
    			return false;
    	}
    	return true;
    }

    /**
     * Compare two class template objects to determine if "a" is equally or more specialized than "b"
     *
     * @param a the "a" class template
     * @param b the "b" class template
     * @return true if a is equally or more specialized than b
     */
    static bool isEquallyOrMoreSpecialized(tree::ClassDecl* a, tree::ClassDecl* b)
    {
    	using namespace zillians::language::tree;

    	tree::TemplatedIdentifier* templated_id_a = tree::cast<tree::TemplatedIdentifier>(a->name);
    	tree::TemplatedIdentifier* templated_id_b = tree::cast<tree::TemplatedIdentifier>(b->name);

    	BOOST_ASSERT(templated_id_a != NULL);
    	BOOST_ASSERT(templated_id_b != NULL);

    	BOOST_ASSERT(templated_id_a->templated_type_list.size() == templated_id_b->templated_type_list.size());

    	return isEquallyOrMoreSpecialized(templated_id_a, templated_id_b);
    }

    /**
     * try to find the most specialized class template and report error if there's ambiguous resolution
     *
     * @return the most specialized class template, which can be fully or partially specialized or with no specialization at all
     */
    tree::ClassDecl* findMostSpecializedClassTemplate()
    {
    	using namespace zillians::language::tree;

    	std::vector<ClassDecl*> most_specialized_class_templates;
    	foreach(i, resolution_visitor.candidates)
		{
    		bool is_equal_or_more_specialized_than_the_rest = true;
    		foreach(j, resolution_visitor.candidates)
			{
    			if(*i != *j) // skip comparing to itself
    			{
					if(!isEquallyOrMoreSpecialized(cast<ClassDecl>(*i), cast<ClassDecl>(*j)))
					{
						is_equal_or_more_specialized_than_the_rest = false;
						break;
					}
    			}
			}

    		if(is_equal_or_more_specialized_than_the_rest)
    			most_specialized_class_templates.push_back(cast<ClassDecl>(*i));
		}

    	if(most_specialized_class_templates.size() == 0)
    	{
    		// we got no suitable class template
    		LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"no suitable class template to instantiate");
    		return NULL;
    	}
    	else if(most_specialized_class_templates.size() == 1)
    	{
    		// we got the only one, most specific class template, that's all we need
    		return most_specialized_class_templates[0];
    	}
    	else if(most_specialized_class_templates.size() > 1)
    	{
    		// log ambiguous template resolution with all class templates in most_specific_class_templates
    		LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"ambiguous class template resolution");
    		foreach(i, most_specialized_class_templates)
    		{
                visitor::NodeInfoVisitor node_info_visitor;
                node_info_visitor.visit(**i);
                LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"\tcandidates are: \"" << node_info_visitor.stream.str() << L"\"");
                node_info_visitor.reset();
    		}
    		return NULL;
    	}

    	UNREACHABLE_CODE();

    	return NULL;
    }

    /**
     * Create/Instantiate specialized class template
     *
     * @param node the type specifier we are trying to resolve
     * @param from the most specified class template (but not fully specialized)
     * @return the fully specialized class template instantiation
     */
    tree::ClassDecl* instantiateClassTemplate(zillians::language::tree::TypeSpecifier* node, tree::ClassDecl* from)
    {
    	using namespace zillians::language::tree;
    	Package* owner_package = ASTNodeHelper::getOwner<Package>(from);

    	BOOST_ASSERT(owner_package != NULL && "can't find owner package for class template");

    	// make a clone from class template
    	ClassDecl* to = cast<ClassDecl>(ASTNodeHelper::clone(from));
    	owner_package->addObject(to);

    	// update the templated identifier to make it a class instantiation
    	TemplatedIdentifier* use_id = cast<TemplatedIdentifier>(node->referred.unspecified);
    	TemplatedIdentifier* decl_id = cast<TemplatedIdentifier>(to->name);

    	for(std::size_t i=0;i<decl_id->templated_type_list.size();++i)
    	{
    		TypenameDecl* decl_typename = decl_id->templated_type_list[i];

    		TypeSpecifier* specialized_type_to_replace = NULL;
    		if(i < use_id->templated_type_list.size())
    		{
    			TypenameDecl* use_typename = use_id->templated_type_list[i];

    			// make the use type as the specialized type in decl id
                ASTNode* uniq = tree::ASTNodeHelper::findUniqueTypeResolution(use_typename->specialized_type);

                ClassDecl* classDecl = cast<ClassDecl>(uniq);
                if(classDecl != NULL)
                {
                    Identifier* classIdentifier = cast<Identifier>(ASTNodeHelper::clone(classDecl->name));
                    TypeSpecifier* fier = new TypeSpecifier(classIdentifier);

                    // template argu is class template
                    TemplatedIdentifier* templateId = cast<TemplatedIdentifier>(classIdentifier);
                    if(templateId != NULL)
                    {
                        templateId->specialize();
                        //templateId->templated_type_list[]
                    }

                    specialized_type_to_replace = fier;
                }
                // primitive types
                else
                {
                    tree::ASTNode* copy = ASTNodeHelper::clone(uniq);
                    specialized_type_to_replace = tree::cast<tree::TypeSpecifier>(copy);
                }

                BOOST_ASSERT(specialized_type_to_replace != NULL);
				ResolvedType::set(specialized_type_to_replace, ResolvedType::get(use_typename->specialized_type));

    		}
    		else
    		{
    			// make the default type as the specialized type in decl id
    			ASTNode* resolved_default_type = ResolvedType::get(decl_typename->default_type);
    			if(isa<ClassDecl>(resolved_default_type))
    			{
    				// create a TypeSpecifier which resolve to the class
    				specialized_type_to_replace = new TypeSpecifier(cast<Identifier>(ASTNodeHelper::clone(cast<ClassDecl>(resolved_default_type)->name)));
    				ResolvedType::set(specialized_type_to_replace, resolved_default_type);
    			}
    			else
    			{
    				UNREACHABLE_CODE();
    			}
    		}

    		// replace the current specialized type to the desired specialized type
			if(!decl_typename->specialized_type)
			{
				decl_typename->setSpecializdType(specialized_type_to_replace);
			}
			else
            {
                if(decl_id->templated_type_list[i]->specialized_type->type == TypeSpecifier::ReferredType::UNSPECIFIED &&
                   isPartiallySpecializedTemplatedIdentifier(decl_id->templated_type_list[i]->specialized_type->referred.unspecified))
                {
                    decl_typename->replaceUseWith(*decl_typename->specialized_type, *specialized_type_to_replace);
                }
			}

    		// remove all default types
			decl_typename->setDefaultType(NULL);
    	}

    	return to;
    }

    /**
     * try to find and instantiate (if needed) the best class template candidates among all
     *
     * @param attach the node where we can attach ResolvedType context on
     * @param node the type specifier we are trying to resolve
     * @param no_action if no_action is enabled, the function will make no change to the AST
     * @return true if either class template is instantiated or specialized class template is found and bind to the attach node; false otherwise
     */
    bool tryInstantiateClassTemplate(tree::ASTNode& attach, zillians::language::tree::TypeSpecifier& node, bool no_action)
    {
    	using namespace zillians::language::tree;

    	// if the current identifier is not a fully specialized templated identifier, it's not suitable for class template instantiation
    	if(node.type != TypeSpecifier::ReferredType::UNSPECIFIED && !isFullySpecializedTemplatedIdentifier(node.referred.unspecified))
        {
            LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"type \"" << node.referred.unspecified->toString() << L"\" is not fully specialized template.\"");
    		return false;
        }

    	// if there's no candidates, of course we cannot find class template to instantiate
    	if(resolution_visitor.candidates.size() == 0)
    		return false;

		// make sure all candidates are class with templated identifier as its name
		// so that it does not lead ambiguous types in the end
		foreach(i, resolution_visitor.candidates)
		{
			if(!(isa<ClassDecl>(*i) && isa<TemplatedIdentifier>(cast<ClassDecl>(*i)->name)))
				return false;
		}

    	ClassDecl* most_specialized_class_template = NULL;

    	if(resolution_visitor.candidates.size() == 1)
    	{
    		// if there's exactly one class template candidates, then that's the closest we can get
    		most_specialized_class_template = cast<ClassDecl>(resolution_visitor.candidates[0]);
    	}
    	else
    	{
    		// if there's more than one class template candidates
			// try to find the most specialized version of class templates
			// note that as we have filtered out all incompatible class templates, all candidates should be able to fit the current identifier
			// so we just need to find the most specialized version of class templates among all candidates,
			// and this version is the one we are looking for
			most_specialized_class_template = findMostSpecializedClassTemplate();
    	}

    	// if we can't find the most specialized class template due to ambiguous instantiation, return false
    	if(!most_specialized_class_template)
    		return false;

        LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"type \"" << node.referred.unspecified->toString() << L"\" is resolved to class template: \"" << most_specialized_class_template->name->toString() << L"\"");

    	// otherwise
    	TemplatedIdentifier* id = cast<TemplatedIdentifier>(most_specialized_class_template->name);
    	if(id->isFullySpecialized())
    	{
    		// we got the fully specialized version, so just set the resolve type
    		if(!no_action)
    		{
                visitor::NodeInfoVisitor node_info_visitor;
                node_info_visitor.visit(*most_specialized_class_template);
                LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"type \"" << node.referred.unspecified->toString() << L"\" is resolved to: \"" << node_info_visitor.stream.str() << L"\"");
                node_info_visitor.reset();

    			ResolvedType::set(&attach, most_specialized_class_template);
    		}
    		return true;
    	}
    	else
    	{
    		if(!no_action)
    		{
        		// the most specialized class template is not fully specialized, we need to create new instantiation
    			// (which is delayed to later applyTrasnform() because we don't want to change the tree while traversing it)
    			auto ranges = class_instantiations.equal_range(most_specialized_class_template);
    			bool found = false;
    			for(auto i = ranges.first;i != ranges.second; ++i)
    			{
    				// if there's already class instantiation requested
                    zillians::language::tree::visitor::ResolutionVisitor v;
                    bool partialmatch;
    				if(v.compare(i->second.specifier->referred.unspecified, node.referred.unspecified, partialmatch))
    				{
    					// just append the attach node to the attach list
    					i->second.to_attach.push_back(&attach);
    					return true;
    				}
    			}

    			// if there's no class instantiation requested, create one
    			if(!found)
    			{
                    class_instantiations.insert(std::make_pair(most_specialized_class_template, ClassInstantiationInfo(&node, &attach)));
    			}
    		}
    		return true;
    	}
    }

    void applyClassInstantiation()
    {
    	using namespace zillians::language::tree;

    	for(auto i = class_instantiations.begin(); i != class_instantiations.end(); ++i)
    	{
    		ClassDecl* class_template = i->first;
    		TypeSpecifier* specifier = i->second.specifier;

    		if(true)
    		{
    			visitor::NodeInfoVisitor node_info_visitor;
    			node_info_visitor.visit(*class_template);
    			LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"try to instantiate type \"" << node_info_visitor.stream.str() << L"\"");
    			node_info_visitor.reset();
    		}

    		ClassDecl* instantiated = instantiateClassTemplate(specifier, class_template);
            zillians::language::InstantiatedFrom::set(instantiated, class_template);

    		foreach(j, i->second.to_attach)
    		{
    			ResolvedType::set(*j, instantiated);
    		}

    		if(true)
    		{
				visitor::NodeInfoVisitor node_info_visitor;
				node_info_visitor.visit(*instantiated);
				LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"type \"" << specifier->referred.unspecified->toString() << L"\" is resolved to: \"" << node_info_visitor.stream.str() << L"\"");
				node_info_visitor.reset();
    		}
    	}

		class_instantiations.clear();
    }

    FunctionDecl* instantiateFunctionTemplate(const std::map<std::wstring, TypeSpecifier*>& deducedTypes, FunctionDecl* func)
    {
        BOOST_ASSERT(!deducedTypes.empty());
        BOOST_ASSERT(isa<TemplatedIdentifier>(func->name));

        Package* owner_package = ASTNodeHelper::getOwner<Package>(func);
        BOOST_ASSERT(owner_package != NULL && "can't find owner package for class template");
        FunctionDecl* result = cast<FunctionDecl>(ASTNodeHelper::clone(func));
        owner_package->addObject(result);

        TemplatedIdentifier* tid = cast<TemplatedIdentifier>(result->name);
        for(size_t i=0; i != tid->templated_type_list.size(); ++i)
        {
            TypenameDecl* typenameDecl = tid->templated_type_list[i];
            auto iter = deducedTypes.find(typenameDecl->name->toString());
            BOOST_ASSERT(iter != deducedTypes.end());
            TypeSpecifier* const deducedType = cast<TypeSpecifier>(ASTNodeHelper::clone(iter->second));
            typenameDecl->setSpecializdType(deducedType);
            BOOST_ASSERT(deducedType != NULL);
        }
        return result;
    }

    void applyFunctionInstantiation()
    {
        foreach(i, function_instantiations)
        {
            FunctionDecl* funcTemplateDecl = i->first;
            const std::map<std::wstring, TypeSpecifier*>& deducedTypes = i->second.deduced_types;

            FunctionDecl* instantiated = instantiateFunctionTemplate(deducedTypes, funcTemplateDecl);
            zillians::language::InstantiatedFrom::set(instantiated, funcTemplateDecl);

            foreach(j, i->second.to_attach)
            {
                ResolvedType::set(*j, instantiated);
                ResolvedSymbol::set(*j, instantiated);
            }
        }
    }

private:
	bool checkResolvedType(tree::ASTNode& attach, tree::TypeSpecifier& node, bool no_action)
	{
		using namespace zillians::language::tree;

        // check if it's class template and need instantiation first
        if(tryInstantiateClassTemplate(attach, node, no_action))
        {
            return true;
        }
        // otherwise, we go for standard resolution
        else
        {
            if(resolution_visitor.candidates.size() == 1)
            {
                ASTNode* ref = resolution_visitor.candidates[0];

                tree::visitor::NodeInfoVisitor node_info_visitor;
                node_info_visitor.visit(*ref);
                LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"type \"" << node.referred.unspecified->toString() << L"\" is resolved to: \"" << node_info_visitor.stream.str() << L"\"");
                node_info_visitor.reset();

                bool valid = true;
                if(isa<ClassDecl>(ref))
                {
                    LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"resolve type \"" << node.referred.unspecified->toString() << L"\" to \"" << cast<ClassDecl>(ref)->name->toString() << L"\"");
                    if(!no_action) ResolvedType::set(&attach, ref);
                }
                else if(isa<InterfaceDecl>(ref))
                {
                    if(!no_action) ResolvedType::set(&attach, ref);
                }
                else if(isa<FunctionDecl>(ref))
                {
                    if(!no_action) ResolvedType::set(&attach, ref);
                }
                else if(isa<EnumDecl>(ref))
                {
                    if(!no_action) ResolvedType::set(&attach, ref);
                }
                else if(isa<TypedefDecl>(ref))
                {
                    if(!no_action) ResolvedType::set(&attach, ref);
                }
                else if(isa<TypenameDecl>(ref))
                {
                    if(!no_action) ResolvedType::set(&attach, ref);
                }
                else
                {
                    LOG4CXX_FATAL(LoggerWrapper::Resolver, L"resolve type \"" << node.referred.unspecified->toString() << L"\" to unknown type");
                    valid = false;
                }

                resolution_visitor.reset();
                return valid;
            }
            else
            {
                if(resolution_visitor.candidates.size() > 1)
                {
                    // mode than one candidate
                    LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"ambiguous type \"" << node.referred.unspecified->toString() << L"\"");

                    tree::visitor::NodeInfoVisitor node_info_visitor;
                    foreach(i, resolution_visitor.candidates)
                    {
                        node_info_visitor.visit(**i);
                        LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"type can be resolved to: \"" << node_info_visitor.stream.str() << L"\"");
                        node_info_visitor.reset();
                    }
                }
                else
                {
                    // no candidate
                    LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"unresolved type \"" << node.referred.unspecified->toString() << L"\"");
                }

                resolution_visitor.reset();
                return false;
            }
        }
	}

public:
	bool resolvePackage(tree::ASTNode& attach, tree::ASTNode& scope, tree::Identifier& node, bool no_action = false)
	{
		using namespace zillians::language::tree;

		tree::visitor::NodeInfoVisitor node_info_visitor;
		node_info_visitor.visit(scope);
		LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"trying to resolve package: \"" << node.toString() << L"\" from scope \"" << node_info_visitor.stream.str() << L"\"");
		node_info_visitor.reset();

		if(!ResolvedPackage::get(&attach))
		{
			resolution_visitor.reset();
			resolution_visitor.candidate(&node);
			resolution_visitor.filter(visitor::ResolutionVisitor::Filter::PACKAGE);

			resolution_visitor.tryVisit(scope);

			return checkResolvedPackage(attach, node, no_action);
		}
		else
		{
			return true;
		}
	}

	bool resolvePackage(tree::ASTNode& attach, tree::Identifier& node, bool no_action = false)
	{
		using namespace zillians::language::tree;

		LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"trying to resolve package: \"" << node.toString() << L"\"");

		if(!ResolvedPackage::get(&attach))
		{
			resolution_visitor.reset();
			resolution_visitor.candidate(&node);
			resolution_visitor.filter(visitor::ResolutionVisitor::Filter::PACKAGE);

			//for(__gnu_cxx::hash_set<ASTNode*>::const_iterator scope = current_scopes.begin(); scope != current_scopes.end(); ++scope)
			for(auto scope = current_scopes.rbegin(); scope != current_scopes.rend(); ++scope)
			{
				resolution_visitor.tryVisit(**scope);
			}

			return checkResolvedPackage(attach, node, no_action);
		}
		else
		{
			return true;
		}
	}

private:
	bool checkResolvedPackage(tree::ASTNode& attach, tree::Identifier& node, bool no_action)
	{
		using namespace zillians::language::tree;

		if(resolution_visitor.candidates.size() == 1)
		{
			ASTNode* ref = resolution_visitor.candidates[0];

			tree::visitor::NodeInfoVisitor node_info_visitor;
			node_info_visitor.visit(*ref);
			LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"package \"" << node.toString() << L"\" is resolved to: \"" << node_info_visitor.stream.str() << L"\"");
			node_info_visitor.reset();

			bool valid = true;
			if(isa<Package>(ref))
			{
				if(!no_action) ResolvedPackage::set(&attach, ref);
			}
			else if(isa<Import>(ref))
			{
				if(!no_action) ResolvedPackage::set(&attach, ref);
			}
			else
			{
				LOG4CXX_FATAL(LoggerWrapper::Resolver, L"resolve package \"" << node.toString() << L"\" to unknown type");
				valid = false;
			}

			resolution_visitor.reset();
			return valid;
		}
		else
		{
			if(resolution_visitor.candidates.size() > 1)
			{
				// mode than one candidate
				LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"ambiguous package \"" << node.toString() << L"\"");

				tree::visitor::NodeInfoVisitor node_info_visitor;
				foreach(i, resolution_visitor.candidates)
				{
					node_info_visitor.visit(**i);
					LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"package can be resolved to: \"" << node_info_visitor.stream.str() << L"\"");
					node_info_visitor.reset();
				}
			}
			else
			{
				// no candidate
				LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"unresolved package \"" << node.toString() << L"\"");
			}

			resolution_visitor.reset();
			return false;

		}
	}

public:
	bool hasTransforms()
	{
        return !class_instantiations.empty() || !function_instantiations.empty();
	}

	void applyTransforms()
	{
		applyClassInstantiation();
		applyFunctionInstantiation();
	}

private:
	std::vector<tree::ASTNode*> current_scopes;
	tree::visitor::ResolutionVisitor resolution_visitor;

private:
	struct ClassInstantiationInfo
	{
		ClassInstantiationInfo(tree::TypeSpecifier* _specifier, tree::ASTNode* _attach) {
			specifier = _specifier;
			to_attach.push_back(_attach);
		}

		tree::TypeSpecifier* specifier;
		std::vector<tree::ASTNode*> to_attach;
	};

	struct FunctionInstantiationInfo
	{
		FunctionInstantiationInfo(const std::map<std::wstring, TypeSpecifier*>& _deduced_types, tree::ASTNode* _attach) : deduced_types(_deduced_types)
        {
			to_attach.push_back(_attach);
		}
        std::map<std::wstring, TypeSpecifier*> deduced_types;
		std::vector<tree::ASTNode*> to_attach;
	};

	std::multimap<tree::ClassDecl*, ClassInstantiationInfo> class_instantiations;
	std::multimap<tree::FunctionDecl*, FunctionInstantiationInfo> function_instantiations;
};

} }

#endif /* ZILLIANS_LANGUAGE_RESOLVER_H_ */
