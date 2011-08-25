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

#ifndef ZILLIANS_LANGUAGE_TREE_VISITOR_GENERAL_NODEINFOVISITOR_H_
#define ZILLIANS_LANGUAGE_TREE_VISITOR_GENERAL_NODEINFOVISITOR_H_

#include "core/Prerequisite.h"
#include "core/Visitor.h"
#include "language/tree/visitor/general/GenericVisitor.h"

namespace zillians { namespace language { namespace tree { namespace visitor {

struct NodeInfoVisitor : Visitor<ASTNode, void, VisitorImplementation::recursive_dfs>
{
	CREATE_INVOKER(infoInvoker, info)

	NodeInfoVisitor()
	{
		REGISTER_ALL_VISITABLE_ASTNODE(infoInvoker)
	}

	void info(ASTNode& node)
	{
		if(node.parent) visit(*node.parent);
	}

	void info(Identifier& node)
	{
		if(node.parent) visit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << node.toString();
	}

	void info(Package& node)
	{
		if(node.parent) visit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << node.id->toString();
	}

	void info(TypeSpecifier& node)
	{
		if(node.parent) visit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		switch(node.type)
		{
		case TypeSpecifier::ReferredType::CLASS_DECL:
			visit(*node.referred.class_decl);
			break;
		case TypeSpecifier::ReferredType::INTERFACE_DECL:
			visit(*node.referred.interface_decl);
			break;
		case TypeSpecifier::ReferredType::FUNCTION_DECL:
			visit(*node.referred.function_decl);
			break;
		case TypeSpecifier::ReferredType::ENUM_DECL:
			visit(*node.referred.enum_decl);
			break;
		case TypeSpecifier::ReferredType::TYPEDEF_DECL:
			visit(*node.referred.typedef_decl);
			break;
		case TypeSpecifier::ReferredType::FUNCTION_TYPE:
			visit(*node.referred.function_type);
			break;
		case TypeSpecifier::ReferredType::PRIMITIVE:
			stream << TypeSpecifier::PrimitiveType::toString(node.referred.primitive);
			break;
		case TypeSpecifier::ReferredType::UNSPECIFIED:
			stream << node.referred.unspecified->toString();
		}
		//stream << L" [" << TypeSpecifier::ReferredType::toString(node.type) << L"]";
	}

	void info(ClassDecl& node)
	{
		if(node.parent) visit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << node.name->toString();
	}

	void info(InterfaceDecl& node)
	{
		if(node.parent) visit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << node.name->toString();
	}

	void info(EnumDecl& node)
	{
		if(node.parent) visit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << node.name->toString();
	}

	void info(FunctionDecl& node)
	{
		if(node.parent) visit(*node.parent);

		if(stream.str().length() > 0)
			stream << L".";

		stream << node.name->toString();
	}

	void reset()
	{
		stream.clear();
	}

	std::wstringstream stream;
};

} } } }

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_GENERAL_NODEINFOVISITOR_H_ */
