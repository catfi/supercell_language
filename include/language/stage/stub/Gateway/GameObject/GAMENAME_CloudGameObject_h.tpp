class ${GAME_NAME}_${OBJECT_NAME}_CloudGameObject : public CloudGameObject
{
public:
    ${GAME_NAME}_${OBJECT_NAME}_CloudGameObject()
    { }

    ~${GAME_NAME}_${OBJECT_NAME}_CloudGameObject()
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
    uint32 pet; // NOTE HERE: UUID <--> uint32 translation is mandatory in the client-cloud conversion
    // GENERATE_END

public:
    template<typename Archive>
    void serialize(Archive& ar, const unsigned int version)
    {
        CloudGameObject::serialize(ar, version); // 4 bytes
        // GENERATE_BEGIN
        ar & x;
        ar & y;
        ar & r;
        ar & dx;
        ar & dy;
        ar & dr;
        ar & pet;
        ar.skip(x);  
        // GENERATE_END
    }
}
