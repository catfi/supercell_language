//[[[global
#include <iostream>

//[[[entry
void gen_gateway_stub()
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
