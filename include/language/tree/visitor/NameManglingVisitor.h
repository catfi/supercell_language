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
#include <iostream>
#include <vector>
#include <string>

namespace zillians { namespace language { namespace tree { namespace visitor {

struct NameManglingVisitor : Visitor<ASTNode, void, VisitorImplementation::recursive_dfs>
{
	CREATE_INVOKER(mangleInvoker, mangle)

	NameManglingVisitor() : mInsideUptrace(false), mInsideComboName(false), mInsideParamList(false)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(mangleInvoker)
	}

	void mangle(ASTNode& node)
	{ }

	void mangle(Package& node)
	{
		if(ASTNodeHelper::isRootPackage(&node))
		{
			if(!mInsideParamList)
				stream << "_Z";
			if(mInsideComboName)
				stream << "N"; // if mangled name is a combo name, prefix "N"
		}
		else
		{
			uptrace(&node);
			visit(*node.id);
		}
	}

	void mangle(SimpleIdentifier& node)
	{
		if(node.name == L"ptr_")        stream << "P";
		else if(node.name == L"ref_")   stream << "R";
		else if(node.name == L"const_") stream << "K";
		else if(node.name == L"void_")  stream << "v";
		else if(node.name == L"new")
		{
			stream << "C1";
#if 0 // NOTE: see --> http://stackoverflow.com/questions/6921295/dual-emission-of-constructor-symbols
			stream << "C2"; // base object constructor
#endif
		}
		else if(!node.name.empty())
			stream << node.name.length() << encode(node.name);
	}

	void mangle(TemplatedIdentifier& node)
	{
		visit(*node.id);

		bool reserved_construct = isReservedConstructName(getPureName(node.id));
		if(!reserved_construct)
			stream << "I";

		{
			mInsideParamList = true;
			foreach(i, node.templated_type_list)
			{
				if(node.type == TemplatedIdentifier::Usage::FORMAL_PARAMETER)
					resolveFirstThenVisit(*i);
			}
			mInsideParamList = false;
		}

		if(!reserved_construct)
			stream << "E"; // ALWAYS postfix "E"
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
			resolveFirstThenVisit(&node);
			break;
		}
	}

	void mangle(FunctionType& node)
	{
		// NOTE: function type occupies 2 substitute names, but only assumes the identity of 2nd substitute
		addToRepeatTypeSet();

		uptrace(&node);
		stream << "PF"; // function pointer is always a pointer, hence "P" in "PF"
		visit(*node.return_type);

		if(node.parameter_types.empty())
			stream << "v"; // empty param-list equivalent to "void" param type
		else
		{
			mInsideParamList = true;
			foreach(i, node.parameter_types)
			{
				if((*i)->type == TypeSpecifier::ReferredType::PRIMITIVE)
					visit(*(*i));
				else
				{
					int type_index = findRepeatTypeSet(*i);
					if(type_index == -1)
					{
//						std::wcout << L"BEFORE visit FunctionType param: " << (*i)->toString() << std::endl;
						visit(*(*i));
//						std::wcout << L"AFTER visit FunctionType param: " << (*i)->toString() << std::endl;
						addToRepeatTypeSet(*i);
					}
					else
						writeSubstitution(type_index);
				}
			}
			mInsideParamList = false;
		}

		stream << "E"; // ALWAYS postfix "E"
	}

	void mangle(Declaration& node)
	{
		uptraceAndAppendName(&node);
	}

	void mangle(FunctionDecl& node)
	{
		clearRepeatTypeSet();
		uptraceAndAppendName(&node);

		auto p = node.parameters.begin();
		if(node.is_member)
		{
			BOOST_ASSERT(node.parent && "method must have parant");
			BOOST_ASSERT((node.parameters.size() >= 1) && "method must have 1st parameter \"this\"");
			addToRepeatTypeSet((*p)->type);
			p++; // no need to mangle "this"
			if(p != node.parameters.end())
			{
				ASTNode* resolved_type = ASTNodeHelper::findUniqueTypeResolution((*p)->type);
				if(resolved_type == node.parent)
				{
					stream << "PS_"; // NOTE: "this" is always a pointer, hence "P" in "PS_"
					TypeSpecifier* this_type = popLastTypeFromRepeatTypeSet();
					addToRepeatTypeSet();
					addToRepeatTypeSet(this_type);
					p++; // skip first visible parameter after "this"
					if(p == node.parameters.end())
						return; // NOTE: prevent writing "v" for empty param list
				}
			}
		}

		if(p == node.parameters.end())
			stream << "v"; // empty param-list equivalent to "void" param type
		else
		{
			mInsideParamList = true;
			for(; p != node.parameters.end(); p++)
			{
				if((*p)->type->type == TypeSpecifier::ReferredType::PRIMITIVE)
					visit(*(*p)->type);
				else
				{
					int type_index = findRepeatTypeSet((*p)->type);
					if(type_index == -1)
					{
//						std::wcout << L"BEFORE visit FunctionDecl param: " << (*p)->type->toString() << std::endl;
						visit(*(*p)->type);
//						std::wcout << L"AFTER visit FunctionDecl param: " << (*p)->type->toString() << std::endl;
						addToRepeatTypeSet((*p)->type);
					}
					else
						writeSubstitution(type_index);
				}
			}
			mInsideParamList = false;
		}

		if(isa<TemplatedIdentifier>(node.name))
			visit(*node.type);
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
#if 0 // NOTE: for debugging only
			if(c == '<') break;
#endif
			if( ((i == ucs4_to_utf8_temp.begin()) ? false : isdigit(c)) || isalpha(c) || (c == '_') || (c == '.') )
				utf8_to_llvm_temp.push_back(c);
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
#if 1 // NOTE: for debugging only
		std::cout << "NameManglingVisitor: " << stream.str() << std::endl;
#endif
		stream.str("");
		mInsideUptrace = mInsideComboName = mInsideParamList = false;
	}

