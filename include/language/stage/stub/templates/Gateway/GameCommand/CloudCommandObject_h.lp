//[[[global
#include "language/stage/stub/ThorScriptStubStage.h"
#include <iostream>

using namespace zillians::language;
using namespace zillians::language::tree;

//[[[entry
template<>
void genStub<stage::ThorScriptStubStage::CLOUDCOMMANDOBJECT_H>(Package* node)
{
//[[[driver
//]]]
}
//]]]

//]]]

class CloudCommandObject
{
public:
    CloudCommandObject()
    { }
    virtual ~CloudCommandObject()
    { }

public:
    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
    }
};
