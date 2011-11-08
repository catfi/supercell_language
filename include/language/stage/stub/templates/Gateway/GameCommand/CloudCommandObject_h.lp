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
std::string genStubFilename<stage::ThorScriptStubStage::CLOUDCOMMANDOBJECT_H>(stage::ThorScriptStubStage::var_map_t& var_map)
{
    auto f = [&](std::wstring &s, std::wstring search_string, std::wstring replace_string){
            for(std::wstring::size_type p = 0; (p = s.find(search_string, p)) != std::string::npos; p++)
                s.replace(p, search_string.size(), replace_string);
        };
    std::wstring s = L"${GAME_NAME}_CLOUDCOMMANDOBJECT.h";
    f(s, L"${GAME_NAME}", var_map[L"game-name"]);
    return zillians::ws_to_s(s);
}
template<>
void genStub<stage::ThorScriptStubStage::CLOUDCOMMANDOBJECT_H>(Tangle* node, stage::ThorScriptStubStage::var_map_t& var_map)
{
//[[[driver
//]]]
}
//]]]

//]]]

#include "vw/services/api/ApiTypes.h"
#include "CloudCommandObject.h"

namespace zillians {

//[[[code
enum { MAX_BYTE_COUNT = 128 };

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

    //[[[gen(replace:GAME_NAME=game_name, FUNCTION_NAME=function_name, FUNCTION_ID=function_id)
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
        foreach(i, node->parameters)
            std::wcout << ASTNodeHelper::getNodeName((*i)->type) << L" " << (*i)->name->toString() << L";" << std::endl;
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
