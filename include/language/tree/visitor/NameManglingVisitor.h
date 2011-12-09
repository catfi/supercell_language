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

	NameManglingVisitor() : mInsideUptrace(false), mInsideComboName(false), mInsideParamList(false),
			mModeCallByValue(false), mOutChannel(&mOutStream)
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
				outChannel() << "_Z"; // RULE: mangled name begins with "_Z"
			if(mInsideComboName)
				outChannel() << "N"; // RULE: combo name begins with "N"
		}
		else
		{
			uptrace(&node);
			visit(*node.id);
		}
	}

	void mangle(SimpleIdentifier& node)
	{
		if(node.name == L"ptr_")        outChannel() << "P";
		else if(node.name == L"ref_")   outChannel() << "R";
		else if(node.name == L"const_") outChannel() << "K";
		else if(node.name == L"void_")  outChannel() << "v";
		else if(node.name == L"new")
		{
			outChannel() << "C1";
#if 0 // NOTE: see --> http://stackoverflow.com/questions/6921295/dual-emission-of-constructor-symbols
			outChannel() << "C2"; // base object constructor
#endif
		}
		else if(!node.name.empty())
			outChannel() << node.name.length() << encode(node.name);
	}

	void mangle(TemplatedIdentifier& node)
	{
		visit(*node.id);

		bool reserved_construct = isReservedConstructName(getBasename(node.id));
		if(!reserved_construct)
			outChannel() << "I"; // RULE: template begins with "I"

		{
			mInsideParamList = true;
			foreach(i, node.templated_type_list)
			{
				if(node.type == TemplatedIdentifier::Usage::FORMAL_PARAMETER)
				{
			        ASTNode* resolved_type = ASTNodeHelper::findUniqueTypeResolution(*i);
			        if(resolved_type)
			        {
			        	if(isa<ClassDecl>(resolved_type))
			        	{
			        		TypeSpecifier* type_specifier = ASTNodeHelper::buildResolvableTypeSpecifier(
			        				cast<ClassDecl>(resolved_type));
			    			addToAliasSlots(type_specifier);
			    			mManagedAliasSlots.push_back(shared_ptr<TypeSpecifier>(type_specifier));
			    			visit(*type_specifier);
			        	}
			        	else
			        		visit(*resolved_type);
			        }
				}
			}
			mInsideParamList = false;
		}

		if(!reserved_construct)
			outChannel() << "E"; // RULE: template ends with "E"
	}

	void mangle(TypeSpecifier& node)
	{
		switch(node.type)
		{
		case TypeSpecifier::ReferredType::FUNCTION_TYPE:
			visit(*node.referred.function_type);
			addToAliasSlots(&node);
			break;
		case TypeSpecifier::ReferredType::PRIMITIVE:
			switch(node.referred.primitive)
			{
			case PrimitiveType::VOID:    outChannel() << "v"; break;
			case PrimitiveType::BOOL:    outChannel() << "b"; break;
			case PrimitiveType::INT8:    outChannel() << "c"; break;
			case PrimitiveType::INT16:   outChannel() << "s"; break;
			case PrimitiveType::INT32:   outChannel() << "l"; /* or 'i' ? */ break;
			case PrimitiveType::INT64:   outChannel() << "x"; break;
			case PrimitiveType::FLOAT32: outChannel() << "f"; break;
			case PrimitiveType::FLOAT64: outChannel() << "d"; break;
			default: UNREACHABLE_CODE();
			}
			break;
		case TypeSpecifier::ReferredType::UNSPECIFIED:
			{
				ASTNode* resolved_type = ASTNodeHelper::findUniqueTypeResolution(&node);
				BOOST_ASSERT(resolved_type && "failed to resolve unspecified symbol!");
				visit(*resolved_type);
				addToAliasSlots(&node);
			}
			break;
		}
	}

	void mangle(FunctionType& node)
	{
		uptrace(&node);
		outChannel() << "PF"; // RULE/THOR_SPECIFIC: callback begins with "F", and is a pointer, hence "P" in "PF"
		visit(*node.return_type);

		mInsideParamList = true;
		if(node.parameter_types.empty())
			visitEmptyParams();
		else
		{
			foreach(i, node.parameter_types)
				visitParam(*i);
		}
		mInsideParamList = false;

		outChannel() << "E"; // RULE: callback ends with "E"

		addToAliasSlots(NULL, false); // HACK: function type occupies 2 alias slots, not sure why..
	}

	void mangle(Declaration& node)
	{
		uptraceAndAppendName(&node);
	}

	void mangle(ClassDecl& node)
	{
		bool need_ptr_type = (mInsideParamList && !isReservedConstructName(getBasename(node.name)) && !mModeCallByValue);
		if(need_ptr_type)
			outChannel() << "P"; // THOR_SPECIFIC: object passing is by pointer, hence "P"

		uptraceAndAppendName(&node);

		if(need_ptr_type)
			addToAliasSlots(NULL, false); // HACK: alias slot for pointer-to-class type
	}

	void mangle(FunctionDecl& node)
	{
		mModeCallByValue = ASTNodeHelper::findAnnotation(&node, L"call_by_value");

		clearAliasSlots();
		uptraceAndAppendName(&node);

		mInsideParamList = true;
		auto p = node.parameters.begin();
		if(node.is_member && !node.is_static)
		{
			BOOST_ASSERT(node.parent && "method must have parant");
			BOOST_ASSERT((node.parameters.size() > 0) && "method must have one parameter");
			ASTNode* resolved_type = ASTNodeHelper::findUniqueTypeResolution((*p)->type);
#if 0 // NOTE: should work, but doesn't -- type has no ResolvedType context
			BOOST_ASSERT((resolved_type == node.parent) && "method's first parameter must be \"this\"");
			mOutChannel = NULL;
			visit(*(*p)->type);
			mOutChannel = &mOutStream;
#endif
			addToAliasSlots((*p)->type);
			p++; // skip "this" param
			if(p != node.parameters.end())
			{
				ASTNode* resolved_type = ASTNodeHelper::findUniqueTypeResolution((*p)->type);
				if(resolved_type == node.parent)
				{
					outChannel() << "PS_"; // THOR_SPECIFIC: "this" is a pointer, hence "P" in "PS_"
					TypeSpecifier* this_type = popFinalAliasSlots(); // HACK: bump "this" param up 1 alias slot
					addToAliasSlots(NULL, false);                    // HACK: alias slot for "*this" param
					addToAliasSlots(this_type);                      // HACK: alias slot for "this" param
					p++;                                             // HACK: skip "this" param (again!)
					if(p == node.parameters.end())
						return; // NOTE: prevent writing "v" for empty param list
				}
			}
		}
		if(p == node.parameters.end())
			visitEmptyParams();
		else
		{
			for(; p != node.parameters.end(); p++)
				visitParam((*p)->type);
		}
		mInsideParamList = false;

		if(isa<TemplatedIdentifier>(node.name))
			visit(*node.type);

		mModeCallByValue = false;
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
		std::cout << "NameManglingVisitor: " << mOutChannel->str() << std::endl;
#endif
		mOutChannel->str("");
		mInsideUptrace = mInsideComboName = mInsideParamList = mModeCallByValue = false;
	}

	std::stringstream mOutStream;

