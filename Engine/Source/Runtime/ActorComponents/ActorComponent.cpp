#include "ActorComponent.h"
#include "Runtime/Actor/Actor.h"
#include "Log/Log.h"

using namespace VulkanEngine;

DEFINE_LOG_CATEGORY_STATIC(LogActorComponent);

ActorComponent::ActorComponent(Actor* owner) : m_owner(owner)
{
    VE_LOG(LogActorComponent, Display, "ActorComponent was created");
}

ActorComponent::~ActorComponent()
{
    VE_LOG(LogActorComponent, Display, "ActorComponent was destroyed");
}

void ActorComponent::BeginPlay()
{
    //
}

void ActorComponent::Tick(float deltaTime)
{
    //
}

Actor* ActorComponent::GetOwner() const
{
    return m_owner;
}
