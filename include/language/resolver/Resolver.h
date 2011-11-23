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
		LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"entering scope: \"" << node_info_visitor.stream.str() << L"\"");

		current_scopes.insert(&node);
	}

	/**
	 * Remove a scope from search
	 */
	void leaveScope(tree::ASTNode& node)
	{
		tree::visitor::NodeInfoVisitor node_info_visitor;
		node_info_visitor.visit(node);
		LOG4CXX_DEBUG(LoggerWrapper::Resolver, L"leaving scope: \"" << node_info_visitor.stream.str() << L"\"");

		__gnu_cxx::hash_set<tree::ASTNode*>::iterator scope = current_scopes.find(&node);
		if(scope != current_scopes.end())
		{
			current_scopes.erase(scope);
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

			for(__gnu_cxx::hash_set<ASTNode*>::const_iterator scope = current_scopes.begin(); scope != current_scopes.end(); ++scope)
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
					ResolvedType::set(&attach, cast<VariableDecl>(ref)->type);
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
			for(__gnu_cxx::hash_set<ASTNode*>::const_iterator scope = current_scopes.begin(); scope != current_scopes.end(); ++scope)
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

    bool isAllCandidatesAreClassTemplate()
    {
        foreach(i, resolution_visitor.candidates)
        {
            if(!tree::isa<tree::ClassDecl>(*i)) return false;
            if(!tree::isa<tree::TemplatedIdentifier>(tree::cast<tree::ClassDecl>(*i)->name)) return false;
        }
        return true;
    }

    bool templateArgumentTypeListIsEqual(const std::vector<tree::ASTNode*>& a, const std::vector<tree::ASTNode*>& b)
    {
        if(a.size() != b.size())
        {
            return false;
        }

        for(size_t i = 0; i != a.size(); ++i)
        {
            tree::ASTNode* na = a[i];
            tree::ASTNode* nb = b[i];
            if (!na->isEqual(*nb))
            {
                return false;
            }
        }
        return true;
    }

    void findInstanciatedVersion(tree::TemplatedIdentifier* id, std::vector<std::pair<size_t, tree::ClassDecl*>>& matched_candidates)
    {
        foreach(i, resolution_visitor.candidates)
        {
            tree::ClassDecl* class_decl = tree::cast<tree::ClassDecl>(*i);
            BOOST_ASSERT_MSG(class_decl, "Declaraion is not class");
            tree::TemplatedIdentifier* templated_id = tree::cast<tree::TemplatedIdentifier>(class_decl->name);
            BOOST_ASSERT_MSG(templated_id, "Declaration is not class template");

            if(id->templated_type_list.size() == templated_id->templated_type_list.size())
            {
                size_t score = 0L;
                bool matched = true;
                for(auto i = id->templated_type_list.begin(), j = templated_id->templated_type_list.begin(); i != id->templated_type_list.end(); ++i, ++j)
                {
                    if(j->specialized_type && j->specialized_type->type == tree::TypeSpecifier::ReferredType::UNSPECIFIED)
                    {
                    	if(i->id->isEqual(*j->specialized_type->referred.unspecified))
                    		score += 0; // exact match to a specialized version, score zero
                    	else
                    	{
                    		matched = false;
                    		break;
                    	}
                    }
                    else
                    {
                    	score += 1; // match non-specialized version, score one
                    }
                }
                matched_candidates.push_back(std::make_pair(score, class_decl));
            }
        }
    }

    bool findBestClassTemplate(const std::vector<tree::ASTNode*>& candidates,
                               const std::vector<tree::ASTNode*>& templateArgs,
                               tree::ClassDecl*& bestMatchClassTemplate)
    {
        std::vector<size_t> scores;
        foreach(i, candidates)
        {
            tree::ClassDecl* classDecl = tree::cast<tree::ClassDecl>(*i);
            BOOST_ASSERT_MSG(classDecl, "Declaraion is not class");
            tree::TemplatedIdentifier* templatedId = tree::cast<tree::TemplatedIdentifier>(classDecl->name);
            BOOST_ASSERT_MSG(templatedId, "Declaration is not class template");

            //scores.push_back(classTemplateMatchScore(templatedId->templated_type_list, templateArgs));
        }
        return false;
    }

    bool isMoreSpecific(tree::TypenameDecl* a, tree::TypenameDecl* b)
    {
    	// is a not specialized, it is impossible a is more specific than b
        if(a->specialized_type == NULL)
        {
        	return false;
        }

        // a is specialized, b not, a is more specific than b
        if(a->specialized_type != NULL && b->specialized_type == NULL)
        {
        	return true;
        }

        // if a and b are both specialized, compare the content of the specialized type recursively.
        //return isMoreSpecific(language::ResolvedType::get(a), language::ResolvedType::get(b));
        // sdk go!!
    }

    bool isMoreSpecific(tree::ClassDecl* a, tree::ClassDecl* b)
    {
    	tree::TemplatedIdentifier* aArgs = tree::cast<tree::TemplatedIdentifier>(a->name);
    	tree::TemplatedIdentifier* bArgs = tree::cast<tree::TemplatedIdentifier>(b->name);
    	// foreach template argument,
		bool hasAnyBetter = false;
    	for(size_t i = 0; i < aArgs->template_type_list.size(); ++i){
    		// TODO check a b is class template
    		//if b's template argument is better(more specific) than a's, return false

    		if(isMoreSpecific(bArgs->template_type_list[i], aArgs->template_type_list[i]))
    		{
    			return false;
    		}
    		else if(isMoreSpecific(aArgs->template_type_list[i], bArgs->template_type_list[i]))
    		{
    			hasAnyBetter = true;
    		}
    	}
    	return hasAnyBetter;
    }

    // TODO optimize performance
    tree::ClassDecl* selectBestCandidate()
    {
        std::vector<ASTNode*> candidates = resolution_visitor.candidates;
        foreach(i, candidates)
        {
        	bool isMoreSpecificThanOthers = true;
        	foreach(j, candidates)
			{
        		if(!isMoreSpecific(i, j))
        		{
        			isMoreSpecificThanOthers = false;
        			break;
        		}
			}
        	if(isMoreSpecificThanOthers)
        	{
        		return &*i; // get the address of the dereferrenced object of the iterator i
        	}
        }
        return NULL;
    }

    bool tryInstantiateClassTemplate(tree::ASTNode& attach, zillians::language::tree::TypeSpecifier& node, bool no_action)
    {
        if(!isAllCandidatesAreClassTemplate())
            return false;
        }

        // get template type argument list
        //const std::vector<tree::TypenameDecl*>& templateArgsList = tree::cast<tree::TemplatedIdentifier>(node.referred.unspecified)->templated_type_list;

        // if instanciated version exists, use that version
        tree::ClassDecl* instantiatedClass = findInstanciatedVersion(tree::cast<tree::TemplatedIdentifier>(node.referred.unspecified));
        if (instantiatedClass)
        {
            ResolvedType::set(&attach, instantiatedClass);
            return true;
        }
        // else, select the best candidate
        else
        {
            tree::ClassDecl* best = selectBestCandidate();
            //tree::ClassDecl* bestMatchClassDecl = nullptr;
            //bool onlyOneBest = findBestClassTemplate(candidates, templateArgsList, bestMatchClassDecl);
            //transforms.push_back([=](){

            //});
        }



        return true;
    }

private:
	bool checkResolvedType(tree::ASTNode& attach, tree::TypeSpecifier& node, bool no_action)
	{
		using namespace zillians::language::tree;

        // class template
        if(tryInstantiateClassTemplate(attach, node, no_action))
        {
            return true;
        }
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

			for(__gnu_cxx::hash_set<ASTNode*>::const_iterator scope = current_scopes.begin(); scope != current_scopes.end(); ++scope)
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

private:
	__gnu_cxx::hash_set<tree::ASTNode*> current_scopes;
	tree::visitor::ResolutionVisitor resolution_visitor;
	std::vector<std::function<void()>> transforms;
};

} }

#endif /* ZILLIANS_LANGUAGE_RESOLVER_H_ */
