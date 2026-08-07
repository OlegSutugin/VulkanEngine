#pragma once

namespace VulkanEngine
{
class Actor;

class ActorComponent
{
protected:
    Actor* m_owner = nullptr;

public:
    ActorComponent(Actor* owner);
    virtual ~ActorComponent();

    virtual void BeginPlay();
    virtual void Tick(float deltaTime);

    Actor* GetOwner() const;
};

}  // namespace VulkanEngine