///[[[signature void generate(std::wstringstream& ss, ASTNode* node)

///[[[global

#include "language/stage/transformer/context/ManglingStageContext.h"
#include "language/tree/ASTNodeHelper.h"
#include <sstream.h>
#include <string>

using namespace zillians::language;

struct GeneratorVisitor : GenericDoubleVisitor
{
    CREATE_INVOKER(generateInvoker, generate)

    enum
    {
        MAX_BYTE_COUNT = 128
    };

    std::wstring GameName;

    GeneratorVisitor() : GameName(L"0") // TODO: need spec
    {
        REGISTER_ALL_VISITABLE_ASTNODE(generateInvoker)
    }

    void generate(ASTNode& node)
    {
        revisit(node);
    }

    void generate(FunctionDecl& node)
    {
        if(tree::ASTNodeHelper::findAnnotation(&node, L"client") || tree::ASTNodeHelper::findAnnotation(&node, L"server"))
        {
            std::wstring FuncName = node.name->toString();
            std::wstringstream ss;
            ss << std::hex << stage::SymbolIdManglingContext::get(&node)->managled_id;
            std::string FuncID = ss.str();

            ///[[[gen( GAME_NAME=GameName, FUNCTION_NAME=FuncName, FUNCTION_ID=FuncID )
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
                ///[[[code
                size_t ByteCount = 0;
                foreach(i, node.parameters)
                {
                    std::wstring TypeName;
                    if(tree::ASTNodeHelper::isUUID((*i)->type))
                        TypeName = L"api_types::api_uint32";
                    else
                        TypeName = tree::getNodeName((*i)->type);
                    std::wcout << TypeName << L" " << (*i)->name->toString() << L";" << std::endl;
                    ByteCount += 0;
                }
                if(ByteCount < MAX_BYTE_COUNT)
                    std::wcout << L"ar.skip(" << (MAX_BYTE_COUNT-ByteCount) << L");" << std::endl;
                ///]]]
                // GENERATE_END

            public:
                template<typename Archive>
                void serialize(Archive& ar, const unsigned int version)
                {
                    //CloudCommandObject::serialize(ar, version); // 16 bytes
                    // GENERATE_BEGIN
                    ///[[[code
                    size_t ByteCount = 0;
                    foreach(i, node.parameters)
                    {
                        std::wcout << L"ar & " << (*i)->name->toString() << L";" << std::endl;
                        ByteCount += 0;
                    }
                    if(ByteCount < MAX_BYTE_COUNT)
                        std::wcout << L"ar.skip(" << (MAX_BYTE_COUNT-ByteCount) << L");" << std::endl;
                    ///]]]
                    // GENERATE_END
                }
            };
            ///]]]
        }
    }
};

///]]]

#include "vw/services/api/ApiTypes.h"
namespace zillians{

///[[[code
GeneratorVisitor v; v.visit(*node);
///]]]

} // namespace zillians

/*
#include "vw/services/api/ApiTypes.h" 
namespace zillians {

class ${GAME_NAME}_${CLIENT_FUNCTION_NAME}_CloudCommandObject 
{
public:
    ${GAME_NAME}_${CLIENT_FUNCTION_NAME}_CloudCommandObject()
    { }

    ~${GAME_NAME}_${CLIENT_FUNCTION_NAME}_CloudCommandObject()
    { }

    enum 
    { 
        // GENERATE_BEGIN
        TYPE = 0x01 // the type need to be unique globally (across all RPC command types)
        // GENERATE_END
    };

public:
    // GENERATE_BEGIN
    api_types::api_float32 x; // api_types::api_only for float32 and float64
    api_types::api_float32 y;
    uint32 other; // NOTE HERE: UUID <--> uint32 translation is mandatory in the client-cloud conversion to avoid easy-hacking
    // GENERATE_END

public:
    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        //CloudCommandObject::serialize(ar, version); // 0 bytes
        // GENERATE_BEGIN
        ar & x;     // 4 bytes
        ar & y;     // 4 bytes
        ar & other; // 16 bytes
        ar.skip(8); // append to 112 bytes aligned
        // GENERATE_END
    }
};

class ${GAME_NAME}_${SERVER_FUNCTION_NAME}_CloudCommandObject 
{
public:
    ${GAME_NAME}_${SERVER_FUNCTION_NAME}_CloudCommandObject()
    { }

    ~${GAME_NAME}_${SERVER_FUNCTION_NAME}_CloudCommandObject()
    { }

    enum 
    { 
        // GENERATE_BEGIN
        TYPE = 0x01 // the type need to be unique globally (across all RPC command types)
        // GENERATE_END
    };

public:
    // GENERATE_BEGIN
    float x;
    float y;
    uint32 other; // NOTE HERE: UUID <--> uint32 translation is mandatory in the client-cloud conversion to avoid easy-hacking
    // GENERATE_END

public:
    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        //CloudCommandObject::serialize(ar, version); // 0 bytes
        // GENERATE_BEGIN
        ar & x;     // 4 bytes
        ar & y;     // 4 bytes
        ar & other; // 16 bytes
        ar.skip(8); // append to 112 bytes aligned
        // GENERATE_END
    }
};
} // namespace zillians
*/
