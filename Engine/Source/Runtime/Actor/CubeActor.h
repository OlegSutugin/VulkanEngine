#pragma once

#include "Actor.h"

namespace VulkanEngine
{

class CubeActor : public Actor
{
public:
    CubeActor(World* world, uint32_t id, const Transform& transform);
    virtual ~CubeActor();
};

}  // namespace VulkanEngine