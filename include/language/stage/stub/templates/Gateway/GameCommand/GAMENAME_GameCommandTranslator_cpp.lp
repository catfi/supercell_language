///[[[signature void generate(std::wstringstream& ss, ASTNode* node)

///[[[global

#include "language/stage/transformer/context/ManglingStageContext.h"
#include "language/tree/ASTNodeHelper.h"
#include <sstream.h>
#include <string>

using namespace zillians::language;

struct AtClientFuncGeneratorVisitor : GenericDoubleVisitor
{
    CREATE_INVOKER(generateInvoker, generate)

    std::vector<FunctionDecl*>& mFuncList;

    AtClientFuncGeneratorVisitor(std::vector<FunctionDecl*>& FuncList) : &mFuncList(FuncList)
    {
        REGISTER_ALL_VISITABLE_ASTNODE(generateInvoker)
    }

    void generate(ASTNode& node)
    {
        revisit(node);
    }

    void generate(FunctionDecl& node)
    {
        if(tree::ASTNodeHelper::findAnnotation(&node, L"client"))
            mFuncList.push_back(&node);
    }
};

struct AtServerFuncGeneratorVisitor : GenericDoubleVisitor
{
    CREATE_INVOKER(generateInvoker, generate)

    std::vector<FunctionDecl*>& mFuncList;

    AtServerFuncGeneratorVisitor(std::vector<FunctionDecl*>& FuncList) : &mFuncList(FuncList)
    {
        REGISTER_ALL_VISITABLE_ASTNODE(generateInvoker)
    }

    void generate(ASTNode& node)
    {
        revisit(node);
    }

    void generate(FunctionDecl& node)
    {
        if(tree::ASTNodeHelper::findAnnotation(&node, L"server"))
            mFuncList.push_back(&node);
    }
};

///]]]

