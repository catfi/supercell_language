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
