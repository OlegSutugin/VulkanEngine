#pragma once

#include "Actor.h"
#include "Core/Subsystems/WorldSubsystems/MeshSubsystem.h"

namespace VulkanEngine
{

class PrimitiveMeshActor : public Actor
{
public:
    PrimitiveMeshActor(World* world, uint32_t id, const Transform& transform, MeshPrimitive type);
    virtual ~PrimitiveMeshActor();
};

}  // namespace VulkanEngine