#ifndef GATEWAY_GAMECOMMAND_CLIENTCOMMANDOBJECT_H_
#define GATEWAY_GAMECOMMAND_CLIENTCOMMANDOBJECT_H_

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

#endif
