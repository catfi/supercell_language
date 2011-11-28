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
 * @date Aug 5, 2011 sdk - Initial version created.
 */

#ifndef ZILLIANS_LANGUAGE_TREE_IDENTIFIER_H_
#define ZILLIANS_LANGUAGE_TREE_IDENTIFIER_H_

#include "language/tree/ASTNode.h"
#include "utility/Foreach.h"

namespace zillians { namespace language { namespace tree {

struct Identifier : public ASTNode
{
	friend class boost::serialization::access;

	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(Identifier, (Identifier)(ASTNode));

	Identifier()
	{ }

	virtual std::wstring toString() const = 0;
	virtual bool isEmpty() const = 0;

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & boost::serialization::base_object<ASTNode>(*this);
    }
};

struct SimpleIdentifier : public Identifier
{
	friend class boost::serialization::access;

	DEFINE_VISITABLE();
	DEFINE_HIERARCHY(SimpleIdentifier, (SimpleIdentifier)(Identifier)(ASTNode));

	explicit SimpleIdentifier(const std::wstring& s);

	virtual std::wstring toString() const;
	virtual bool isEmpty() const;

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const;
    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true);
    virtual ASTNode* clone() const;

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & boost::serialization::base_object<Identifier>(*this);
    	ar & name;
    }

	std::wstring name;

protected:
	SimpleIdentifier();
};

struct NestedIdentifier : public Identifier
{
	friend class boost::serialization::access;

	DEFINE_VISITABLE()
	DEFINE_HIERARCHY(NestedIdentifier, (NestedIdentifier)(Identifier)(ASTNode));

	NestedIdentifier();

	virtual std::wstring toString() const;
	virtual bool isEmpty() const;

	void appendIdentifier(Identifier* id);

    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const;
    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true);

	virtual ASTNode* clone() const;

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & boost::serialization::base_object<Identifier>(*this);
    	ar & identifier_list;
    }

	std::vector<Identifier*> identifier_list;
};

struct TemplateType
{
	friend class boost::serialization::access;

	TemplateType(SimpleIdentifier* id, TypeSpecifier* specialized_type = NULL, ASTNode* default_type = NULL);

	TemplateType& operator= (const TemplateType& s);

	bool isEqualImpl(const TemplateType& rhs, ASTNodeSet& visited) const;
    bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true);
    std::wstring toString() const;

    TemplateType clone() const;

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & id;
    	ar & specialized_type;
    	ar & default_type;
    }

	SimpleIdentifier* id;
	TypeSpecifier* specialized_type;
	ASTNode* default_type;

protected:
	TemplateType() { }
};

struct TemplatedIdentifier : public Identifier
{
	friend class boost::serialization::access;

	DEFINE_VISITABLE()
	DEFINE_HIERARCHY(TemplatedIdentifier, (TemplatedIdentifier)(Identifier)(ASTNode));

	struct Usage
	{
		enum type {
			FORMAL_PARAMETER,
			ACTUAL_ARGUMENT,
		};

		static const wchar_t* toString(type t)
		{
			switch(t)
			{
			case FORMAL_PARAMETER: return L"param";
			case ACTUAL_ARGUMENT: return L"arg";
			}
		}
	};

	explicit TemplatedIdentifier(Usage::type type, Identifier* id);
	virtual std::wstring toString() const;

	virtual bool isEmpty() const;
	void setIdentifier(Identifier* identifier);

	void append(TemplateType type);
    virtual bool isEqualImpl(const ASTNode& rhs, ASTNodeSet& visited) const;
    virtual bool replaceUseWith(const ASTNode& from, const ASTNode& to, bool update_parent = true);
	virtual ASTNode* clone() const;

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    	UNUSED_ARGUMENT(version);

    	ar & boost::serialization::base_object<Identifier>(*this);
    	ar & type;
    	ar & id;
    	ar & templated_type_list;
    }

	Usage::type type;
	Identifier* id;
	std::vector<TemplateType> templated_type_list;

protected:
	TemplatedIdentifier();
};

} } }

#endif /* ZILLIANS_LANGUAGE_TREE_IDENTIFIER_H_ */
