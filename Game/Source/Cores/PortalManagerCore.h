#pragma once
#include "ECS/ComponentFilter.h"
#include "ECS/Core.h"
#include "Pointers.h"
#include "Components/Transform.h"

class PortalManagerCore
	: public Core<PortalManagerCore>
{
public:
	PortalManagerCore();

	void OnEntityAdded(Entity& NewEntity) override;
	void OnEntityRemoved(Entity& InEntity) override;

#if ME_EDITOR
	void OnEditorInspect() override;
#endif

	void Update(float dt) override;

private:
	void Init() override;
	void OnStart() override;
	void OnStop() override;

	void RecusiveDelete(Entity ent, Transform* trans);;


	SharedPtr<Entity> BluePortalCamera;
	SharedPtr<Entity> OrangePortalCamera;

	Entity BluePortal;
	Entity OrangePortal;
};

ME_REGISTER_CORE(PortalManagerCore)