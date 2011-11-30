#include "language/stage/stub/ThorScriptStubStage.h"
#include "language/stage/transformer/context/ManglingStageContext.h"
#include "language/tree/ASTNodeHelper.h"
#include "language/tree/ASTNode.h"
#include "language/tree/visitor/GenericDoubleVisitor.h"
#include "utility/Foreach.h"
#include "utility/UnicodeUtil.h"
#include <sstream>
#include <iostream>
#include <string>
#include <vector>
using namespace zillians::language;
using namespace zillians::language::tree; // needed by CREATE_INVOKER
struct AtFuncGeneratorVisitor : public visitor::GenericDoubleVisitor
{
	CREATE_INVOKER(generateInvoker, generate)
		std::vector<FunctionDecl*>& mFuncList;
	std::wstring mTag;
	AtFuncGeneratorVisitor(std::vector<FunctionDecl*>& FuncList, std::wstring tag) : mFuncList(FuncList), mTag(tag)
	{
		REGISTER_ALL_VISITABLE_ASTNODE(generateInvoker)
	}
	void generate(ASTNode& node)
	{
		revisit(node);
	}
	void generate(FunctionDecl& node)
	{
		if(ASTNodeHelper::findAnnotation(&node, mTag))
			mFuncList.push_back(&node);
	}
};
	template<>
std::string get_stub_filename<stage::ThorScriptStubStage::CLIENT_CLIENTSTUB_H>(stage::ThorScriptStubStage::var_map_t& var_map)
{
	return "ClientStub.h";
}
	template<>
void print_stub<stage::ThorScriptStubStage::CLIENT_CLIENTSTUB_H>(Tangle* node, stage::ThorScriptStubStage::var_map_t& var_map)
{
	std::wcout <<
		"\n"
		"\n"
		"#ifndef CLIENT_CLIENTSTUB_H_\n"
		"#define CLIENT_CLIENTSTUB_H_\n"
		"\n"
		"#include \"zillians/Base.h\"\n"
		"#include \"GameObjects.h\"\n"
		"\n"
		"class xx_ClientStub\n"
		"{\n"
		"public:\n"
		"\n"
		"";
	std::vector<FunctionDecl*> AtClientFuncList;
	{ AtFuncGeneratorVisitor v(AtClientFuncList, L"client"); v.visit(*node); }
	auto f = [&](FunctionDecl* node){
		std::wstring function_name = node->name->toString();
		std::wstringstream ss;
		ss << L"0x" << std::hex << stage::SymbolIdManglingContext::get(node)->managled_id;
		std::wstring function_id = ss.str();
		std::wcout <<
			"\n"
			"        virtual void ";
		std::wcout << function_name;
		std::wcout <<
			"(\n"
			"            // GENERATE_BEGIN\n"
			"            ";
		foreach(i, node->parameters)
		{
			TypeSpecifier* type_specifier = (*i)->type;
			if(ASTNodeHelper::isUnspecifiedType(type_specifier))
			{
				ASTNode* resolved_type = ResolvedType::get(*i);
				if(resolved_type && isa<TypeSpecifier>(resolved_type))
					type_specifier = cast<TypeSpecifier>(resolved_type);
			}
			if(ASTNodeHelper::isPrimitiveType(type_specifier))
				std::wcout << L"zillians::" << ASTNodeHelper::getNodeName((*i)->type) << L" " << (*i)->name->toString() << L"," << std::endl;
			else
				std::wcout << ASTNodeHelper::getNodeName(type_specifier) << L"& " << (*i)->name->toString() << L"," << std::endl;
		}
		std::wcout <<
			"\n"
			"            // GENERATE_END\n"
			"            ) = 0;\n"
			"        ";
	};
	foreach(i, AtClientFuncList) f(*i);
	std::wcout <<
		"\n"
		"\n"
		"};\n"
		"\n"
		"#endif\n"
		"";
}
