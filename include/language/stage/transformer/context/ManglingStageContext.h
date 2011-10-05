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

#ifndef ZILLIANS_LANGUAGE_STAGE_CONTEXT_MANGLINGSTAGECONTEXT_H_
#define ZILLIANS_LANGUAGE_STAGE_CONTEXT_MANGLINGSTAGECONTEXT_H_

#include "language/tree/ASTNodeFactory.h"

namespace zillians { namespace language { namespace stage {

// this context will be generated and attached to ASTNode if necessary
struct NameManglingContext
{
	friend class boost::serialization::access;

	explicit NameManglingContext(const std::string& name) : managled_name(name)
	{ }

	static NameManglingContext* get(tree::ASTNode* node)
	{
		return node->get<NameManglingContext>();
	}

	static void set(tree::ASTNode* node, NameManglingContext* ctx)
	{
		node->set<NameManglingContext>(ctx);
	}

    template<typename Archive>
    void serialize(Archive& ar, unsigned int version)
    {
    	ar & managled_name;
    }

	std::string managled_name;

private:
	NameManglingContext() { }
};

struct TypeIdManglingContext
{
	friend class boost::serialization::access;

	explicit TypeIdManglingContext(uint64 id) : managled_id(id)
	{ }

	static TypeIdManglingContext* get(tree::ASTNode* node)
	{
		return node->get<TypeIdManglingContext>();
	}

	static void set(tree::ASTNode* node, TypeIdManglingContext* ctx)
	{
		node->set<TypeIdManglingContext>(ctx);
	}

    template<typename Archive>
    void serialize(Archive& ar, unsigned int version)
    {
    	ar & managled_id;
    }

	uint64 managled_id;

private:
	TypeIdManglingContext() { }
};

struct SymbolIdManglingContext
{
	friend class boost::serialization::access;

	explicit SymbolIdManglingContext(uint64 id) : managled_id(id)
	{ }

	static SymbolIdManglingContext* get(tree::ASTNode* node)
	{
		return node->get<SymbolIdManglingContext>();
	}

	static void set(tree::ASTNode* node, SymbolIdManglingContext* ctx)
	{
		node->set<SymbolIdManglingContext>(ctx);
	}

    template<typename Archive>
    void serialize(Archive& ar, unsigned int version)
    {
    	ar & managled_id;
    }

	uint64 managled_id;

private:
	SymbolIdManglingContext() { }
};

} } }

#endif /* ZILLIANS_LANGUAGE_STAGE_CONTEXT_MANGLINGSTAGECONTEXT_H_ */
