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

//    bool isAllCandidatesAreClassTemplate()
//    {
//        foreach(i, resolution_visitor.candidates)
//        {
//            if(!tree::isa<tree::ClassDecl>(*i)) return false;
//            if(!tree::isa<tree::TemplatedIdentifier>(tree::cast<tree::ClassDecl>(*i)->name)) return false;
//        }
//        return true;
//    }
//
//    bool templateArgumentTypeListIsEqual(const std::vector<tree::ASTNode*>& a, const std::vector<tree::ASTNode*>& b)
//    {
//        if(a.size() != b.size())
//        {
//            return false;
//        }
//
//        for(size_t i = 0; i != a.size(); ++i)
//        {
//            tree::ASTNode* na = a[i];
//            tree::ASTNode* nb = b[i];
//            if (!na->isEqual(*nb))
//            {
//                return false;
//            }
//        }
//        return true;
//    }

//    void findInstanciatedVersion(tree::TemplatedIdentifier* id, std::vector<std::pair<size_t, tree::ClassDecl*>>& matched_candidates)
//    {
//        foreach(i, resolution_visitor.candidates)
//        {
//            tree::ClassDecl* class_decl = tree::cast<tree::ClassDecl>(*i);
//            BOOST_ASSERT_MSG(class_decl, "Declaraion is not class");
//            tree::TemplatedIdentifier* templated_id = tree::cast<tree::TemplatedIdentifier>(class_decl->name);
//            BOOST_ASSERT_MSG(templated_id, "Declaration is not class template");
//
//            if(id->templated_type_list.size() == templated_id->templated_type_list.size())
//            {
//                size_t score = 0L;
//                bool matched = true;
//                for(auto i = id->templated_type_list.begin(), j = templated_id->templated_type_list.begin(); i != id->templated_type_list.end(); ++i, ++j)
//                {
//                    if(j->specialized_type && j->specialized_type->type == tree::TypeSpecifier::ReferredType::UNSPECIFIED)
//                    {
//                    	if(i->id->isEqual(*j->specialized_type->referred.unspecified))
//                    		score += 0; // exact match to a specialized version, score zero
//                    	else
//                    	{
//                    		matched = false;
//                    		break;
//                    	}
//                    }
//                    else
//                    {
//                    	score += 1; // match non-specialized version, score one
//                    }
//                }
//                matched_candidates.push_back(std::make_pair(score, class_decl));
//            }
//        }
//    }

//    bool findBestClassTemplate(const std::vector<tree::ASTNode*>& candidates,
//                               const std::vector<tree::ASTNode*>& templateArgs,
//                               tree::ClassDecl*& bestMatchClassTemplate)
//    {
//        std::vector<size_t> scores;
//        foreach(i, candidates)
//        {
//            tree::ClassDecl* classDecl = tree::cast<tree::ClassDecl>(*i);
//            BOOST_ASSERT_MSG(classDecl, "Declaraion is not class");
//            tree::TemplatedIdentifier* templatedId = tree::cast<tree::TemplatedIdentifier>(classDecl->name);
//            BOOST_ASSERT_MSG(templatedId, "Declaration is not class template");
//
//            //scores.push_back(classTemplateMatchScore(templatedId->templated_type_list, templateArgs));
//        }
//        return false;
//    }

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
    					BOOST_ASSERT(a->specialized_type->referred.unspecified->isEqual(*b->specialized_type->referred.unspecified));
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

    static bool isEquallyOrMoreSpecialized(tree::TemplatedIdentifier* a, tree::TemplatedIdentifier* b)
    {
    	for(size_t i = 0; i < a->templated_type_list.size(); ++i)
    	{
    		if(!isEquallyOrMoreSpecialized(a->templated_type_list[i], b->templated_type_list[i]))
    			return false;
    	}
    	return true;
    }

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
     * try to find the most specific class template and report error if there's ambiguous resolution
     * @return
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

    	if(most_specialized_class_templates.size() == 1)
    	{
    		// we got the only one, most specific class template, that's all we need
    		return most_specialized_class_templates[0];
    	}
    	else if(most_specialized_class_templates.size() > 1)
    	{
    		// TODO log ambiguous template resolution with all class templates in most_specific_class_templates
    		return NULL;
    	}
    	else // no matched
    	{
    		// TODO log ambiguous template resolution with all candidates
    		return NULL;
    	}
    }

    /**
     * try to find and instantiate (if needed) the best class template candidates among all
     * @param attach
     * @param node
     * @param no_action
     * @return
     */
    bool tryInstantiateClassTemplate(tree::ASTNode& attach, zillians::language::tree::TypeSpecifier& node, bool no_action)
    {
    	using namespace zillians::language::tree;

    	// if there's no candidates, of course we cannot find class template to instantiate
    	if(resolution_visitor.candidates.size() == 0)
    		return false;

    	// make sure all candidates are class with templated identifier as its name
    	// so that it does not lead ambiguous types in the end
    	foreach(i, resolution_visitor.candidates)
		{
    		if(!isa<ClassDecl>(*i) || !isa<TemplatedIdentifier>(cast<ClassDecl>(*i)->name))
      			return false;
		}

    	// try to find the most specialized version of class templates
    	// note that as we have filtered out all incompatible class templates, all candidates should be able to fit the current identifier
    	// so we just need to find the most specialized version of class templates among all candidates,
    	// and this version is the one we are looking for
    	ClassDecl* most_specialized_class_template = findMostSpecializedClassTemplate();

    	// if we can't find the most specialized class template due to ambiguous instantiation, return false
    	if(!most_specialized_class_template)
    		return false;

    	// otherwise
    	TemplatedIdentifier* id = cast<TemplatedIdentifier>(most_specialized_class_template->name);
    	if(id->isFullySpecialized())
    	{
    		// we got the fully specialized version, so just set the resolve type
    		ResolvedType::set(&attach, most_specialized_class_template);
    		return true;
    	}
    	else
    	{
    		// TODO the most specialized class template is not fully specialized, we need to create new instantiation using lambda expression for it
    		return true;
    	}
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
