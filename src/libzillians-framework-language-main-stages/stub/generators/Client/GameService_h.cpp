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
std::string get_stub_filename<stage::ThorScriptStubStage::CLIENT_GAMESERVICE_H>(stage::ThorScriptStubStage::var_map_t& var_map)
{
	return "GameService.h";
}
	template<>
void print_stub<stage::ThorScriptStubStage::CLIENT_GAMESERVICE_H>(Tangle* node, stage::ThorScriptStubStage::var_map_t& var_map)
{
	std::wcout <<
		"\n"
		"\n"
		"#ifndef CLIENT_GAMESERVICE_H_\n"
		"#define CLIENT_GAMESERVICE_H_\n"
		"\n"
		"#include \"zillians/BaseService.h\" \n"
		"#include \"ClientStub.h\" \n"
		"\n"
		"class GameService : public zillians::BaseService\n"
				     "{\n"
					     "public:\n"
					     "    virtual void onClientCallReceived(zillians::uint32 fid, zillians::Buffer& buf);\n"
					     "    void setxx_ClientStub(xx_ClientStub* stub) { mxxStub = stub; }\n"
					     "    zillians::uint64 getServiceId() { return 0; }\n"
					     "protected:\n"
					     "    xx_ClientStub* mxxStub;\n"
					     "};\n"
					     "\n"
					     "class GameService : public BaseService\n"
								  "{\n"
									  "public:\n"
									  "";
	std::vector<FunctionDecl*> AtServerFuncList;
	{ AtFuncGeneratorVisitor v(AtServerFuncList, L"server"); v.visit(*node); }
	auto f = [&](FunctionDecl* node){
		std::wstring function_name = node->name->toString();
		std::wstringstream ss;
		ss << L"0x" << std::hex << stage::SymbolIdManglingContext::get(node)->managled_id;
		std::wstring function_id = ss.str();
		std::wcout <<
			"\n"
			"        void ";
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
			"            );\n"
			"        ";
	};
	foreach(i, AtServerFuncList) f(*i);
	std::wcout <<
		"\n"
		"    GameService() : BaseService() {}\n"
		"};\n"
		"\n"
		"#endif\n"
		"";
}
