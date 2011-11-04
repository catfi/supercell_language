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
