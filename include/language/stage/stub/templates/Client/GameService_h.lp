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
std::string get_stub_filename<stage::ThorScriptStubStage::CLIENT_GAMESERVICE_H>(stage::ThorScriptStubStage::var_map_t& var_map)
{
    return "GameService.h";
}
template<>
void print_stub<stage::ThorScriptStubStage::CLIENT_GAMESERVICE_H>(Tangle* node, stage::ThorScriptStubStage::var_map_t& var_map)
{
//[[[driver
//]]]
}
//]]]

//]]]

#include "zillians/BaseService.h" 
#include "ClientStub.h" 

class GameService : public zillians::BaseService
{
public:
    virtual void onClientCallReceived(zillians::uint32 fid, zillians::Buffer& buf);
    void setxx_ClientStub(xx_ClientStub* stub) { mxxStub = stub; }
    zillians::uint64 getServiceId() {return 0/*TODO This value should be generated*/;}
protected:
    xx_ClientStub* mxxStub;
};//GameService

//[[[code
std::vector<FunctionDecl*> AtServerFuncList;
{ AtFuncGeneratorVisitor v(AtServerFuncList, L"server"); v.visit(*node); }

auto f = [&](FunctionDecl* node){
        //[[[gen
        //GameService.h
        #include "zillians/BaseService.h" 
        
        class GameService : public BaseService
        {
        public:
            //Enclosing packages
            class PackageAAA
            {
            public:
                //Enclosing packages
                class PackageBBB
                {
                public:
                    //Enclosing packages
                    //Members
                    GameService* mGS;
        
                    //@server calls
                    void SomeRemoteCall(zillians::uint32 v, zillians::float32 t, AAA::BBB::Tank &tank);
        
                    //Constructor
                    PackageBBB(GameService* gs) : mGS(gs) {}
                };//BBB
        
                //Members
                PackageBBB BBB;
                GameService* mGS;
        
                //@server calls
        
                //Constructor
                PackageAAA(GameService* gs) : mGS(gs), BBB(gs) {}
            };//AAA
            PackageAAA AAA;
        
        public:
            GameService() : BaseService(), AAA(this) {} // Assign game service to package in root scope
        };
        //]]]
    };
foreach(i, AtServerFuncList) f(*i);
//]]]
