//[[[global

#include <iostream>

/*
#include "language/stage/transformer/context/ManglingStageContext.h"
#include "language/tree/ASTNodeHelper.h"
#include <sstream.h>
#include <string>

using namespace zillians::language;

void f(FunctionDecl& node);
struct GeneratorVisitor : GenericDoubleVisitor
{
    CREATE_INVOKER(generateInvoker, generate)

    std::wstring GameName; // TODO: need spec

    GeneratorVisitor()
    {
        REGISTER_ALL_VISITABLE_ASTNODE(generateInvoker)
    }

    void generate(ASTNode& node)
    {
        revisit(node);
    }

    void generate(FunctionDecl& node)
    {
    	f(node);
    }
};
*/

//]]]

#include "vw/services/api/ApiTypes.h"
#include "objects/ClientCommandObject.h"
namespace zillians{

//[[[code
//GeneratorVisitor v; v.visit(*node);
//]]]

} // namespace zillians

//[[[entry(auto)
//]]]

void f(FunctionDecl& node)
{
    if(tree::ASTNodeHelper::findAnnotation(&node, L"client") || tree::ASTNodeHelper::findAnnotation(&node, L"server"))
    {
        std::wstring FuncName = node.name->toString();
        std::wstringstream ss;
        ss << std::hex << stage::SymbolIdManglingContext::get(&node)->managled_id;
        std::string FuncID = ss.str();

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
			foreach(i, node.parameters)
				std::wcout << tree::getNodeName((*i)->type) << L" " << (*i)->name->toString() << L";" << std::endl;
			//]]]
			// GENERATE_END
		
		public:
			template<typename Archive>
			void serialize(Archive& ar, const unsigned int version)
			{
				ClientCommandObject::serialize(ar, version); // 16 bytes
				// GENERATE_BEGIN
				//[[[code
				foreach(i, node.parameters)
					std::wcout << L"ar & " << (*i)->name->toString() << L";" << std::endl;
				//]]]
				// GENERATE_END
			}
		};
		//]]]
    }
}

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
