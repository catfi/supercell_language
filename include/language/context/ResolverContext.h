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

#ifndef ZILLIANS_LANGUAGE_RESOLVERCONTEXT_H_
#define ZILLIANS_LANGUAGE_RESOLVERCONTEXT_H_

#include "language/tree/ASTNodeFactory.h"

namespace zillians { namespace language {

struct ResolvedType
{
	friend class boost::serialization::access;

	explicit ResolvedType(tree::ASTNode* ref) : ref(ref)
	{ }

	static bool isValidResolvedType(tree::ASTNode* node)
	{
		using namespace zillians::language::tree;

		if(!isa<ClassDecl>(node) &&
		   !isa<InterfaceDecl>(node) &&
		   !isa<EnumDecl>(node) &&
		   !isa<TypenameDecl>(node) &&
		   !isa<TypedefDecl>(node) &&
		   !isa<FunctionDecl>(node) &&
		   !isa<FunctionType>(node) &&
		   !(isa<TypeSpecifier>(node) && cast<TypeSpecifier>(node)->type != TypeSpecifier::ReferredType::UNSPECIFIED)
		   )
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	static tree::ASTNode* get(tree::ASTNode* node)
	{
		ResolvedType* resolved = node->get<ResolvedType>();
		if(resolved)
			return resolved->ref;
		else
			return NULL;
	}

	static void set(tree::ASTNode* node, tree::ASTNode* ref)
	{
		if(ref)
		{
			BOOST_ASSERT(isValidResolvedType(ref) && "invalid resolved type");
		}

		return node->set<ResolvedType>(new ResolvedType(ref));
	}

    template<typename Archive>
    void serialize(Archive& ar, unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & ref;
    }

	tree::ASTNode* ref;

private:
	ResolvedType() { }
};

struct InstantiatedFrom
{
	friend class boost::serialization::access;

	explicit InstantiatedFrom(tree::ASTNode* ref) : ref(ref)
	{ }

	static bool isValid(tree::ASTNode* node)
	{
		using namespace zillians::language::tree;

        if (!isa<ClassDecl>(node) &&
            !isa<InterfaceDecl>(node) &&
            !isa<FunctionDecl>(node))
        {
            return false;
        }

        TemplatedIdentifier* tid = cast<TemplatedIdentifier>(cast<Declaration>(node)->name);
        if (tid == NULL) return false;
        if (tid->isFullySpecialized()) return false;

        return true;
	}

	static tree::ASTNode* get(tree::ASTNode* node)
	{
		InstantiatedFrom* resolved = node->get<InstantiatedFrom>();
		if(resolved)
			return resolved->ref;
		else
			return NULL;
	}

	static void set(tree::ASTNode* node, tree::ASTNode* ref)
	{
		if(ref)
		{
			BOOST_ASSERT(isValid(ref) && "invalid resolved type");
		}

		return node->set<InstantiatedFrom>(new InstantiatedFrom(ref));
	}

    template<typename Archive>
    void serialize(Archive& ar, unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & ref;
    }

	tree::ASTNode* ref;

private:
	InstantiatedFrom() { }
};

struct SpecializationOf
{
	friend class boost::serialization::access;

	explicit SpecializationOf(tree::ASTNode* ref) : ref(ref)
	{ }

	static bool isValid(tree::ASTNode* node)
	{
		using namespace zillians::language::tree;

        if (!isa<ClassDecl>(node) &&
            !isa<InterfaceDecl>(node) &&
            !isa<FunctionDecl>(node))
        {
            return false;
        }

        TemplatedIdentifier* tid = cast<TemplatedIdentifier>(cast<Declaration>(node)->name);
        if (tid == NULL) return false;
        if (tid->isFullySpecialized()) return false;

        return true;
	}

	static tree::ASTNode* get(tree::ASTNode* node)
	{
		SpecializationOf* resolved = node->get<SpecializationOf>();
		if(resolved)
			return resolved->ref;
		else
			return NULL;
	}

