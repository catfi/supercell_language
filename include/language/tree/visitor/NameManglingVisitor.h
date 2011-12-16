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

// References:
// - http://sourcery.mentor.com/public/cxx-abi/abi.html#mangling
// - http://www.agner.org/optimize/calling_conventions.pdf

// Rule Summary:
// 1. all names begin with "_Z"
// 2. simple names have the form: [NAME_LENGTH]<NAME>, EXAMPLE: "aaa" --> "3aaa"
// 3. nested namespaces translate into "combo names"
//    combo names have the form: N(*<SIMPLE_NAME>)E,  EXAMPLE: "aaa.bbb.ccc"            --> "N3aaa3bbb3cccE"
// 4. templates have the form: <NAME>I(*<NAME>)E,     EXAMPLE: "ddd<aaa.bbb.ccc>"       --> "3dddIN3aaa3bbb3cccEE"
// 5. functions have the form: <NAME>(+<PARAM_TYPE>), EXAMPLE: "void _eee_(char, char)" --> "_Z5_eee_cc"
//    an empty parameter list translates into "void"
// 6. template functions have the form: <NAME><RETURN_TYPE>(+<PARAM_TYPE>)
//    EXAMPLE: "void _fff_<bool>(char, char)" --> "_Z5_fff_IbEvcc"
// 7. primitive types are mangled as follows:
//    void      --> v
//    bool      --> b
//    char      --> c
//    short     --> s
//    int       --> i
//    long      --> l
//    long long --> x
//    float     --> f
//    double    --> d
// 8. modifiers are mangled as follows:
//    pointer(*)   --> P<TYPE>,                                      EXAMPLE: "char*"                  --> "Pc"
//    reference(&) --> R<TYPE>,                                      EXAMPLE: "char*&"                 --> "RPc"
//    const        --> K<TYPE>,                                      EXAMPLE: "const char*&"           --> "KRPc"
// 9. function types have the form: PF<RETURN_TYPE>(+<PARAM_TYPE>)E, EXAMPLE: "void (char, char)"      --> "PFvccE"
// 10. member functions do not mangle "this",                        EXAMPLE: "aaa::_ggg_(char, char)" --> "_ZN3aaa5_ggg_Ecc"
// 11. duplicate name/types are substituted with substitution symbols
//     substitution symbols start with "S_", then "S0_", "S1_", etc.
//     the index of the form "S[index]_" is encoded in base 36
//     if these run out, then no substitution occurs
//     substitution candidates are namespace/class/modified_types/function_types/enum, but functions are never substituted
//     substitution candidates are visited from left to right
//     templates occupy an extra substitution slot with their stem name
//     function types occupy an extra substitution slot (not sure why)
//     EXAMPLE: "aaa::_hhh_(aaa* x, aaa* y, aaa* z)" --> "_ZN3aaa5_hhh_EPS_S0_S0_"
// 12. enums are mangled just like namespaces/classes
// 13. enum value mangling is not relevant to ThorScript, since template specialization to integer is not supported

