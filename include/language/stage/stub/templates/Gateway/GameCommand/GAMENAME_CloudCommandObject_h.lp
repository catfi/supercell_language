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

//[[[codeenum
{
    MAX_BYTE_COUNT = 128
};
std::wstring GameName = L"Dummy";
std::vector<FunctionDecl*> AtClientFuncList;
std::vector<FunctionDecl*> AtServerFuncList;
{
    AtFuncGeneratorVisitor v(AtClientFuncList, L"client"); v.visit(*node);
}
{
    AtFuncGeneratorVisitor v(AtServerFuncList, L"server"); v.visit(*node);
}

auto f = [&](FunctionDecl* node){
    std::wstring FuncName = node->name->toString();
    std::wstringstream ss;
    ss << std::hex << stage::SymbolIdManglingContext::get(node)->managled_id;
    std::wstring FuncID = ss.str();

    //[[[gen(replace:GAME_NAME=GameName, FUNCTION_NAME=FuncName, FUNCTION_ID=FuncID)
    class ${GAME_NAME}_${FUNCTION_NAME}_CloudCommandObject : public CloudCommandObject
    {
    public:
        ${GAME_NAME}_${FUNCTION_NAME}_CloudCommandObject()
        { }

        ~${GAME_NAME}_${FUNCTION_NAME}_CloudCommandObject()
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
        {
            size_t ByteCount = 0;
            foreach(i, node->parameters)
            {
                std::wstring TypeName;
                if(ASTNodeHelper::isUUID((*i)->type))
                    TypeName = L"api_types::api_uint32";
                else
                    TypeName = ASTNodeHelper::getNodeName((*i)->type);
                std::wcout << TypeName << L" " << (*i)->name->toString() << L";" << std::endl;
                ByteCount += 0;
            }
            if(ByteCount < MAX_BYTE_COUNT)
                std::wcout << L"ar.skip(" << (MAX_BYTE_COUNT-ByteCount) << L");" << std::endl;
        }
        //]]]
        // GENERATE_END

    public:
        template<typename Archive>
        void serialize(Archive& ar, const unsigned int version)
        {
            //CloudCommandObject::serialize(ar, version); // 16 bytes
            // GENERATE_BEGIN
            //[[[code
            {
                size_t ByteCount = 0;
                foreach(i, node->parameters)
                {
                    std::wcout << L"ar & " << (*i)->name->toString() << L";" << std::endl;
                    ByteCount += 0;
                }
                if(ByteCount < MAX_BYTE_COUNT)
                    std::wcout << L"ar.skip(" << (MAX_BYTE_COUNT-ByteCount) << L");" << std::endl;
            }
            //]]]
            // GENERATE_END
        }
    };
    //]]]
};
foreach(i, AtClientFuncList) f(*i);
foreach(i, AtServerFuncList) f(*i);
//]]]

} // namespace zillians
