#ifndef CLOUDGAMEOBJECT_H_
#define CLOUDGAMEOBJECT_H_

class CloudGameObject
{
public:
    CloudGameObject()
    { }
    virtual ~CloudGameObject()
    { }

public:
    uint32 id; // internal game object id

    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ar & id;
    }
};

#endif
