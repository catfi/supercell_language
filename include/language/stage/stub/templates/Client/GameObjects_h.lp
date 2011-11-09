#ifndef GAMEOBJECTS_H_
#define GAMEOBJECTS_H_

#include "zillians/GameObject.h" 

namespace xxx { namespace yyy {

class Sinbad : public zillians::GameObject
{
public:
    zillians::float32 x;
    zillians::float32 y;
    zillians::float32 z;
};

class SinbadFactory : public zillians::GameObjectFactory
{
public:
    zillians::GameObject* create() { return new Sinbad(); }
};

} }

#endif
