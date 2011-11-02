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
