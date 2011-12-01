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
#include <algorithm>

namespace zillians { namespace language { namespace tree { namespace visitor {

struct NameManglingVisitor : Visitor<ASTNode, void, VisitorImplementation::recursive_dfs>
{
	CREATE_INVOKER(mangleInvoker, mangle)

	NameManglingVisitor() : mUptraceDepth(0), mComboName(false), mDependentComponent(false), mCtorMode(CTOR_NONE)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(mangleInvoker)
	}

	void mangle(ASTNode& node)
	{ }

	void mangle(Package& node)
	{
		bool is_root = ASTNodeHelper::isRootPackage(&node);
		mComboName = (is_root && mUptraceDepth>1);
		if(node.parent)
		{
			mUptraceDepth++;
			visit(*node.parent); // up-trace to build complete name
			mUptraceDepth = 0;
		}
		if(is_root)
		{
			if(!mDependentComponent)
				stream << "_Z"; // always begin with "_Z"
			if(mComboName)
				stream << "N"; // name involves a Package/ClassDecl
		}
		else
			visit(*node.id);
	}

	void mangle(SimpleIdentifier& node)
	{
		if(node.name == L"ptr_")        stream << "P";
		else if(node.name == L"ref_")   stream << "R";
		else if(node.name == L"const_") stream << "K";
		else if(node.name == L"void_")  stream << "v";
		else if(isConstructor(&node))
		{
			switch(mCtorMode)
			{
			case CTOR_1: stream << "C1"; break;
			case CTOR_2: stream << "C2"; break;
			default: break;
			}
		}
		else
			stream << node.name.length() << encode(node.name);
	}

	void mangle(TemplatedIdentifier& node)
	{
		visit(*node.id);
		foreach(i, node.templated_type_list)
            resolveAndVisit(*i);
	}

	void mangle(TypeSpecifier& node)
	{
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
			resolveAndVisit(&node);
			break;
		}
	}

	void mangle(FunctionType& node)
	{
		if(node.parent)
		{
			mUptraceDepth++;
			visit(*node.parent); // up-trace to build complete name
			mUptraceDepth = 0;
		}
		stream << "PF"; // function pointer is always a pointer, hence "P" in "PF"
		resolveAndVisit(node.return_type);
		std::vector<TypeSpecifier*> type_list;
		foreach(i, node.argument_types)
		{
			int type_index = -1;
			if((*i)->type != TypeSpecifier::ReferredType::PRIMITIVE)
			{
				auto p = std::find_if(type_list.begin(), type_list.end(),
						std::bind2nd(std::ptr_fun(isEqual), *i));
				if(p != type_list.end())
					type_index = std::distance(type_list.begin(), p);
				type_list.push_back(*i);
			}
			if(type_index != -1)
				stream << "S" << type_index << "_";
			else
				resolveAndVisit(*i);
		}
		stream << "E"; // always end with "E"
	}

	void mangle(ClassDecl& node)
	{
		if(node.parent)
		{
			mUptraceDepth++;
			visit(*node.parent); // up-trace to build complete name
			mUptraceDepth = 0;
		}
		visitIdentifier(node.name);
	}

	void mangle(InterfaceDecl& node)
	{
		if(node.parent)
		{
			mUptraceDepth++;
			visit(*node.parent); // up-trace to build complete name
			mUptraceDepth = 0;
		}
		visitIdentifier(node.name);
	}

	void mangle(EnumDecl& node)
	{
		if(node.parent)
		{
			mUptraceDepth++;
			visit(*node.parent); // up-trace to build complete name
			mUptraceDepth = 0;
		}
		visitIdentifier(node.name);
	}