	static void set(tree::ASTNode* node, tree::ASTNode* ref)
	{
		if(ref)
		{
			BOOST_ASSERT(isValid(ref) && "invalid resolved type");
		}

		return node->set<SpecializationOf>(new SpecializationOf(ref));
	}

    template<typename Archive>
    void serialize(Archive& ar, unsigned int version)
    {
        UNUSED_ARGUMENT(version);

        ar & ref;
    }

	tree::ASTNode* ref;

private:
	SpecializationOf() { }
};

struct AmbiguousResolvedType
{
	friend class boost::serialization::access;

	static AmbiguousResolvedType* get(tree::ASTNode* node)
	{
		return node->get<AmbiguousResolvedType>();
	}

	static void set(tree::ASTNode* node, AmbiguousResolvedType* ref)
	{
		return node->set<AmbiguousResolvedType>(ref);
	}

    template<typename Archive>
    void serialize(Archive& ar, unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & refs;
    }

	std::vector<tree::ASTNode*> refs;
};

struct ResolvedSymbol
{
	friend class boost::serialization::access;

	explicit ResolvedSymbol(tree::ASTNode* ref) : ref(ref)
	{ }

	static bool isValidResolvedSymbol(tree::ASTNode* node)
	{
		using namespace zillians::language::tree;

		if(!isa<ClassDecl>(node) &&
		   !isa<EnumDecl>(node) &&
		   !isa<FunctionDecl>(node) &&
		   !isa<VariableDecl>(node)
		   )
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	static tree::ASTNode* get(tree::ASTNode* node)
	{
		ResolvedSymbol* resolved = node->get<ResolvedSymbol>();
		if(resolved)
			return resolved->ref;
		else
			return NULL;
	}

	static void set(tree::ASTNode* node, tree::ASTNode* ref)
	{
		if(ref)
		{
			BOOST_ASSERT(isValidResolvedSymbol(ref) && "invalid resolved symbol");
		}

		return node->set<ResolvedSymbol>(new ResolvedSymbol(ref));
	}

    template<typename Archive>
    void serialize(Archive& ar, unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & ref;
    }

	tree::ASTNode* ref;

private:
	ResolvedSymbol() { }
};

struct AmbiguousResolvedSymbol
{
	friend class boost::serialization::access;

	static AmbiguousResolvedSymbol* get(tree::ASTNode* node)
	{
		return node->get<AmbiguousResolvedSymbol>();
	}

	static void set(tree::ASTNode* node, AmbiguousResolvedSymbol* ref)
	{
		return node->set<AmbiguousResolvedSymbol>(ref);
	}

    template<typename Archive>
    void serialize(Archive& ar, unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & refs;
    }

	std::vector<tree::ASTNode*> refs;
};

struct ResolvedPackage
{
	friend class boost::serialization::access;

	explicit ResolvedPackage(tree::ASTNode* ref) : ref(ref)
	{ }

	static tree::ASTNode* get(tree::ASTNode* node)
	{
		ResolvedPackage* resolved = node->get<ResolvedPackage>();
		if(resolved)
			return resolved->ref;
		else
			return NULL;
	}

	static void set(tree::ASTNode* node, tree::ASTNode* ref)
	{
		return node->set<ResolvedPackage>(new ResolvedPackage(ref));
	}

    template<typename Archive>
    void serialize(Archive& ar, unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & ref;
    }

	tree::ASTNode* ref;

private:
	ResolvedPackage() { }
};

struct AmbiguousResolvedPackage
{
	friend class boost::serialization::access;

	static AmbiguousResolvedPackage* get(tree::ASTNode* node)
	{
		return node->get<AmbiguousResolvedPackage>();
	}

	static void set(tree::ASTNode* node, AmbiguousResolvedPackage* ref)
	{
		return node->set<AmbiguousResolvedPackage>(ref);
	}

    template<typename Archive>
    void serialize(Archive& ar, unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & refs;
    }

	std::vector<tree::ASTNode*> refs;
};

} }

#endif /* ZILLIANS_LANGUAGE_RESOLVERCONTEXT_H_ */
