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

	NameManglingVisitor() : mInsideUptrace(false), mInsideComboName(false), mParamDepth(0),
			mModeCallByValue(false), mCurrentOutStream(&mOutStream)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(mangleInvoker)
	}

	void mangle(ASTNode& node)
	{ }

	void mangle(Package& node)
	{
		if(ASTNodeHelper::isRootPackage(&node))
		{
			if(mParamDepth == 0)
				outStream() << "_Z"; // RULE: mangled name begins with "_Z"
			if(mInsideComboName)
				outStream() << "N"; // RULE: combo name begins with "N"
		}
		else
		{
			visitParentThenPrintName(&node);

			{
				TypeSpecifier* temp_type_specifier = ASTNodeHelper::buildResolvableTypeSpecifier(&node);
				mAliasMgr.add(temp_type_specifier);
				mAliasMgr.mManagedAliasSlots.push_back(shared_ptr<TypeSpecifier>(temp_type_specifier));
			}
		}
	}

	void mangle(SimpleIdentifier& node)
	{
		if(node.name == L"ptr_")        outStream() << "P";
		else if(node.name == L"ref_")   outStream() << "R";
		else if(node.name == L"const_") outStream() << "K";
		else if(node.name == L"void_")  outStream() << "v";
		else if(node.name == L"new")
		{
			outStream() << "C1";
#if 0 // NOTE: see --> http://stackoverflow.com/questions/6921295/dual-emission-of-constructor-symbols
			outStream() << "C2"; // base object constructor
#endif
		}
		else if(!node.name.empty())
			outStream() << node.name.length() << encode(node.name);
	}

	void mangle(TemplatedIdentifier& node)
	{
		visit(*node.id);

		bool reserved_construct = isReservedConstructName(getBasename(node.id));
		if(!reserved_construct)
		{
//			if(isa<ClassDecl>(node.parent))
//			{
//				TypeSpecifier* temp_type_specifier = ASTNodeHelper::buildResolvableTypeSpecifier(
//						cast<ClassDecl>(node.parent));
//				if(mAliasMgr.find(temp_type_specifier) == -1)
//					mAliasMgr.addDummy();
//			}
			mAliasMgr.addDummy(); // FIXME: HACK: unreasonable to do this every time
			outStream() << "I"; // RULE: template params begin with "I"
		}

		{
			mParamDepth++;
			foreach(i, node.templated_type_list)
			{
				if(node.type == TemplatedIdentifier::Usage::FORMAL_PARAMETER)
				{
					ASTNode* resolved_type = ASTNodeHelper::findUniqueTypeResolution(*i);
					if(resolved_type)
					{
						if(isa<ClassDecl>(resolved_type))
						{
							// HACK: can't do reverse-lookup
							// * since "ASTNodeHelper::findUniqueTypeResolution" jumps directly to "Declaration",
							//   skipping "TypeSpecifier", we miss a chance to check if we've visited a type.
							// * a work-around, albeit dirty, is to build a temporary type for the "Declaration",
							//   and visit that instead.
							// * one side-effect is that we must remember to delete the temporary type after each
							//   name mangling.
							TypeSpecifier* temp_type_specifier = ASTNodeHelper::buildResolvableTypeSpecifier(
									cast<ClassDecl>(resolved_type));
							visitAliasedType(temp_type_specifier);
							mAliasMgr.mManagedAliasSlots.push_back(shared_ptr<TypeSpecifier>(temp_type_specifier));
						}
						else
							visit(*resolved_type);
					}
				}
			}
			mParamDepth--;
		}

		if(!reserved_construct)
			outStream() << "E"; // RULE: template params end with "E"
	}

	void mangle(TypeSpecifier& node)
	{
		switch(node.type)
		{
		case TypeSpecifier::ReferredType::FUNCTION_TYPE:
			visit(*node.referred.function_type);
			mAliasMgr.add(&node);
			break;
		case TypeSpecifier::ReferredType::PRIMITIVE:
			switch(node.referred.primitive)
			{
			case PrimitiveType::VOID:    outStream() << "v"; break;
			case PrimitiveType::BOOL:    outStream() << "b"; break;
			case PrimitiveType::INT8:    outStream() << "c"; break;
			case PrimitiveType::INT16:   outStream() << "s"; break;
			case PrimitiveType::INT32:   outStream() << "l"; /* or 'i' ? */ break;
			case PrimitiveType::INT64:   outStream() << "x"; break;
			case PrimitiveType::FLOAT32: outStream() << "f"; break;
			case PrimitiveType::FLOAT64: outStream() << "d"; break;
			default: UNREACHABLE_CODE();
			}
			break;
		case TypeSpecifier::ReferredType::UNSPECIFIED:
			{
				ASTNode* resolved_type = ASTNodeHelper::findUniqueTypeResolution(&node);
				BOOST_ASSERT(resolved_type && "failed to resolve type");
				visit(*resolved_type);
				mAliasMgr.add(&node);
			}
			break;
		}
	}

	void mangle(FunctionType& node)
	{
		visitParent(&node);
		outStream() << "PF"; // RULE/THOR_SPECIFIC: callback begins with "F", and is a pointer, hence "P" in "PF"
		visit(*node.return_type);
		mParamDepth++;
		if(node.parameter_types.empty())
			writeVoid(); // RULE: empty param-list equivalent to "void" type
		else
		{
			foreach(i, node.parameter_types)
				visitAliasedType(*i);
		}
		mParamDepth--;
		outStream() << "E"; // RULE: callback ends with "E"
		mAliasMgr.addDummy(); // HACK: function type occupies 2 alias slots
	}

	void mangle(Declaration& node)
	{
		visitParentThenPrintName(&node);
	}

	void mangle(ClassDecl& node)
	{
		if(isa<TemplatedIdentifier>(node.name) && !cast<TemplatedIdentifier>(node.name)->isFullySpecialized())
			return;

		visitParentThenPrintName(&node);

		{
			TypeSpecifier* temp_type_specifier = ASTNodeHelper::buildResolvableTypeSpecifier(&node);
			mAliasMgr.add(temp_type_specifier);
			mAliasMgr.mManagedAliasSlots.push_back(shared_ptr<TypeSpecifier>(temp_type_specifier));
		}
	}

	void mangle(FunctionDecl& node)
	{
		if(isa<TemplatedIdentifier>(node.name) && !cast<TemplatedIdentifier>(node.name)->isFullySpecialized())
			return;

		mModeCallByValue = ASTNodeHelper::findAnnotation(&node, L"call_by_value");

		mAliasMgr.clear();
		visitParentThenPrintName(&node);

		if(isa<TemplatedIdentifier>(node.name))
			visit(*node.type);

		mParamDepth++;
		int ThisSlot = -1;
		if(node.is_member && !node.is_static)
		{
			BOOST_ASSERT(node.parent && "method must have parent");
			BOOST_ASSERT(isa<Declaration>(node.parent) && "method parent must be declaration");
			Declaration* decl = cast<Declaration>(node.parent);
			if(isa<TemplatedIdentifier>(decl->name) && !cast<TemplatedIdentifier>(decl->name)->isFullySpecialized())
				return;
			BOOST_ASSERT((node.parameters.size() > 0) && "method must have one parameter");
			auto first_param = node.parameters.begin();
			ASTNode* resolved_type = ASTNodeHelper::findUniqueTypeResolution((*first_param)->type);
			BOOST_ASSERT(resolved_type->isEqual(*node.parent) && "method's first parameter must be \"this\"");

			{
				mCurrentOutStream = NULL;
				visit(*(*first_param)->type);
				ThisSlot = mAliasMgr.find((*first_param)->type);
				mCurrentOutStream = &mOutStream;
			}
		}
		if(ThisSlot == -1)
		{
			if(node.parameters.empty())
				writeVoid(); // RULE: empty param-list equivalent to "void" type
			else
			{
				foreach(i, node.parameters)
					visitAliasedType((*i)->type);
			}
		}
		else
		{
			if(node.parameters.size() == 1)
				writeVoid(); // RULE: empty param-list equivalent to "void" type
			else
			{
				bool VisitedFirstExplicitThis = false;
				foreach(i, node.parameters)
				{
					if(i != node.parameters.begin())
					{
						if(!VisitedFirstExplicitThis
								&& ASTNodeHelper::sameResolvedType((*i)->type, mAliasMgr.mAliasSlots[ThisSlot]))
						{
							if(mModeCallByValue)
								visitAliasedType((*i)->type);
							else
							{
								outStream() << "P"; // THOR_SPECIFIC: object passing is by pointer, hence "P"
								visitAliasedType((*i)->type);
								TypeSpecifier* type_specifier = mAliasMgr.remove(ThisSlot);
								mAliasMgr.addDummy(); // HACK: bump up actual type to account for pointer type
								mAliasMgr.add(type_specifier); // HACK: send "ThisSlot" to back
							}
							VisitedFirstExplicitThis = true;
						}
						else
							visitAliasedType((*i)->type);
					}
				}
			}
		}
		mParamDepth--;

		mModeCallByValue = false;
	}

	void reset()
	{
#if 0 // NOTE: for debugging only
		std::cout << "NameManglingVisitor: " << mCurrentOutStream->str() << std::endl;
#endif
		mCurrentOutStream->str("");
		mInsideUptrace = mInsideComboName = false;
		mParamDepth = 0;
		mModeCallByValue = false;
	}

	std::stringstream mOutStream;

