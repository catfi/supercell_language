//[[[global
#include "language/stage/stub/ThorScriptStubStage.h"
#include <iostream>

using namespace zillians::language;
using namespace zillians::language::tree;

//[[[entry
template<>
void genStub<stage::ThorScriptStubStage::CLIENTCOMMANDOBJECT_H>(Tangle* node)
{
//[[[driver
//]]]
}
//]]]

//]]]

class ClientCommandObject
{
public:
    ClientCommandObject()
    { }
    virtual ~ClientCommandObject()
    { }

public:
    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    }
};
