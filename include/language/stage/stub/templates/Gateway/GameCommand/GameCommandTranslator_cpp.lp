//[[[global
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

//[[[entry
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
    //[[[driver
    //]]]
}
//]]]

//]]]

//[[[code
std::vector<FunctionDecl*> AtClientFuncList;
std::vector<FunctionDecl*> AtServerFuncList;
{ AtFuncGeneratorVisitor v(AtClientFuncList, L"client"); v.visit(*node); }
{ AtFuncGeneratorVisitor v(AtServerFuncList, L"server"); v.visit(*node); }

std::wstring game_name       = var_map[L"game-name"];
std::wstring translator_uuid = var_map[L"translator-uuid"];

//[[[gen(replace:GAME_NAME=game_name, TRANSLATOR_UUID=translator_uuid)
#include "plugin/PluginComponent.h"
#include "plugin/PluginManager.h"
#include "extension-point/NetworkTranslator.h"
#include "core-api/Prerequisite.h"
#include "modules/WorldGatewayComponent.h"

// GENERATE_BEGIN
#include "${GAME_NAME}_CloudCommandObject.h"
#include "${GAME_NAME}_ClientCommandObject.h"
// GENERATE_END

using namespace zillians::modules::world_gateway;
using namespace zillians::plugin;

class ${GAME_NAME}_GameCommandTranslator : public PluginComponent, public NetworkTranslator
{
public:
    ${GAME_NAME}_GameCommandTranslator()
    { }

    ~${GAME_NAME}_GameCommandTranslator()
    { }

public:
    virtual void start()
    {
        SharedPtr<PluginComponent> component = PluginManager::instance()->getRegistry()->queryComponent("com.zillians.module.world-gateway.main");
        mWorldGateway = boost::static_pointer_cast<WorldGatewayComponent>(component);
        SharedPtr<NetworkTranslator> this_translator = boost::dynamic_pointer_cast<NetworkTranslator>(shared_from_this());

        // GENERATE_BEGIN
        //[[[code
        foreach(i, AtClientFuncList)
        {
            std::wstring function_name = (*i)->name->toString();
        	//[[[gen(replace:GAME_NAME=game_name, FUNCTION_NAME=function_name)
            mWorldGateway->registerCommandTranslator(${GAME_NAME}_${FUNCTION_NAME}_CloudCommandObject::TYPE, this_translator);
        	//]]]
        }
        foreach(i, AtServerFuncList)
        {
            std::wstring function_name = (*i)->name->toString();
        	//[[[gen(replace:GAME_NAME=game_name, FUNCTION_NAME=function_name)
            mWorldGateway->registerCommandTranslator(${GAME_NAME}_${FUNCTION_NAME}_ClientCommandObject::TYPE, this_translator);
        	//]]]
        }
        //]]]
        // GENERATE_END
    }

    virtual void stop()
    {
        // GENERATE_BEGIN
        //[[[code
        foreach(i, AtClientFuncList)
        {
            std::wstring function_name = (*i)->name->toString();
        	//[[[gen(replace:GAME_NAME=game_name, FUNCTION_NAME=function_name)
            mWorldGateway->unregisterCommandTranslator(${GAME_NAME}_${FUNCTION_NAME}_CloudCommandObject::TYPE);
        	//]]]
        }
        foreach(i, AtServerFuncList)
        {
            std::wstring function_name = (*i)->name->toString();
        	//[[[gen(replace:GAME_NAME=game_name, FUNCTION_NAME=function_name)
            mWorldGateway->unregisterCommandTranslator(${GAME_NAME}_${FUNCTION_NAME}_ClientCommandObject::TYPE);
        	//]]]
        }
        //]]]
        // GENERATE_END
        mWorldGateway.reset();
    }

private:
    inline bool translateExternalId(const UUID& external, uint32& internal)
    {
        bool result = false;
        boost::tie(internal, result) = mWorldGateway->findInternalId(external);
        if(!result)
        {
            return false;
        }
        return true;
    }

    inline bool translateInternalId(const uint32& internal, UUID& external)
    {
        bool result = false;
        boost::tie(external, result) = mWorldGateway->findExternalId(internal);
        if(!result)
        {
            return false;
        }
        return true;
    }