private:
	std::stringstream* mCurrentOutStream;

	std::ostream& outStream()
	{
		static std::stringstream NullOutStream;
		return mCurrentOutStream ? *mCurrentOutStream : NullOutStream;
	}

	void visitParent(ASTNode* node)
	{
		mInsideUptrace = true;
		if(node->parent)
			visit(*node->parent); // up-trace to build FQN
		mInsideUptrace = false;
	}

	void visitParentThenPrintName(ASTNode* node)
	{
		BOOST_ASSERT((isa<Declaration>(node) || isa<Package>(node)) && "unhandled node type");
		bool end_of_combo_name = (!mInsideUptrace && !ASTNodeHelper::isRootPackage(node->parent));

		{
			mInsideComboName |= end_of_combo_name;
			visitParent(node);
			mInsideComboName = false;
		}

		if(isa<Declaration>(node))
			visit(*cast<Declaration>(node)->name);
		else if(isa<Package>(node))
			visit(*cast<Package>(node)->id);
		else
			UNREACHABLE_CODE();
		if(end_of_combo_name)
			outStream() << "E"; // RULE: combo name ends with "E"
	}

	void visitAliasedType(TypeSpecifier* type_specifier)
	{
		if(type_specifier->type == TypeSpecifier::ReferredType::PRIMITIVE)
			visit(*type_specifier);
		else
		{
			int slot = mAliasMgr.find(type_specifier);
			if(slot == -1)
			{
				ASTNode* resolved_type = ASTNodeHelper::findUniqueTypeResolution(type_specifier);
				BOOST_ASSERT(resolved_type && "failed to resolve type");
				if((mParamDepth > 0)
						&& isa<Declaration>(resolved_type)
						&& !isReservedConstructName(getBasename(cast<Declaration>(resolved_type)->name))
						&& !mModeCallByValue)
				{
					outStream() << "P"; // THOR_SPECIFIC: object passing is by pointer, hence "P"
					mAliasMgr.addDummy(); // HACK: bump up actual type to account for pointer type
				}
				visit(*type_specifier);
			}
			else
				printSubstitution(slot);
		}
	}

	bool mInsideUptrace;
	bool mInsideComboName;
	int mParamDepth;
	bool mModeCallByValue;

	struct AliasMgr
	{
		int find(TypeSpecifier* type_specifier)
		{
			auto p = std::find_if(mAliasSlots.begin(), mAliasSlots.end(),
					[&](TypeSpecifier* other) {
							return other ? ASTNodeHelper::sameResolvedType(type_specifier, other) : false;
							} );
			return (p != mAliasSlots.end()) ? std::distance(mAliasSlots.begin(), p) : -1;
		}

		void add(TypeSpecifier* type_specifier = NULL, bool check_if_unique = true)
		{
			if(check_if_unique && find(type_specifier) != -1)
				return;
			mAliasSlots.push_back(type_specifier);
		}

		void addDummy()
		{
			add(NULL, false);
		}

		TypeSpecifier* remove(int slot)
		{
			BOOST_ASSERT((slot != -1) && "invalid slot");
			TypeSpecifier* type_specifier = mAliasSlots[slot];
			mAliasSlots.erase(mAliasSlots.begin()+slot);
			return type_specifier;
		}

		void clear()
		{
			mAliasSlots.clear();
			mManagedAliasSlots.clear();
		}

		std::vector<TypeSpecifier*> mAliasSlots;
		std::vector<shared_ptr<TypeSpecifier>> mManagedAliasSlots;
	} mAliasMgr;

	void printSubstitution(int slot)
	{
		auto ito36a = [](size_t n)
			{
				static const char b[] = "0123456789abcdefghijklmnopqrstuvwxyz";
				std::string s ;
				for(;n ; n /= 36)
					s += b[n % 36];
				return !s.empty() ? std::string(s.rbegin(), s.rend()) : "0";
			};
		BOOST_ASSERT(slot >= 0 && "invalid slot");
		outStream() << ((slot == 0) ? "S_" : ("S"+ito36a(slot-1)+"_"));
	}

	void writeVoid()
	{
		TypeSpecifier type_specifier(PrimitiveType::VOID);
		visit(type_specifier);
	}

	static std::wstring getBasename(Identifier* ident)
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

	static bool isReservedConstructName(std::wstring s)
	{
		return (s == L"ptr_" || s == L"ref_" || s == L"const_" || s == L"void_");
	}

public:
	std::string encode(const std::wstring ucs4)
	{
		auto toAsciiNumber = [](char c)
			{
				static char buffer[4];
				snprintf(buffer, 3, "%d", c);
				return buffer;
			};

		std::string ucs4_to_utf8_temp;
		std::string utf8_to_llvm_temp;

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
};

} } } }

#endif /* ZILLIANS_LANGUAGE_TREE_VISITOR_GENERAL_NAMEMANGLINGVISITOR_H_ */
