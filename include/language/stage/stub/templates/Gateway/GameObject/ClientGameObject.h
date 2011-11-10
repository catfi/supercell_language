#ifndef GATEWAY_GAMEOBJECT_CLIENTGAMEOBJECT_H_
#define GATEWAY_GAMEOBJECT_CLIENTGAMEOBJECT_H_

class ClientGameObject
{
public:
    ClientGameObject()
    { }
    virtual ~ClientGameObject()
    { }

public:
    UUID id; // external game object id

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & id;
    }
};

#endif
