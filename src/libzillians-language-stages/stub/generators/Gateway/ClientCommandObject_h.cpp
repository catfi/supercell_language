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
std::string get_stub_filename<stage::ThorScriptStubStage::GATEWAY_GAMECOMMAND_CLIENTCOMMANDOBJECT_H>(stage::ThorScriptStubStage::var_map_t& var_map)
{
	auto f = [&](std::wstring &s, std::wstring search_string, std::wstring replace_string){
		for(std::wstring::size_type p = 0; (p = s.find(search_string, p)) != std::wstring::npos; p++)
			s.replace(p, search_string.size(), replace_string);
	};
	std::wstring s = L"${GAME_NAME}_CLIENTCOMMANDOBJECT.h";
	f(s, L"${GAME_NAME}", var_map[L"game-name"]);
	return zillians::ws_to_s(s);
}
	template<>
void print_stub<stage::ThorScriptStubStage::GATEWAY_GAMECOMMAND_CLIENTCOMMANDOBJECT_H>(Tangle* node, stage::ThorScriptStubStage::var_map_t& var_map)
{
	std::wcout <<
		"\n"
		"\n"
		"#ifndef GATEWAY_GAMECOMMAND_CLIENTCOMMANDOBJECT_H_\n"
		"#define GATEWAY_GAMECOMMAND_CLIENTCOMMANDOBJECT_H_\n"
		"\n"
		"#include \"vw/services/api/ApiTypes.h\"\n"
		"#include \"ClientCommandObject.h\"\n"
		"\n"
		"namespace zillians {\n"
		"\n"
		"";
	std::vector<FunctionDecl*> AtClientFuncList;
	std::vector<FunctionDecl*> AtServerFuncList;
	{ AtFuncGeneratorVisitor v(AtClientFuncList, L"client"); v.visit(*node); }
	{ AtFuncGeneratorVisitor v(AtServerFuncList, L"server"); v.visit(*node); }
	auto f = [&](FunctionDecl* node){
		std::wstring game_name     = var_map[L"game-name"];
		std::wstring function_name = node->name->toString();
		std::wstringstream ss;
		ss << L"0x" << std::hex << stage::SymbolIdManglingContext::get(node)->managled_id;
		std::wstring function_id = ss.str();
		std::wcout <<
			"\n"
			"        class ";
		std::wcout << game_name;
		std::wcout <<
			"_";
		std::wcout << function_name;
		std::wcout <<
			"_ClientCommandObject : public ClientCommandObject\n"
			"        {\n"
			"        public:\n"
			"            ";
		std::wcout << game_name;
		std::wcout <<
			"_";
		std::wcout << function_name;
		std::wcout <<
			"_ClientCommandObject()\n"
			"            { }\n"
			"\n"
			"            ~";
		std::wcout << game_name;
		std::wcout <<
			"_";
		std::wcout << function_name;
		std::wcout <<
			"_ClientCommandObject()\n"
			"            { }\n"
			"\n"
			"            enum\n"
			"            {\n"
			"                // GENERATE_BEGIN\n"
			"                TYPE = ";
		std::wcout << function_id;
		std::wcout <<
			" // the type need to be unique globally (across all RPC command types)\n"
			"                // GENERATE_END\n"
			"            };\n"
			"\n"
			"        public:\n"
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
				std::wcout << L"api_types::api_" << ASTNodeHelper::getNodeName(type_specifier) << L" " << (*i)->name->toString() << L";" << std::endl;
			else
				std::wcout << L"UUID " << (*i)->name->toString() << L";" << std::endl;
		}
		std::wcout <<
			"\n"
			"            // GENERATE_END\n"
			"\n"
			"        public:\n"
			"            template<typename Archive>\n"
			"            void serialize(Archive& ar, const unsigned int version)\n"
			"            {\n"
			"                ClientCommandObject::serialize(ar, version); // 16 bytes\n"
			"                // GENERATE_BEGIN\n"
			"                ";
		foreach(i, node->parameters)
			std::wcout << L"ar & " << (*i)->name->toString() << L";" << std::endl;
		std::wcout <<
			"\n"
			"                // GENERATE_END\n"
			"            }\n"
			"        };\n"
			"        ";
	};
	foreach(i, AtClientFuncList) f(*i);
	foreach(i, AtServerFuncList) f(*i);
	std::wcout <<
		"\n"
		"\n"
		"} // namespace zillians\n"
		"\n"
		"#endif\n"
		"";
}
