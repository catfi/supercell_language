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
std::string get_stub_filename<stage::ThorScriptStubStage::CLIENT_GAMEOBJECTS_H>(stage::ThorScriptStubStage::var_map_t& var_map)
{
    return "GameObjects.h";
}
template<>
void print_stub<stage::ThorScriptStubStage::CLIENT_GAMEOBJECTS_H>(Tangle* node, stage::ThorScriptStubStage::var_map_t& var_map)
{
//[[[driver
//]]]
}
//]]]

//]]]

#ifndef GAMEOBJECTS_H_
#define GAMEOBJECTS_H_
#include "zillians/GameObject.h" 

namespace xxx { namespace yyy {
class Sinbad : public zillians::GameObject
{
public:
    zillians::float32 x;
    zillians::float32 y;
    zillians::float32 z;
};//Sinbad
class SinbadFactory : public zillians::GameObjectFactory
{
public:
    zillians::GameObject* create() { return new Sinbad(); }
};//SinbadFactory
} }//

#endif//GAMEOBJECTS_H_
