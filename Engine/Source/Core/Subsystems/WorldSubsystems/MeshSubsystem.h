#pragma once

#include "WorldSubsystem.h"
#include "Core/EngineTypes.h"
#include "Libraries/Geometry/MeshDescriptor.h"
#include "Render/IRenderer.h"

#include <unordered_map>
#include <functional>

namespace VulkanEngine
{
enum class MeshPrimitive : uint8_t
{
    Cube,
    Sphere,
    Plane,
    //
};

class MeshSubsystem final : public WorldSubsystem
{
    // for primitives
    std::unordered_map<MeshPrimitive, MeshDesc> m_primitiveDescCache;
    std::unordered_map<MeshPrimitive, MeshHandle> m_primitiveHandleCache;
    std::unordered_map<MeshPrimitive, std::function<MeshDesc()>> m_primitivesMap{{MeshPrimitive::Cube, MeshDesc::GetCube}};

    // for files
    std::unordered_map<std::string, MeshDesc> m_fileDescCache;
    std::unordered_map<std::string, MeshHandle> m_fileHandleCache;

    IRenderer* m_renderer;

public:
    MeshSubsystem(World* world, IRenderer* renderer);
    virtual ~MeshSubsystem();

    virtual void Init() override;
    virtual void Shutdown() override;

    MeshHandle GetOrCreatePrimitive(MeshPrimitive type);
    MeshHandle GetOrCreateMesh(const std::string& name);
};

}  // namespace VulkanEngine