    // GENERATE_BEGIN
    //[[[code
    foreach(i, AtClientFuncList)
    {
        std::wstring function_name = (*i)->name->toString();
    	//[[[gen(replace:GAME_NAME=game_name, FUNCTION_NAME=function_name)
        inline bool assign(${GAME_NAME}_${FUNCTION_NAME}_CloudCommandObject& src, ${GAME_NAME}_${FUNCTION_NAME}_ClientCommandObject& dest)
        {
            // GENERATE_BEGIN
            //[[[code
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
        	//]]]
            // GENERATE_END

            return true;
        }
    	//]]]
    }
    foreach(i, AtServerFuncList)
    {
        std::wstring function_name = (*i)->name->toString();
    	//[[[gen(replace:GAME_NAME=game_name, FUNCTION_NAME=function_name)
        inline bool assign(${GAME_NAME}_${FUNCTION_NAME}_ClientCommandObject& src, ${GAME_NAME}_${FUNCTION_NAME}_CloudCommandObject& dest)
        {
            // GENERATE_BEGIN
            //[[[code
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
        	//]]]
            // GENERATE_END

            return true;
        }
    	//]]]
    }
    //]]]
    // GENERATE_END

public:
    bool translate(uint32 type_in, Buffer& data_in, uint32& type_out, Buffer& data_out)
    {
        return translate_t(type_in, data_in, type_out, data_out);
    }

    bool translate(uint32 type_in, Buffer& data_in, uint32& type_out, SpscCircularBuffer& data_out)
    {
        return translate_t(type_in, data_in, type_out, data_out);
    }

    bool translate(uint32 type_in, SpscCircularBuffer& data_in, uint32& type_out, Buffer& data_out)
    {
        return translate_t(type_in, data_in, type_out, data_out);
    }

    bool translate(uint32 type_in, SpscCircularBuffer& data_in, uint32& type_out, SpscCircularBuffer& data_out)
    {
        return translate_t(type_in, data_in, type_out, data_out);
    }

    template<typename inputBuffer, typename outputBuffer>
    bool translate_t(uint32 type_in, inputBuffer& data_in, uint32& type_out, outputBuffer& data_out)
    {
        switch(type_in)
        {
        // GENERATE_BEGIN
        //[[[code
        foreach(i, AtClientFuncList)
        {
            std::wstring function_name = (*i)->name->toString();
            //[[[gen(replace:GAME_NAME=game_name, FUNCTION_NAME=function_name)
			case ${GAME_NAME}_${FUNCTION_NAME}_CloudCommandObject::TYPE:
			{
				${GAME_NAME}_${FUNCTION_NAME}_CloudCommandObject src;
				${GAME_NAME}_${FUNCTION_NAME}_ClientCommandObject dest;
				type_out = ${GAME_NAME}_${FUNCTION_NAME}_ClientCommandObject::TYPE;
				data_in >> src;
				if(!assign(src, dest))
				{
					// TODO: log here
					return false;
				}
				data_out << dest;
				return true;
			}
			//]]]
        }
        foreach(i, AtServerFuncList)
        {
            std::wstring function_name = (*i)->name->toString();
            //[[[gen(replace:GAME_NAME=game_name, FUNCTION_NAME=function_name)
			case ${GAME_NAME}_${FUNCTION_NAME}_ClientCommandObject::TYPE:
			{
				${GAME_NAME}_${FUNCTION_NAME}_ClientCommandObject src;
				${GAME_NAME}_${FUNCTION_NAME}_CloudCommandObject dest;
				type_out = ${GAME_NAME}_${FUNCTION_NAME}_CloudCommandObject::TYPE;
				data_in >> src;
				if(!assign(src, dest))
				{
					// TODO: log here
					return false;
				}
				data_out << dest;
				return true;
			}
			//]]]
        }
        //]]]
        // GENERATE_END
        default:
        {
             throw "the function Id is wrong";
             return false;
        }
        }
    }

private:
    SharedPtr<WorldGatewayComponent> mWorldGateway;
};

DECLARE_COMPONENT_IMPL(${GAME_NAME}_GameCommandTranslator, ${TRANSLATOR_UUID});
//]]]

//]]]
