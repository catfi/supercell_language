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

#ifndef ZILLIANS_LANGUAGE_TREE_VISITOR_GENERAL_NAMEMANGLINGVISITOR_H_
#define ZILLIANS_LANGUAGE_TREE_VISITOR_GENERAL_NAMEMANGLINGVISITOR_H_

#include "core/Prerequisite.h"
#include "utility/UnicodeUtil.h"
#include "core/Visitor.h"
#include "language/tree/visitor/general/GenericVisitor.h"

namespace zillians { namespace language { namespace tree { namespace visitor {

struct NameManglingVisitor : Visitor<ASTNode, void, VisitorImplementation::recursive_dfs>
{
	CREATE_INVOKER(mangleInvoker, mangle)

	NameManglingVisitor()
	{
		REGISTER_ALL_VISITABLE_ASTNODE(mangleInvoker)
	}

	void mangle(ASTNode& node)
	{
		if(node.parent) visit(*node.parent);
	}

	void mangle(Identifier& node)
	{
		if(node.parent) visit(*node.parent);

		if(stream.str().length() > 0)
			stream << ".";

		stream << encode(node.toString());
	}

	void mangle(Package& node)
	{
		if(node.parent) visit(*node.parent);

		if(stream.str().length() > 0)
			stream << ".";

		if(node.id->isEmpty())
			stream << "root";
		else
			stream << encode(node.id->toString());
	}

	void mangle(Block& node)
	{
		if(node.parent) visit(*node.parent);

		if(stream.str().length() > 0)
			stream << ".";

		stream << "_B_";
	}

	void mangle(TypeSpecifier& node)
	{
		if(node.parent) visit(*node.parent);

		if(stream.str().length() > 0)
			stream << ".";

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
			stream << encode(node.referred.unspecified->toString());
			break;
		}
	}

	void mangle(ClassDecl& node)
	{
		if(node.parent) visit(*node.parent);

		if(stream.str().length() > 0)
			stream << ".";

		stream << encode(node.name->toString());
	}

	void mangle(InterfaceDecl& node)
	{
		if(node.parent) visit(*node.parent);

		if(stream.str().length() > 0)
			stream << ".";

		stream << encode(node.name->toString());
	}

	void mangle(EnumDecl& node)
	{
		if(node.parent) visit(*node.parent);

		if(stream.str().length() > 0)
			stream << ".";

		stream << encode(node.name->toString());
	}

	void mangle(FunctionDecl& node)
	{
		if(node.parent) visit(*node.parent);

		if(stream.str().length() > 0)
			stream << ".";

		stream << encode(node.name->toString());
	}

	void reset()
	{
		stream.str("");
	}

	const std::string& encode(const std::wstring& ucs4)
	{
		ucs4_to_utf8_temp.clear();
		utf8_to_llvm_temp.clear();

		// first we should covert UCS-4 to UTF-8
		ucs4_to_utf8(ucs4, ucs4_to_utf8_temp);

		// because LLVM only accept identifier of the form: '[%@][a-zA-Z$._][a-zA-Z$._0-9]*'
		// we have to convert illegal identifier into legal one
		for(std::string::const_iterator i = ucs4_to_utf8_temp.begin(), e = ucs4_to_utf8_temp.end(); i != e; ++i)
		{
			char c = *i;
			if(i == ucs4_to_utf8_temp.begin())
			{
				if( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_') || (c == '.') )
				{
					utf8_to_llvm_temp.push_back(c);
				}
				else
				{
					utf8_to_llvm_temp.push_back('$');
					utf8_to_llvm_temp.append(toAsciiNumber(c));
					utf8_to_llvm_temp.push_back('$');
				}
			}
			else
			{
				if( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c == '_') || (c == '.') )
				{
					utf8_to_llvm_temp.push_back(c);
				}
				else
				{
					utf8_to_llvm_temp.push_back('$');
					utf8_to_llvm_temp.append(toAsciiNumber(c));
					utf8_to_llvm_temp.push_back('$');
				}
			}
		}

		return utf8_to_llvm_temp;
	}

	inline const char* toAsciiNumber(char c)
	{
		static char buffer[4];
		snprintf(buffer, 3, "%d", c);
		return buffer;
	}

	std::string ucs4_to_utf8_temp;
	std::string utf8_to_llvm_temp;

	std::stringstream stream;
};

} } } }

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_GENERAL_NAMEMANGLINGVISITOR_H_ */