// ThorScript Specific:
// 1. modifiers require special reserved templates to generate correct AST for mangling
//    pointer:   ptr_<T>
//    reference: ref_<T>
//    const:     const_<T>
//    void:      void_
// 2. when a ThorScript signature requires "exact" translation to a c++ signature, that function must be
//    annotated with "@call_by_value", otherwise ThorScript objects assume pass-by-reference mangling

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
			mangleParentThenPrintName(&node);
			mAliasMgr.addManaged(buildResolvableTypeSpecifier(&node));
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

		bool reserved_construct = isReservedConstructName(node.id);
		if(!reserved_construct)
		{
			mAliasMgr.addDummy(__LINE__); // HACK: template name occupies a slot
			outStream() << "I"; // RULE: template params begin with "I"
		}

		{
			mParamDepth++;
			foreach(i, node.templated_type_list)
			{
				if(node.type == TemplatedIdentifier::Usage::FORMAL_PARAMETER)
				{
					ASTNode* resolved_type = findUniqueTypeResolution(*i);
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
							TypeSpecifier* temp_type_specifier = buildResolvableTypeSpecifier(
									cast<ClassDecl>(resolved_type));
							mangleAliasedType(temp_type_specifier);
							mAliasMgr.addManaged(temp_type_specifier);
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
				ASTNode* resolved_type = findUniqueTypeResolution(&node);
				BOOST_ASSERT(resolved_type && "failed to resolve type");
				visit(*resolved_type);
				mAliasMgr.add(&node);
			}
			break;
		}
	}

	void mangle(FunctionType& node)
	{
		mangleParent(&node);
		outStream() << "PF"; // RULE/THOR_SPECIFIC: callback begins with "F", and is a pointer, hence "P" in "PF"
		visit(*node.return_type);
		mParamDepth++;
		if(node.parameter_types.empty())
			mangleVoid(); // RULE: empty param-list equivalent to "void" type
		else
		{
			foreach(i, node.parameter_types)
				mangleAliasedType(*i);
		}
		mParamDepth--;
		outStream() << "E"; // RULE: callback ends with "E"
		mAliasMgr.addDummy(__LINE__); // HACK: function type occupies 2 alias slots
	}

	void mangle(Declaration& node)
	{
		mangleParentThenPrintName(&node);
		mAliasMgr.addManaged(buildResolvableTypeSpecifier(&node));
	}

	void mangle(ClassDecl& node)
	{
		if(isa<TemplatedIdentifier>(node.name) && !cast<TemplatedIdentifier>(node.name)->isFullySpecialized())
			return;

		mangleParentThenPrintName(&node);
		mAliasMgr.addManaged(buildResolvableTypeSpecifier(&node));
	}

	void mangle(FunctionDecl& node)
	{
		if(isa<TemplatedIdentifier>(node.name) && !cast<TemplatedIdentifier>(node.name)->isFullySpecialized())
			return;

		mModeCallByValue = ASTNodeHelper::findAnnotation(&node, L"call_by_value");

		mAliasMgr.clear();
		mangleParentThenPrintName(&node);

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
			ASTNode* resolved_type = findUniqueTypeResolution((*first_param)->type);
			BOOST_ASSERT(resolved_type && "failed to resolve type");
			BOOST_ASSERT(resolved_type->isEqual(*node.parent) && "method's first parameter must be \"this\"");

			{
				mangleAliasedType((*first_param)->type, true);
				ThisSlot = mAliasMgr.find((*first_param)->type);
			}
		}
		if(ThisSlot == -1)
		{
			if(node.parameters.empty())
				mangleVoid(); // RULE: empty param-list equivalent to "void" type
			else
			{
				foreach(i, node.parameters)
					mangleAliasedType((*i)->type);
			}
		}
		else
		{
			if(node.parameters.size() == 1)
				mangleVoid(); // RULE: empty param-list equivalent to "void" type
			else
			{
				bool VisitedFirstExplicitThis = false;
				foreach(i, node.parameters)
				{
					if(i != node.parameters.begin())
					{
						if(!VisitedFirstExplicitThis && sameResolvedType((*i)->type, mAliasMgr.typeAt(ThisSlot)))
						{
							if(mModeCallByValue)
								mangleAliasedType((*i)->type);
							else
							{
								outStream() << "P"; // THOR_SPECIFIC: object passing is by pointer, hence "P"
								mangleAliasedType((*i)->type);
								TypeSpecifier* type_specifier = mAliasMgr.remove(ThisSlot);
								mAliasMgr.addDummy(__LINE__); // HACK: bump up actual type to account for pointer type
								mAliasMgr.add(type_specifier); // HACK: send "ThisSlot" to back
							}
							VisitedFirstExplicitThis = true;
						}
						else
							mangleAliasedType((*i)->type);
					}
				}
			}
		}
		mParamDepth--;

		mModeCallByValue = false;
	}

	void reset()
	{
#if 0 // NOTE: debug mangler
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

	void mangleParent(ASTNode* node)
	{
		mInsideUptrace = true;
		if(node->parent)
			visit(*node->parent); // up-trace to build FQN
		mInsideUptrace = false;
	}

	void mangleParentThenPrintName(ASTNode* node)
	{
		BOOST_ASSERT((isa<Declaration>(node) || isa<Package>(node)) && "unhandled node type");
		bool end_of_combo_name = (!mInsideUptrace && !ASTNodeHelper::isRootPackage(node->parent));

		{
			mInsideComboName |= end_of_combo_name;
			mangleParent(node);
			mInsideComboName = false;
		}

		if(isa<Declaration>(node))
			visit(*cast<Declaration>(node)->name);
		else if(isa<Package>(node))
		{
			TypeSpecifier* temp_type_specifier = buildResolvableTypeSpecifier(cast<Package>(node));
			mangleAliasedType(temp_type_specifier);
			mAliasMgr.addManaged(temp_type_specifier);
		}
		else
			UNREACHABLE_CODE();
		if(end_of_combo_name)
			outStream() << "E"; // RULE: combo name ends with "E"
	}

	void mangleAliasedType(TypeSpecifier* type_specifier, bool mute = false)
	{
		if(mute) mCurrentOutStream = NULL;
		if(type_specifier->type == TypeSpecifier::ReferredType::PRIMITIVE)
			visit(*type_specifier);
		else
		{
			int slot = mAliasMgr.find(type_specifier);
			if(slot == -1)
			{
				ASTNode* resolved_type = findUniqueTypeResolution(type_specifier);
				BOOST_ASSERT(resolved_type && "failed to resolve type");
				if(isa<Package>(resolved_type))
					visit(*cast<Package>(resolved_type)->id);
				else
				{
					if((mParamDepth > 0)
							&& isa<Declaration>(resolved_type)
							&& !isa<EnumDecl>(resolved_type)
							&& !isReservedConstructName(cast<Declaration>(resolved_type)->name)
							&& !mModeCallByValue)
					{
						outStream() << "P"; // THOR_SPECIFIC: object passing is by pointer, hence "P"
						mAliasMgr.addDummy(__LINE__); // HACK: bump up actual type to account for pointer type
					}
					visit(*type_specifier);
				}
			}
			else
				outStream() << genSubstitutionSymbol(slot);
		}
		if(mute) mCurrentOutStream = &mOutStream;
	}

	bool mInsideUptrace;
	bool mInsideComboName;
	int mParamDepth;
	bool mModeCallByValue;

	class AliasMgr
	{
	public:
		int find(TypeSpecifier* type_specifier)
		{
			auto p = std::find_if(mAliasSlots.begin(), mAliasSlots.end(),
					[&](TypeSpecifier* other) {
							return other ? sameResolvedType(type_specifier, other) : false;
							} );
			return (p != mAliasSlots.end()) ? std::distance(mAliasSlots.begin(), p) : -1;
		}

		void add(TypeSpecifier* type_specifier = NULL, bool check_if_unique = true)
		{
			if(check_if_unique && find(type_specifier) != -1)
				return;
#if 0 // NOTE: debug mangler
			if(check_if_unique)
			{
				BOOST_ASSERT(type_specifier && "bad input");
				std::wcout << mAliasSlots.size() << L": [add] " << type_specifier->toString() << std::endl;
			}
			else
				std::wcout << mAliasSlots.size() << L": [add] NULL";
#endif
			mAliasSlots.push_back(type_specifier);
		}

		void addManaged(TypeSpecifier* type_specifier = NULL, bool check_if_unique = true)
		{
			add(type_specifier, check_if_unique);
			mManagedAliasSlots.push_back(shared_ptr<TypeSpecifier>(type_specifier));
		}

		void addDummy(int line_number)
		{
			add(NULL, false);
#if 0 // NOTE: debug mangler
			std::wcout << L".. from line #" << line_number << std::endl;
#endif
		}

		TypeSpecifier* remove(int slot)
		{
			BOOST_ASSERT((slot != -1) && "invalid slot");
			TypeSpecifier* type_specifier = mAliasSlots[slot];
#if 0 // NOTE: debug mangler
			{
				BOOST_ASSERT(type_specifier && "bad input");
				std::wcout << mAliasSlots.size() << L": [remove]" << type_specifier->toString() << std::endl;
			}
#endif
			mAliasSlots.erase(mAliasSlots.begin()+slot);
			return type_specifier;
		}

		void clear()
		{
			mAliasSlots.clear();
			mManagedAliasSlots.clear();
		}

		TypeSpecifier* typeAt(int slot)
		{
			BOOST_ASSERT((slot != -1) && "invalid slot");
			return mAliasSlots[slot];
		}

	private:
		std::vector<TypeSpecifier*> mAliasSlots;
		std::vector<shared_ptr<TypeSpecifier>> mManagedAliasSlots;
	} mAliasMgr;

	std::string genSubstitutionSymbol(int slot)
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
		return ((slot == 0) ? "S_" : ("S"+ito36a(slot-1)+"_"));
	}

	void mangleVoid()
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

	static bool isReservedConstructName(Identifier* ident)
	{
		std::wstring s = getBasename(ident);
		return (s == L"ptr_" || s == L"ref_" || s == L"const_" || s == L"void_");
	}

	// HACK: package rejected by "ResolvedType::isValidResolvedType", must avoid "findUniqueTypeResolution"
	static ASTNode* findUniqueTypeResolution(ASTNode* node)
	{
		ASTNode* resolved_type = ResolvedType::get(node);
		if(resolved_type && isa<Package>(resolved_type))
			return resolved_type;
		else
			return ASTNodeHelper::findUniqueTypeResolution(node);
	}

	// HACK: package rejected by "ResolvedType::isValidResolvedType", must avoid "findUniqueTypeResolution"
    static bool sameResolvedType(TypeSpecifier* a, TypeSpecifier* b)
	{
        BOOST_ASSERT(a && b && "bad input");
		if((a->type == b->type) && (a->type == TypeSpecifier::ReferredType::UNSPECIFIED))
		{
			ASTNode* resolved_type_a = ResolvedType::get(a);
			ASTNode* resolved_type_b = ResolvedType::get(b);
			if((resolved_type_a && isa<Package>(resolved_type_a))
					|| (resolved_type_b && isa<Package>(resolved_type_b)))
			{
				return (resolved_type_a == resolved_type_b);
			}
		}
		return ASTNodeHelper::sameResolvedType(a, b);
	}

    // HACK: package rejected by "ResolvedType::isValidResolvedType", must avoid "findUniqueTypeResolution"
    static TypeSpecifier* buildResolvableTypeSpecifier(ASTNode* node)
	{
    	if(isa<Package>(node))
    	{
    		TypeSpecifier* type_specifier = new TypeSpecifier(cast<SimpleIdentifier>(cast<Package>(node)->id->clone()));
#if 0 // NOTE: the usual way -- BOOST_ASSERT fail if we enable this
    		ResolvedType::set(type_specifier, package);
#else
    		type_specifier->set<ResolvedType>(new ResolvedType(node));
#endif
    		return type_specifier;
    	}
    	else if(isa<Declaration>(node))
    		return ASTNodeHelper::buildResolvableTypeSpecifier(cast<Declaration>(node));
    	else
    		UNREACHABLE_CODE();
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
#if 0 // NOTE: debug mangler
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