	void mangle(FunctionDecl& node)
	{
		if(isConstructor(node.name) && mCtorMode == CTOR_NONE)
		{
			mCtorMode = CTOR_1;
			visit(node);
#if 0 // NOTE: unused -- see http://stackoverflow.com/questions/6921295/dual-emission-of-constructor-symbols
			mCtorMode = CTOR_2;
			visit(node);
#endif
			mCtorMode = CTOR_NONE;
			return;
		}
		mComboName = false;
		if(node.parent)
		{
			mUptraceDepth++;
			visit(*node.parent); // up-trace to build complete name
			mUptraceDepth = 0;
		}
		visitIdentifier(node.name);
		if(mComboName)
			stream << "E"; // if mangled name begins with "N", always end with "E"
		std::vector<TypeSpecifier*> type_list;
		std::vector<VariableDecl*>::iterator p = node.parameters.begin();
		if(ASTNodeHelper::hasOwnerNamedScope(&node))
		{
			if(isa<ClassDecl>(node.parent))
			{
				BOOST_ASSERT(node.parameters.size() >= 1 && "methods must have 1st parameter \"this\"");
				type_list.push_back((*p)->type);
				p++; // skip "this" parameter for methods
			}
		}
		if(p == node.parameters.end())
			stream << "v"; // empty param-list equivalent to "void" param type
		else
		{
			for(; p != node.parameters.end(); p++)
			{
				int type_index = -1;
				if((*p)->type->type != TypeSpecifier::ReferredType::PRIMITIVE)
				{
					auto q = std::find_if(type_list.begin(), type_list.end(),
							std::bind2nd(std::ptr_fun(isEqual), (*p)->type));
					if(q != type_list.end())
						type_index = std::distance(type_list.begin(), q);
					type_list.push_back((*p)->type);
				}
				if(type_index != -1)
				{
					if(node.is_member && node.parent)
						type_index--; // start counting from zero after "this" parameter
					if(type_index == -1)
						stream << "S_"; // parameter is same type as "this" parameter
					else
						stream << "S" << type_index << "_";
				}
				else
					resolveAndVisit((*p)->type);
			}
		}
	}

	std::string ucs4_to_utf8_temp;
	std::string utf8_to_llvm_temp;

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
#if 1 // NOTE: truncate llvm-rewrite for now -- not useful
			if(c == '<') break;
#endif
			if( ((i == ucs4_to_utf8_temp.begin()) ? false : isdigit(c)) || isalpha(c) || (c == '_') || (c == '.') )
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

		return utf8_to_llvm_temp;
	}

	void reset()
	{
#if 0 // NOTE: for debugging only
		std::cout << "NameManglingVisitor: " << stream.str() << std::endl;
#endif
		stream.str("");
		mUptraceDepth = 0;
		mComboName = false;
		mDependentComponent = false;
		mCtorMode = CTOR_NONE;
	}

	std::stringstream stream;

private:
	static const char* toAsciiNumber(char c)
	{
		static char buffer[4];
		snprintf(buffer, 3, "%d", c);
		return buffer;
	}

	void resolveAndVisit(ASTNode* node)
	{
        ASTNode* resolved_type = ASTNodeHelper::findUniqueTypeResolution(node);
        if(resolved_type)
        {
			mDependentComponent = true;
			if(isa<TypeSpecifier>(resolved_type))
				visit(*cast<TypeSpecifier>(resolved_type));
			else if(isa<ClassDecl>(resolved_type))
				visit(*cast<ClassDecl>(resolved_type));
			else if(isa<InterfaceDecl>(resolved_type))
				visit(*cast<InterfaceDecl>(resolved_type));
			else if(isa<EnumDecl>(resolved_type))
				visit(*cast<EnumDecl>(resolved_type));
			else
				UNREACHABLE_CODE();
			mDependentComponent = false;
        }
	}

	void visitIdentifier(Identifier* ident)
	{
		if(isa<SimpleIdentifier>(ident))
			visit(*cast<SimpleIdentifier>(ident));
		else if(isa<TemplatedIdentifier>(ident))
			visit(*cast<TemplatedIdentifier>(ident));
		else
			UNREACHABLE_CODE();
	}

	static bool isConstructor(Identifier* ident)
	{
		if(isa<SimpleIdentifier>(ident))
			return (cast<SimpleIdentifier>(ident)->name == L"new");
		else if(isa<TemplatedIdentifier>(ident))
			return isConstructor(cast<TemplatedIdentifier>(ident)->id);
		else
			UNREACHABLE_CODE();
		return false;
	}

	static bool isEqual(TypeSpecifier* a, TypeSpecifier* b)
	{
		return a->isEqual(*b);
	}

	size_t mUptraceDepth;
	bool mComboName;
	bool mDependentComponent;

	enum CTOR_MODE
	{
		CTOR_NONE,
		CTOR_1,
		CTOR_2
	};
	CTOR_MODE mCtorMode;
};

} } } }

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_GENERAL_NAMEMANGLINGVISITOR_H_ */
