#include "plugin/PluginComponent.h" 
#include "extension-point/NetworkTranslator.h" 
#include "core-api/Prerequisite.h" 
#include "modules/WorldGatewayComponent.h" 

using namespace zillians::world;
using namespace zillians::plugin;

class ${GAME_NAME}_GameObjectTranslator : public PluginComponent, public NetworkTranslator
{
public:
    ${GAME_NAME}_GameObjectTranslator()
    { }

    ~${GAME_NAME}_GameObjectTranslator()
    { }

public:
    virtual void start()
    { 
        SharedPtr<PluginComponent> component = PluginManager::instance()->getRegistry()->queryComponent("com.zillians.module.world-gateway.main");
        mWorldGateway = boost::static_pointer_cast<WorldGatewayComponent>(component);

        SharedPtr<NetworkTranslator> this_translator = boost::dynamic_pointer_cast<NetworkTranslator>(shared_from_this());
        mWorldGateway->registerObjectTranslator(${GAME_NAME}_${OBJECT_NAME}_CloudGameObject::TYPE, this_translator);
    }

    virtual void stop()
    { 
        mWorldGateway->unregisterObjectTranslator(${GAME_NAME}_${OBJECT_NAME}_CloudGameObject::TYPE);
        mWorldGateway.reset();
    }

private:
    inline bool translateExternalId(const UUID& external, uint32& internal)
    {
        bool result = false;
        boost::tie(internal, result) = mWorldGateway->findInternalId(external);
        if(!result)
        {
            return false;
        }
        return true;
    }

    // GENERATE_BEGIN
    inline bool assign(${GAME_NAME}_${OBJECT_NAME}_CloudGameObject& src, ${GAME_NAME}_${OBJECT_NAME}_ClientGameObject& dest)
    {
        // external id to internal id translation
        translateExternalId(src.id, dest.id);
        dest.x = src.x;
        dest.y = src.y;
        dest.r = src.r;
        dest.dx = src.dx;
        dest.dy = src.dy;
        dest.dr = src.dr;
        if(!translateExternalId(src.pet, dest.pet)) return false;
        return true;
    }
    // GENERATE_END

public:
    virtual bool translate(uint32 type_in, Buffer& data_in, uint32& type_out, Buffer& data_out)
    {
        switch(type_in)
        {
        // GENERATE_BEGIN
        case ${GAME_NAME}_${OBJECT_NAME}_ClientGameObject::TYPE
        {
            ${GAME_NAME}_${OBJECT_NAME}_CloudGameObject src;
            ${GAME_NAME}_${OBJECT_NAME}_ClientGameObject dest;
            type_out = CloudDemoGameObject::TYPE;
            data_in >> src;
            if(!assign(src, dest))
            {
                return false;
            }
            data_out << dest;
            break;
        }
        // GENERATE_END
        }
    }

private:
    SharedPtr<WorldGatewayComponent> mWorldGateway;
};