	std::stringstream stream;

private:
	void uptrace(ASTNode* node)
	{
		mInsideUptrace = true;
		if(node->parent)
			visit(*node->parent); // up-trace to build FQN
		mInsideUptrace = false;
	}

	void uptraceAndAppendName(Declaration* node)
	{
		bool inside_combo_name = (!mInsideUptrace && !ASTNodeHelper::isRootPackage(node->parent));

		{
			mInsideComboName |= inside_combo_name;
			uptrace(node);
			mInsideComboName = false;
		}

		visit(*node->name);
		if(inside_combo_name)
			stream << "E"; // if mangled name is a combo name, postfix "E"
	}

	void resolveFirstThenVisit(ASTNode* node)
	{
        ASTNode* resolved_type = ASTNodeHelper::findUniqueTypeResolution(node);
        if(resolved_type)
        	visit(*resolved_type);
	}

	static bool isEqual(TypeSpecifier* a, TypeSpecifier* b)
	{
		return a->isEqual(*b);
	}

	static const char* toAsciiNumber(char c)
	{
		static char buffer[4];
		snprintf(buffer, 3, "%d", c);
		return buffer;
	}

	bool mInsideUptrace;
	bool mInsideComboName;
	bool mInsideParamList;

	std::vector<TypeSpecifier*> mRepeatTypeSet;

	int findRepeatTypeSet(TypeSpecifier* type_specifier)
	{
		auto p = std::find_if(mRepeatTypeSet.begin(), mRepeatTypeSet.end(),
				[&](TypeSpecifier* other) { return other ? type_specifier->isEqual(*other) : false; } );
		return (p != mRepeatTypeSet.end()) ? std::distance(mRepeatTypeSet.begin(), p) : -1;
	}

	void addToRepeatTypeSet(TypeSpecifier* type_specifier = NULL, bool check_if_unique = false)
	{
		if(check_if_unique && findRepeatTypeSet(type_specifier) != -1)
			return;
		mRepeatTypeSet.push_back(type_specifier);
	}

	void clearRepeatTypeSet()
	{
		mRepeatTypeSet.clear();
	}

	TypeSpecifier* popLastTypeFromRepeatTypeSet()
	{
		TypeSpecifier* type_specifier = mRepeatTypeSet.back();
		mRepeatTypeSet.pop_back();
		return type_specifier;
	}

	std::string ito36a(size_t n)
	{
	    static char b[] = "0123456789abcdefghijklmnopqrstuvwxyz";
	    std::string tmp ;
	    for(;n; n /= 36)
	        tmp += b[n % 36];
	    if(tmp.empty()) return "0";
	    else return std::string(tmp.rbegin(), tmp.rend());
	}

	void writeSubstitution(int type_index)
	{
		BOOST_ASSERT(type_index >= 0 && "negative type index has no substitution");
		stream << ((type_index == 0) ? "S_" : "S"+ito36a(type_index-1)+"_");
	}

	std::wstring getPureName(Identifier* ident)
	{
		if(isa<SimpleIdentifier>(ident))
			return cast<SimpleIdentifier>(ident)->name;
		else if(isa<TemplatedIdentifier>(ident))
			return getPureName(cast<TemplatedIdentifier>(ident)->id);
		else if(isa<NestedIdentifier>(ident))
		{
			if(!cast<NestedIdentifier>(ident)->identifier_list.empty())
				return getPureName(cast<NestedIdentifier>(ident)->identifier_list.back());
			else
				return L"";
		}
		else
			UNREACHABLE_CODE();
		return L"";
	}

	bool isReservedConstructName(std::wstring s)
	{
		return (s == L"ptr_" || s == L"ref_" || s == L"const_" || s == L"void_");
	}
};

} } } }

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_GENERAL_NAMEMANGLINGVISITOR_H_ */
