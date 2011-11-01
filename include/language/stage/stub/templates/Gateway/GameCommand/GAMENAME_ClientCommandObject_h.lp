//[[[global

#include "language/stage/transformer/context/ManglingStageContext.h"
#include "language/tree/ASTNodeHelper.h"
#include "language/tree/ASTNode.h"
#include "language/tree/visitor/GenericDoubleVisitor.h"
#include "utility/Foreach.h"
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

using namespace zillians::language;
using namespace zillians::language::tree; // needed by CREATE_INVOKER

struct AtClientFuncGeneratorVisitor : public visitor::GenericDoubleVisitor
{
    CREATE_INVOKER(generateInvoker, generate)

    std::vector<FunctionDecl*>& mFuncList;

    AtClientFuncGeneratorVisitor(std::vector<FunctionDecl*>& FuncList) : mFuncList(FuncList)
    {
        REGISTER_ALL_VISITABLE_ASTNODE(generateInvoker)
    }

    void generate(ASTNode& node)
    {
        revisit(node);
    }

    void generate(FunctionDecl& node)
    {
        if(ASTNodeHelper::findAnnotation(&node, L"client"))
            mFuncList.push_back(&node);
    }
};

struct AtServerFuncGeneratorVisitor : public visitor::GenericDoubleVisitor
{
    CREATE_INVOKER(generateInvoker, generate)

    std::vector<FunctionDecl*>& mFuncList;

    AtServerFuncGeneratorVisitor(std::vector<FunctionDecl*>& FuncList) : mFuncList(FuncList)
    {
        REGISTER_ALL_VISITABLE_ASTNODE(generateInvoker)
    }

    void generate(ASTNode& node)
    {
        revisit(node);
    }

    void generate(FunctionDecl& node)
    {
        if(ASTNodeHelper::findAnnotation(&node, L"server"))
            mFuncList.push_back(&node);
    }
};

//]]]

//[[[entry
void gen_gateway_stub(Package* node)
{
//[[[driver
//]]]
}
//]]]

#include "vw/services/api/ApiTypes.h"
#include "objects/ClientCommandObject.h"
namespace zillians{

//[[[code
std::wstring GameName = L"Dummy";
std::vector<FunctionDecl*> AtClientFuncList;
std::vector<FunctionDecl*> AtServerFuncList;
{
    AtClientFuncGeneratorVisitor v(AtClientFuncList); v.visit(*node);
}
{
    AtServerFuncGeneratorVisitor v(AtServerFuncList); v.visit(*node);
}

auto f = [&](FunctionDecl* node){
    std::wstring FuncName = node->name->toString();
    std::wstringstream ss;
    ss << std::hex << stage::SymbolIdManglingContext::get(node)->managled_id;
    std::wstring FuncID = ss.str();

    //[[[gen(replace:GAME_NAME=GameName, FUNCTION_NAME=FuncName, FUNCTION_ID=FuncID)
    class ${GAME_NAME}_${FUNCTION_NAME}_ClientCommandObject : public ClientCommandObject
    {
    public:
        ${GAME_NAME}_${FUNCTION_NAME}_ClientCommandObject()
        { }

        ~${GAME_NAME}_${FUNCTION_NAME}_ClientCommandObject()
        { }

        enum 
        { 
            // GENERATE_BEGIN
            TYPE = ${FUNCTION_ID} // the type need to be unique globally (across all RPC command types)
            // GENERATE_END
        };

    public:
        // GENERATE_BEGIN
        //[[[code
        foreach(j, node->parameters)
            std::wcout << ASTNodeHelper::getNodeName((*j)->type) << L" " << (*j)->name->toString() << L";" << std::endl;
        //]]]
        // GENERATE_END

    public:
        template<typename Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            ClientCommandObject::serialize(ar, version); // 16 bytes
            // GENERATE_BEGIN
            //[[[code
            foreach(j, node->parameters)
                std::wcout << L"ar & " << (*j)->name->toString() << L";" << std::endl;
            //]]]
            // GENERATE_END
        }
    };
    //]]]
};
foreach(i, AtClientFuncList)
{
    f(*i);
}
foreach(i, AtServerFuncList)
{
    f(*i);
}
//]]]

} // namespace zillians

/*
#include "vw/services/api/ApiTypes.h" 
#include "objects/ClientCommandObject.h" 
namespace zillians {

class ${GAME_NAME}_${CLIENT_FUNCTION_NAME}_ClientCommandObject : public ClientCommandObject
{
public:
    ${GAME_NAME}_${CLIENT_FUNCTION_NAME}_ClientCommandObject()
    { }

    ~${GAME_NAME}_${CLIENT_FUNCTION_NAME}_ClientCommandObject()
    { }

    enum 
    { 
        // GENERATE_BEGIN
        TYPE = 0x01 // the type need to be unique globally (across all RPC command types)
        // GENERATE_END
    };

public:
    // GENERATE_BEGIN
    uint32 x;
    float y;
    UUID other;
    // GENERATE_END

public:
    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ClientCommandObject::serialize(ar, version); // 16 bytes
        // GENERATE_BEGIN
        ar & x;
        ar & y;
        ar & other;
        // GENERATE_END
    }
};

class ${GAME_NAME}_${SERVER_FUNCTION_NAME}_ClientCommandObject : public ClientCommandObject
{
public:
    ${GAME_NAME}_${SERVER_FUNCTION_NAME}_ClientCommandObject()
    { }

    ~${GAME_NAME}_${SERVER_FUNCTION_NAME}_ClientCommandObject()
    { }

    enum 
    { 
        // GENERATE_BEGIN
        TYPE = 0x02 // the type need to be unique globally (across all RPC command types)
        // GENERATE_END
    };

public:
    // GENERATE_BEGIN
    uint32 x;
    float y;
    UUID other;
    // GENERATE_END

public:
    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ClientCommandObject::serialize(ar, version); // 16 bytes
        // GENERATE_BEGIN
        ar & x;
        ar & y;
        ar & other;
        // GENERATE_END
    }
};
} // namespace zillians
*/