///[[[code
std::vector<FunctionDecl*> AtClientFuncList;
std::vector<FunctionDecl*> AtServerFuncList;
{
    AtClientFuncGeneratorVisitor v(AtClientFuncList); v.visit(*node);
}
{
    AtServerFuncGeneratorVisitor v(AtServerFuncList); v.visit(*node);
}
std::wstring GameName; // TODO: need spec

    ///[[[gen( GAME_NAME=GameName, TRANSLATOR_UUID=TranslatorUUID )
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
            ///[[[code
            foreach(i, AtClientFuncList)
            {
                std::wstring FuncName = (*i)->name->toString();
            	///[[[gen( GAME_NAME=GameName, FUNCTION_NAME=FuncName )
                mWorldGateway->registerCommandTranslator(${GAME_NAME}_${FUNCTION_NAME}_CloudCommandObject::TYPE, this_translator);
            	///]]]
            }
            foreach(i, AtServerFuncList)
            {
                std::wstring FuncName = (*i)->name->toString();
            	///[[[gen( GAME_NAME=GameName, FUNCTION_NAME=FuncName )
                mWorldGateway->registerCommandTranslator(${GAME_NAME}_${FUNCTION_NAME}_ClientCommandObject::TYPE, this_translator);
            	///]]]
            }
            ///]]]
            // GENERATE_END
        }

        virtual void stop()
        {
            // GENERATE_BEGIN
            ///[[[code
            foreach(i, AtClientFuncList)
            {
                std::wstring FuncName = (*i)->name->toString();
            	///[[[gen( GAME_NAME=GameName, FUNCTION_NAME=FuncName )
                mWorldGateway->unregisterCommandTranslator(${GAME_NAME}_${FUNCTION_NAME}_CloudCommandObject::TYPE);
            	///]]]
            }
            foreach(i, AtServerFuncList)
            {
                std::wstring FuncName = (*i)->name->toString();
            	///[[[gen( GAME_NAME=GameName, FUNCTION_NAME=FuncName )
                mWorldGateway->unregisterCommandTranslator(${GAME_NAME}_${FUNCTION_NAME}_ClientCommandObject::TYPE);
            	///]]]
            }
            ///]]]
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
        ///[[[code
        foreach(i, AtClientFuncList)
        {
            std::wstring FuncName = (*i)->name->toString();
        	///[[[gen( GAME_NAME=GameName, FUNCTION_NAME=FuncName )
            inline bool assign(${GAME_NAME}_${FUNCTION_NAME}_CloudCommandObject& src, ${GAME_NAME}_${FUNCTION_NAME}_ClientCommandObject& dest)
            {
                // GENERATE_BEGIN
                ///[[[code
            	foreach(j, (*i)->parameters)
				{
            		std::wstring name = (*j)->name->toString();
                    if((*j)->type == tree::TypeSpecifier::ReferredType::UNSPECIFIED)
                    	std::wcout << L"if(!translateInternalId(src." << name << L", dest." << name << L")) return false;" << std::endl;
					else
						std::wcout << L"dest." << name << L" = src." << name << L";" << std::endl;
				}
            	///]]]
                // GENERATE_END

                return true;
            }
        	///]]]
        }
        foreach(i, AtServerFuncList)
        {
            std::wstring FuncName = (*i)->name->toString();
        	///[[[gen( GAME_NAME=GameName, FUNCTION_NAME=FuncName )
            inline bool assign(${GAME_NAME}_${FUNCTION_NAME}_ClientCommandObject& src, ${GAME_NAME}_${FUNCTION_NAME}_CloudCommandObject& dest)
            {
                // GENERATE_BEGIN
                ///[[[code
            	foreach(j, (*i)->parameters)
				{
            		std::wstring name = (*j)->name->toString();
                    if((*j)->type == tree::TypeSpecifier::ReferredType::UNSPECIFIED)
                    	std::wcout << L"if(!translateExternalId(src." << name << L", dest." << name << L")) return false;" << std::endl;
					else
						std::wcout << L"dest." << name << L" = src." << name << L";" << std::endl;
				}
            	///]]]
                // GENERATE_END

                return true;
            }
        	///]]]
        }
        ///]]]
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
            ///[[[code
            foreach(i, AtClientFuncList)
            {
                std::wstring FuncName = (*i)->name->toString();
                ///[[[gen( GAME_NAME=GameName, FUNCTION_NAME=FuncName )
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
				///]]]
            }
            foreach(i, AtServerFuncList)
            {
                std::wstring FuncName = (*i)->name->toString();
                ///[[[gen( GAME_NAME=GameName, FUNCTION_NAME=FuncName )
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
				///]]]
            }
            ///]]]
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
    ///]]]

///]]]

/*
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
        mWorldGateway->registerCommandTranslator(${GAME_NAME}_${CLIENT_FUNCTION_NAME}_CloudCommandObject::TYPE, this_translator);
        mWorldGateway->registerCommandTranslator(${GAME_NAME}_${SERVER_FUNCTION_NAME}_ClientCommandObject::TYPE, this_translator);
        // GENERATE_END
    }

    virtual void stop()
    { 
        // GENERATE_BEGIN
        mWorldGateway->unregisterCommandTranslator(${GAME_NAME}_${CLIENT_FUNCTION_NAME}_CloudCommandObject::TYPE);
        mWorldGateway->unregisterCommandTranslator(${GAME_NAME}_${SERVER_FUNCTION_NAME}_ClientCommandObject::TYPE);
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
    inline bool assign(${GAME_NAME}_${CLIENT_FUNCTION_NAME}_CloudCommandObject& src, ${GAME_NAME}_${CLIENT_FUNCTION_NAME}_ClientCommandObject& dest)
    {
        // GENERATE_BEGIN
        dest.x = src.x;
        dest.y = src.y;
        if(!translateInternalId(src.other, dest.other)) return false;
        // GENERATE_END

        return true;
    }

    inline bool assign(${GAME_NAME}_${SERVER_FUNCTION_NAME}_ClientCommandObject& src, ${GAME_NAME}_${SERVER_FUNCTION_NAME}_CloudCommandObject& dest)
    {
        // GENERATE_BEGIN
        dest.x = src.x;
        dest.y = src.y;
        if(!translateExternalId(src.other, dest.other)) return false;
        // GENERATE_END

        return true;
    }
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
        case ${GAME_NAME}_${CLIENT_FUNCTION_NAME}_CloudCommandObject::TYPE:
        {
            ${GAME_NAME}_${CLIENT_FUNCTION_NAME}_CloudCommandObject src;
            ${GAME_NAME}_${CLIENT_FUNCTION_NAME}_ClientCommandObject dest;
            type_out = ${GAME_NAME}_${CLIENT_FUNCTION_NAME}_ClientCommandObject::TYPE;
            data_in >> src;
            if(!assign(src, dest))
            {
                // TODO: log here
                return false;
            }
            data_out << dest;
            return true;
        }
        case ${GAME_NAME}_${SERVER_FUNCTION_NAME}_ClientCommandObject::TYPE:
        {
            ${GAME_NAME}_${SERVER_FUNCTION_NAME}_ClientCommandObject src;
            ${GAME_NAME}_${SERVER_FUNCTION_NAME}_CloudCommandObject dest;
            type_out = ${GAME_NAME}_${SERVER_FUNCTION_NAME}_CloudCommandObject::TYPE;
            data_in >> src;
            if(!assign(src, dest))
            {
                // TODO: log here
                return false;
            }
            data_out << dest; 
            return true;
        }
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
/*