private:
	std::stringstream* mOutChannel;

	std::ostream& outChannel()
	{
		static std::stringstream nullOutChannel;
		if(mOutChannel)
			return *mOutChannel;
		else
			return nullOutChannel;
	}

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
			outChannel() << "E"; // RULE: combo name ends with "E"
	}

	void visitParam(TypeSpecifier* type_specifier)
	{
		if(type_specifier->type == TypeSpecifier::ReferredType::PRIMITIVE)
			visit(*type_specifier);
		else
		{
			int slot = findAliasSlot(type_specifier);
			if(slot == -1)
				visit(*type_specifier);
			else
				writeSubstitution(slot);
		}
	}

	void visitEmptyParams()
	{
		TypeSpecifier type_specifier(PrimitiveType::VOID);
		visit(type_specifier); // RULE: empty param-list equivalent to "void" param type
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
	bool mModeCallByValue;

	std::vector<TypeSpecifier*> mAliasSlots;
	std::vector<shared_ptr<TypeSpecifier>> mManagedAliasSlots;

	int findAliasSlot(TypeSpecifier* type_specifier)
	{
		auto p = std::find_if(mAliasSlots.begin(), mAliasSlots.end(),
				[&](TypeSpecifier* other) { return other ? type_specifier->isEqual(*other) : false; } );
		return (p != mAliasSlots.end()) ? std::distance(mAliasSlots.begin(), p) : -1;
	}

	void addToAliasSlots(TypeSpecifier* type_specifier = NULL, bool check_if_unique = true)
	{
		if(check_if_unique && findAliasSlot(type_specifier) != -1)
			return;
		mAliasSlots.push_back(type_specifier);
	}

	void clearAliasSlots()
	{
		mAliasSlots.clear();
		mManagedAliasSlots.clear();
	}

	TypeSpecifier* popFinalAliasSlots()
	{
		TypeSpecifier* type_specifier = mAliasSlots.back();
		mAliasSlots.pop_back();
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

	void writeSubstitution(int slot)
	{
		BOOST_ASSERT(slot >= 0 && "negative type index has no substitution");
		outChannel() << ((slot == 0) ? "S_" : "S"+ito36a(slot-1)+"_");
	}

	std::wstring getBasename(Identifier* ident)
	{
		if(isa<SimpleIdentifier>(ident))
			return cast<SimpleIdentifier>(ident)->name;
		else if(isa<TemplatedIdentifier>(ident))
			return getBasename(cast<TemplatedIdentifier>(ident)->id);
		else if(isa<NestedIdentifier>(ident))
		{
			if(!cast<NestedIdentifier>(ident)->identifier_list.empty())
				return getBasename(cast<NestedIdentifier>(ident)->identifier_list.back());
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
