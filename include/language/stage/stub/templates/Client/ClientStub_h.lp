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
std::string get_stub_filename<stage::ThorScriptStubStage::CLIENT_CLIENTSTUB_H>(stage::ThorScriptStubStage::var_map_t& var_map)
{
    return "ClientStub.h";
}
template<>
void print_stub<stage::ThorScriptStubStage::CLIENT_CLIENTSTUB_H>(Tangle* node, stage::ThorScriptStubStage::var_map_t& var_map)
{
//[[[driver
//]]]
}
//]]]

//]]]

#include "zillians/Base.h"
#include "GameObjects.h"

class xx_ClientStub
{
public:

//[[[code
std::vector<FunctionDecl*> AtClientFuncList;
{ AtFuncGeneratorVisitor v(AtClientFuncList, L"client"); v.visit(*node); }

auto f = [&](FunctionDecl* node){
        std::wstring function_name = node->name->toString();
        std::wstringstream ss;
        ss << L"0x" << std::hex << stage::SymbolIdManglingContext::get(node)->managled_id;
        std::wstring function_id = ss.str();

        //[[[gen(replace:FUNCTION_NAME=function_name, FUNCTION_ID=function_id)
        virtual void ${FUNCTION_NAME}(
            // GENERATE_BEGIN
            //[[[code
            foreach(i, node->parameters)
            {
                if((*i)->type->type == TypeSpecifier::ReferredType::UNSPECIFIED)
                    std::wcout << ASTNodeHelper::getNodeName((*i)->type) << L"& " << (*i)->name->toString() << L"," << std::endl;
                else
                    std::wcout << L"zillians::" << ASTNodeHelper::getNodeName((*i)->type) << L" " << (*i)->name->toString() << L"," << std::endl;
            }
            //]]]
            // GENERATE_END
            ) = 0;
        //]]]
    };
foreach(i, AtClientFuncList) f(*i);
//]]]

};
