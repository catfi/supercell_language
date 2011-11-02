class ${GAME_NAME}_${OBJECT_NAME}_ClientGameObject : public ClientGameObject
{
public:
    ${GAME_NAME}_${OBJECT_NAME}_ClientGameObject()
    { }

    ~${GAME_NAME}_${OBJECT_NAME}_ClientGameObject()
    { }

    enum 
    { 
        // GENERATE_BEGIN
        TYPE = 0x01 // the type need to be unique globally (across all object types)
        // GENERATE_END
    };

public:

    // GENERATE_BEGIN
    float x;
    float y;
    float r;
    float dx;
    float dy;
    float dr;
    UUID pet;
    // GENERATE_END

public:
    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        ClientGameObject::serialize(ar, version); // 16 bytes
        // GENERATE_BEGIN
        ar & x;
        ar & y;
        ar & r;
        ar & dx;
        ar & dy;
        ar & dr;
        ar & pet;
        // GENERATE_END
    }
};
