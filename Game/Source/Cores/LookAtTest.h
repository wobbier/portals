#pragma once

#include "ECS/Core.h"
#include "Components/Transform.h"
#include "ECS/Entity.h"
#include "Engine/World.h"
#include "ECS/Component.h"
#include "Components/Graphics/Model.h"
#include "Engine/Input.h"
#include "Math/Vector3.h"
#include "Components/Graphics/Mesh.h"
#include "Engine/Engine.h"
#include "Cores/SceneCore.h"
#include "Components/Camera.h"
#include "Mathf.h"
#include "Components/Physics/CharacterController.h"

class LookAtObject final
	: public Component<LookAtObject>
{
public:
	LookAtObject()
		: Component("LookAtObject")
	{
	}

#if ME_EDITOR
	virtual void OnEditorInspect() final
	{
	}
#endif

	virtual void Init() final
	{
	}

private:
	void OnSerialize(json& outJson) final
	{
	}

	void OnDeserialize(const json& inJson) final
	{
	}

};
ME_REGISTER_COMPONENT(LookAtObject)

class LookAtTestCore final
	: public Core<LookAtTestCore>
{
	Transform* target = nullptr;
public:
	LookAtTestCore()
		: Base(ComponentFilter().Requires<Transform>().RequiresOneOf<LookAtObject>().RequiresOneOf<CharacterController>())
	{

	}

	virtual void OnEntityAdded(Entity& NewEntity) final
	{
		if (NewEntity.HasComponent<CharacterController>())
		{
			target = &NewEntity.GetComponent<Transform>();
		}
	}
	virtual void OnEntityRemoved(Entity& InEntity) final
	{

	}
#if ME_EDITOR
	virtual void OnEditorInspect() final
	{

	}
#endif

	virtual void Update(float dt) final
	{
		auto Entities = GetEntities();
		for (auto& ent : Entities)
		{
			Transform& trans = ent.GetComponent<Transform>();

			if (ent.HasComponent<LookAtObject>())
			{
				if (target)
				{
					trans.LookAt(target->GetWorldPosition() - trans.GetWorldPosition());
				}
			}
		}
	}

private:
	virtual void Init() final
	{

	}
	virtual void OnStart() override
	{

	}
	virtual void OnStop() override
	{

	}
};
ME_REGISTER_CORE(LookAtTestCore)