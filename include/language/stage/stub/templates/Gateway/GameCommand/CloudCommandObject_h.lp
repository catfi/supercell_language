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
