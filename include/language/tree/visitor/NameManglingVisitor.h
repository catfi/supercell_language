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
#include "language/tree/visitor/GenericVisitor.h"
#include "language/tree/basic/PrimitiveType.h"
#include "language/tree/ASTNodeHelper.h"
#include <ctype.h>

namespace zillians { namespace language { namespace tree { namespace visitor {

struct NameManglingVisitor : Visitor<ASTNode, void, VisitorImplementation::recursive_dfs>
{
	CREATE_INVOKER(mangleInvoker, mangle)

	NameManglingVisitor() : scoped(false)
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

		stream << "[" << __LINE__ << "]" << encode(node.toString());
	}

	void mangle(Package& node)
	{
		if(!ASTNodeHelper::isRootPackage(&node))
			scoped = true;
		if(node.parent) visit(*node.parent);

		if(ASTNodeHelper::isRootPackage(&node))
		{
			stream << "_Z"; // always begin with "_Z"
			if(scoped)
				stream << "N"; // name involves a named scope
		}
		else
			stream << node.id->toString().length() << "[" << __LINE__ << "]" << encode(node.id->toString());
	}

	void mangle(Block& node)
	{
		if(node.parent) visit(*node.parent);

		stream << "_B_";
	}

	void mangle(TypeSpecifier& node)
	{
//		if(node.parent) visit(*node.parent);

		switch(node.type)
		{
		case TypeSpecifier::ReferredType::FUNCTION_TYPE:
			visit(*node.referred.function_type);
			break;
		case TypeSpecifier::ReferredType::PRIMITIVE:
			switch(node.referred.primitive)
			{
			case PrimitiveType::VOID:    stream << "v"; break;
			case PrimitiveType::BOOL:    stream << "b"; break;
			case PrimitiveType::INT8:    stream << "c"; break;
			case PrimitiveType::INT16:   stream << "s"; break;
			case PrimitiveType::INT32:   stream << "l"; /* or 'i' ? */ break;
			case PrimitiveType::INT64:   stream << "x"; break;
			case PrimitiveType::FLOAT32: stream << "f"; break;
			case PrimitiveType::FLOAT64: stream << "d"; break;
			default: UNREACHABLE_CODE();
			}
			break;
		case TypeSpecifier::ReferredType::UNSPECIFIED:
			{
	            ASTNode* resolved_type = ResolvedType::get(node.referred.unspecified);
	            //BOOST_ASSERT(resolved_type && "shouldn't be NULL");
	            if(resolved_type)
	            {
					if(isa<TypeSpecifier>(resolved_type))
					{
						TypeSpecifier* type_specifier = cast<TypeSpecifier>(resolved_type);
	                	visit(*type_specifier);
					}
					else if(isa<ClassDecl>(resolved_type))
					{
						ClassDecl* decl = cast<ClassDecl>(resolved_type);
						visit(*decl);
					}
					else
						UNREACHABLE_CODE();
	            }
			}
			stream << node.referred.unspecified->toString().length() << "[" << __LINE__ << "]" << encode(node.referred.unspecified->toString());
			break;
		}
	}

	void mangle(ClassDecl& node)
	{
		scoped = true;
		if(node.parent) visit(*node.parent);

		if(isa<TemplatedIdentifier>(node.name))
		{
			TemplatedIdentifier* templated_identifier = cast<TemplatedIdentifier>(node.name);
			if(templated_identifier->toString() == L"ptr_")
			{
				stream << "P";
			}
			else if(templated_identifier->toString() == L"ref_")
			{
				stream << "R";
			}
			else if(templated_identifier->toString() == L"const_")
			{
				stream << "C";
			}
		}
		else
			stream << node.name->toString().length() << "[" << __LINE__ << "]" << encode(node.name->toString());
	}

	void mangle(InterfaceDecl& node)
	{
		if(node.parent) visit(*node.parent);

		stream << "[" << __LINE__ << "]" << encode(node.name->toString());
	}

	void mangle(EnumDecl& node)
	{
		if(node.parent) visit(*node.parent);

		stream << "[" << __LINE__ << "]" << encode(node.name->toString());
	}

	void mangle(FunctionDecl& node)
	{
		if(isa<ClassDecl>(node.parent))
			return; // bug work-around

		if(node.parent) visit(*node.parent);

		stream << node.name->toString().length() << "[" << __LINE__ << "]" << encode(node.name->toString());
		std::vector<VariableDecl*>::iterator p = node.parameters.begin();
		if(ASTNodeHelper::hasOwnerNamedScope(&node))
		{
			stream << "E"; // if mangled name begins with "N", end with "E"
			if(isa<ClassDecl>(node.parent))
			{
				BOOST_ASSERT(node.parameters.size() >= 1 && "methods must have 1st parameter \"this\"");
				p++; // skip "this" parameter for methods
			}
		}
		if(p == node.parameters.end())
			stream << "v"; // empty param list equivalent to "void" param type
		else
		{
			for(; p != node.parameters.end(); p++)
                visit(*(*p)->type);
		}
	}

	void reset()
	{
		std::cout << "NameManglingVisitor::visit(): " << stream.str() << std::endl;
		stream.str("");
		scoped = false;
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
			if(c == '<')
				break;
			if(i == ucs4_to_utf8_temp.begin())
			{
				if( isalpha(c) || (c == '_') || (c == '.') )
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
				if( isalpha(c) || isdigit(c) || (c == '_') || (c == '.') )
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

	bool scoped;
};

} } } }

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_GENERAL_NAMEMANGLINGVISITOR_H_ */
