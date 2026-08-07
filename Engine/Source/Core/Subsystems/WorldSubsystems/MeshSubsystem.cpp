#include "MeshSubsystem.h"
#include "Log/Log.h"

using namespace VulkanEngine;

DEFINE_LOG_CATEGORY_STATIC(LogMeshSubsystem);

MeshSubsystem::MeshSubsystem(World* world, IRenderer* renderer) : WorldSubsystem(world), m_renderer(renderer)
{
    //
}

MeshSubsystem::~MeshSubsystem()
{
    //
}

void MeshSubsystem::Init()
{
    //
}

void MeshSubsystem::Shutdown()
{
    for (auto& [type, handle] : m_primitiveHandleCache)
    {
        m_renderer->DestroyMesh(handle);
    }
    m_primitiveHandleCache.clear();

    for (auto& [name, handle] : m_fileHandleCache)
    {
        m_renderer->DestroyMesh(handle);
    }
    m_fileHandleCache.clear();
}

MeshHandle MeshSubsystem::GetOrCreatePrimitive(MeshPrimitive type)
{
    if (!m_renderer)
    {
        VE_LOG(LogMeshSubsystem, Error, "Cannot create mesh: renderer is not set in MeshSubsystem");
        return MeshHandle();
    }

    auto cachedIt = m_primitiveHandleCache.find(type);
    if (cachedIt != m_primitiveHandleCache.end())
    {
        return cachedIt->second;  // allready has that primitive on the GPU
    }

    auto handleIt = m_primitivesMap.find(type);
    if (handleIt == m_primitivesMap.end())
    {
        VE_LOG(LogMeshSubsystem, Error, "No generator registered for primitive type");
        return MeshHandle();
    }

    MeshDesc desc = handleIt->second();
    m_primitiveDescCache[type] = desc;  // for future physics

    MeshHandle handle = m_renderer->CreateMesh(desc);
    m_primitiveHandleCache[type] = handle;

    return handle;
}

MeshHandle MeshSubsystem::GetOrCreateMesh(const std::string& name)
{
    if (!m_renderer)
    {
        VE_LOG(LogMeshSubsystem, Error, "Cannot create mesh: renderer is not set in MeshSubsystem");
        return MeshHandle();
    }

    auto handleIt = m_fileHandleCache.find(name);
    if (handleIt != m_fileHandleCache.end())
    {
        return handleIt->second;
    }

    // TODO: loader
    VE_LOG(LogMeshSubsystem, Warning, "File-based mesh loading is not implemented yet: {}", name);
    return MeshHandle();
}
