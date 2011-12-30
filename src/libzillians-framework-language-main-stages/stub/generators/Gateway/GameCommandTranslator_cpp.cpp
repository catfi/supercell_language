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
std::string get_stub_filename<stage::ThorScriptStubStage::GATEWAY_GAMECOMMAND_GAMECOMMANDTRANSLATOR_CPP>(stage::ThorScriptStubStage::var_map_t& var_map)
{
	auto f = [&](std::wstring &s, std::wstring search_string, std::wstring replace_string){
		for(std::wstring::size_type p = 0; (p = s.find(search_string, p)) != std::wstring::npos; p++)
			s.replace(p, search_string.size(), replace_string);
	};
	std::wstring s = L"${GAME_NAME}_GAMECOMMANDTRANSLATOR.cpp";
	f(s, L"${GAME_NAME}", var_map[L"game-name"]);
	return zillians::ws_to_s(s);
}
	template<>
void print_stub<stage::ThorScriptStubStage::GATEWAY_GAMECOMMAND_GAMECOMMANDTRANSLATOR_CPP>(Tangle* node, stage::ThorScriptStubStage::var_map_t& var_map)
{
	std::wcout <<
		"\n"
		"\n"
		"";
	std::vector<FunctionDecl*> AtClientFuncList;
	std::vector<FunctionDecl*> AtServerFuncList;
	{ AtFuncGeneratorVisitor v(AtClientFuncList, L"client"); v.visit(*node); }
	{ AtFuncGeneratorVisitor v(AtServerFuncList, L"server"); v.visit(*node); }
	std::wstring game_name       = var_map[L"game-name"];
	std::wstring translator_uuid = var_map[L"translator-uuid"];
	std::wcout <<
		"\n"
		"#include \"plugin/PluginComponent.h\"\n"
		"#include \"plugin/PluginManager.h\"\n"
		"#include \"extension-point/NetworkTranslator.h\"\n"
		"#include \"core-api/Prerequisite.h\"\n"
		"#include \"modules/WorldGatewayComponent.h\"\n"
		"\n"
		"// GENERATE_BEGIN\n"
		"#include \"";
	std::wcout << game_name;
	std::wcout <<
		"_CloudCommandObject.h\"\n"
		"#include \"";
	std::wcout << game_name;
	std::wcout <<
		"_ClientCommandObject.h\"\n"
		"// GENERATE_END\n"
		"\n"
		"using namespace zillians::modules::world_gateway;\n"
		"using namespace zillians::plugin;\n"
		"\n"
		"class ";
	std::wcout << game_name;
	std::wcout <<
		"_GameCommandTranslator : public PluginComponent, public NetworkTranslator\n"
		"{\n"
		"public:\n"
		"    ";
	std::wcout << game_name;
	std::wcout <<
		"_GameCommandTranslator()\n"
		"    { }\n"
		"\n"
		"    ~";
	std::wcout << game_name;
	std::wcout <<
		"_GameCommandTranslator()\n"
		"    { }\n"
		"\n"
		"public:\n"
		"    virtual void start()\n"
		"    {\n"
		"        SharedPtr<PluginComponent> component = PluginManager::instance()->getRegistry()->queryComponent(\"com.zillians.module.world-gateway.main\");\n"
		"        mWorldGateway = boost::static_pointer_cast<WorldGatewayComponent>(component);\n"
		"        SharedPtr<NetworkTranslator> this_translator = boost::dynamic_pointer_cast<NetworkTranslator>(shared_from_this());\n"
		"\n"
		"        // GENERATE_BEGIN\n"
		"        ";
	foreach(i, AtClientFuncList)
	{
		std::wstring function_name = (*i)->name->toString();
		std::wcout <<
			"\n"
			"            mWorldGateway->registerCommandTranslator(";
		std::wcout << game_name;
		std::wcout <<
			"_";
		std::wcout << function_name;
		std::wcout <<
			"_CloudCommandObject::TYPE, this_translator);\n"
			"        	";
	}
	foreach(i, AtServerFuncList)
	{
		std::wstring function_name = (*i)->name->toString();
		std::wcout <<
			"\n"
			"            mWorldGateway->registerCommandTranslator(";
		std::wcout << game_name;
		std::wcout <<
			"_";
		std::wcout << function_name;
		std::wcout <<
			"_ClientCommandObject::TYPE, this_translator);\n"
			"        	";
	}
	std::wcout <<
		"\n"
		"        // GENERATE_END\n"
		"    }\n"
		"\n"
		"    virtual void stop()\n"
		"    {\n"
		"        // GENERATE_BEGIN\n"
		"        ";
	foreach(i, AtClientFuncList)
	{
		std::wstring function_name = (*i)->name->toString();
		std::wcout <<
			"\n"
			"            mWorldGateway->unregisterCommandTranslator(";
		std::wcout << game_name;
		std::wcout <<
			"_";
		std::wcout << function_name;
		std::wcout <<
			"_CloudCommandObject::TYPE);\n"
			"        	";
	}
	foreach(i, AtServerFuncList)
	{
		std::wstring function_name = (*i)->name->toString();
		std::wcout <<
			"\n"
			"            mWorldGateway->unregisterCommandTranslator(";
		std::wcout << game_name;
		std::wcout <<
			"_";
		std::wcout << function_name;
		std::wcout <<
			"_ClientCommandObject::TYPE);\n"
			"        	";
	}
	std::wcout <<
		"\n"
		"        // GENERATE_END\n"
		"        mWorldGateway.reset();\n"
		"    }\n"
		"\n"
		"private:\n"
		"    inline bool translateExternalId(const UUID& external, uint32& internal)\n"
		"    {\n"
		"        bool result = false;\n"
		"        boost::tie(internal, result) = mWorldGateway->findInternalId(external);\n"
		"        if(!result)\n"
		"        {\n"
		"            return false;\n"
		"        }\n"
		"        return true;\n"
		"    }\n"
		"\n"
		"    inline bool translateInternalId(const uint32& internal, UUID& external)\n"
		"    {\n"
		"        bool result = false;\n"
		"        boost::tie(external, result) = mWorldGateway->findExternalId(internal);\n"
		"        if(!result)\n"
		"        {\n"
		"            return false;\n"
		"        }\n"
		"        return true;\n"
		"    }\n"
		"\n"
		"    // GENERATE_BEGIN\n"
		"    ";
	foreach(i, AtClientFuncList)
	{
		std::wstring function_name = (*i)->name->toString();
		std::wcout <<
			"\n"
			"        inline bool assign(";
		std::wcout << game_name;
		std::wcout <<
			"_";
		std::wcout << function_name;
		std::wcout <<
			"_CloudCommandObject& src, ";
		std::wcout << game_name;
		std::wcout <<
			"_";
		std::wcout << function_name;
		std::wcout <<
			"_ClientCommandObject& dest)\n"
			"        {\n"
			"            // GENERATE_BEGIN\n"
			"            ";
		foreach(j, (*i)->parameters)
		{
			std::wstring name = (*j)->name->toString();
			TypeSpecifier* type_specifier = (*j)->type;
			if(ASTNodeHelper::isUnspecifiedType(type_specifier))
			{
				ASTNode* resolved_type = ResolvedType::get(*j);
				if(resolved_type && isa<TypeSpecifier>(resolved_type))
					type_specifier = cast<TypeSpecifier>(resolved_type);
			}
			if(ASTNodeHelper::isPrimitiveType(type_specifier))
				std::wcout << L"dest." << name << L" = src." << name << L";" << std::endl;
			else
				std::wcout << L"if(!translateInternalId(src." << name << L", dest." << name << L")) return false;" << std::endl;
		}
		std::wcout <<
			"\n"
			"            // GENERATE_END\n"
			"\n"
			"            return true;\n"
			"        }\n"
			"    	";
	}
	foreach(i, AtServerFuncList)
	{
		std::wstring function_name = (*i)->name->toString();
		std::wcout <<
			"\n"
			"        inline bool assign(";
		std::wcout << game_name;
		std::wcout <<
			"_";
		std::wcout << function_name;
		std::wcout <<
			"_ClientCommandObject& src, ";
		std::wcout << game_name;
		std::wcout <<
			"_";
		std::wcout << function_name;
		std::wcout <<
			"_CloudCommandObject& dest)\n"
			"        {\n"
			"            // GENERATE_BEGIN\n"
			"            ";
		foreach(j, (*i)->parameters)
		{
			std::wstring name = (*j)->name->toString();
			TypeSpecifier* type_specifier = (*j)->type;
			if(ASTNodeHelper::isUnspecifiedType(type_specifier))
			{
				ASTNode* resolved_type = ResolvedType::get(*j);
				if(resolved_type && isa<TypeSpecifier>(resolved_type))
					type_specifier = cast<TypeSpecifier>(resolved_type);
			}
			if(ASTNodeHelper::isPrimitiveType(type_specifier))
				std::wcout << L"dest." << name << L" = src." << name << L";" << std::endl;
			else
				std::wcout << L"if(!translateExternalId(src." << name << L", dest." << name << L")) return false;" << std::endl;
		}
		std::wcout <<
			"\n"
			"            // GENERATE_END\n"
			"\n"
			"            return true;\n"
			"        }\n"
			"    	";
	}
	std::wcout <<
		"\n"
		"    // GENERATE_END\n"
		"\n"
		"public:\n"
		"    bool translate(uint32 type_in, Buffer& data_in, uint32& type_out, Buffer& data_out)\n"
		"    {\n"
		"        return translate_t(type_in, data_in, type_out, data_out);\n"
		"    }\n"
		"\n"
		"    bool translate(uint32 type_in, Buffer& data_in, uint32& type_out, SpscCircularBuffer& data_out)\n"
		"    {\n"
		"        return translate_t(type_in, data_in, type_out, data_out);\n"
		"    }\n"
		"\n"
		"    bool translate(uint32 type_in, SpscCircularBuffer& data_in, uint32& type_out, Buffer& data_out)\n"
		"    {\n"
		"        return translate_t(type_in, data_in, type_out, data_out);\n"
		"    }\n"
		"\n"
		"    bool translate(uint32 type_in, SpscCircularBuffer& data_in, uint32& type_out, SpscCircularBuffer& data_out)\n"
		"    {\n"
		"        return translate_t(type_in, data_in, type_out, data_out);\n"
		"    }\n"
		"\n"
		"    template<typename inputBuffer, typename outputBuffer>\n"
		"    bool translate_t(uint32 type_in, inputBuffer& data_in, uint32& type_out, outputBuffer& data_out)\n"
		"    {\n"
		"        switch(type_in)\n"
		"        {\n"
		"        // GENERATE_BEGIN\n"
		"        ";
	foreach(i, AtClientFuncList)
	{
		std::wstring function_name = (*i)->name->toString();
		std::wcout <<
			"\n"
			"			case ";
		std::wcout << game_name;
		std::wcout <<
			"_";
		std::wcout << function_name;
		std::wcout <<
			"_CloudCommandObject::TYPE:\n"
			"			{\n"
			"				";
		std::wcout << game_name;
		std::wcout <<
			"_";
		std::wcout << function_name;
		std::wcout <<
			"_CloudCommandObject src;\n"
			"				";
		std::wcout << game_name;
		std::wcout <<
			"_";
		std::wcout << function_name;
		std::wcout <<
			"_ClientCommandObject dest;\n"
			"				type_out = ";
		std::wcout << game_name;
		std::wcout <<
			"_";
		std::wcout << function_name;
		std::wcout <<
			"_ClientCommandObject::TYPE;\n"
			"				data_in >> src;\n"
			"				if(!assign(src, dest))\n"
			"				{\n"
			"					// TODO: log here\n"
			"					return false;\n"
			"				}\n"
			"				data_out << dest;\n"
			"				return true;\n"
			"			}\n"
			"			";
	}
	foreach(i, AtServerFuncList)
	{
		std::wstring function_name = (*i)->name->toString();
		std::wcout <<
			"\n"
			"			case ";
		std::wcout << game_name;
		std::wcout <<
			"_";
		std::wcout << function_name;
		std::wcout <<
			"_ClientCommandObject::TYPE:\n"
			"			{\n"
			"				";
		std::wcout << game_name;
		std::wcout <<
			"_";
		std::wcout << function_name;
		std::wcout <<
			"_ClientCommandObject src;\n"
			"				";
		std::wcout << game_name;
		std::wcout <<
			"_";
		std::wcout << function_name;
		std::wcout <<
			"_CloudCommandObject dest;\n"
			"				type_out = ";
		std::wcout << game_name;
		std::wcout <<
			"_";
		std::wcout << function_name;
		std::wcout <<
			"_CloudCommandObject::TYPE;\n"
			"				data_in >> src;\n"
			"				if(!assign(src, dest))\n"
			"				{\n"
			"					// TODO: log here\n"
			"					return false;\n"
			"				}\n"
			"				data_out << dest;\n"
			"				return true;\n"
			"			}\n"
			"			";
	}
	std::wcout <<
		"\n"
		"        // GENERATE_END\n"
		"        default:\n"
		"        {\n"
		"             throw \"the function Id is wrong\";\n"
		"             return false;\n"
		"        }\n"
		"        }\n"
		"    }\n"
		"\n"
		"private:\n"
		"    SharedPtr<WorldGatewayComponent> mWorldGateway;\n"
		"};\n"
		"\n"
		"DECLARE_COMPONENT_IMPL(";
	std::wcout << game_name;
	std::wcout <<
		"_GameCommandTranslator, ";
	std::wcout << translator_uuid;
	std::wcout <<
		");\n"
		"";
	std::wcout <<
		"\n"
		"";
}
