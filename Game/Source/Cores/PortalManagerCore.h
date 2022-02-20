#pragma once
#include "ECS/ComponentFilter.h"
#include "ECS/Core.h"
#include "Pointers.h"
#include "Components/Transform.h"
#include "ECS/EntityHandle.h"
#include "Events/EventReceiver.h"

namespace Moonlight { class Texture; }

class PortalManagerCore
	: public Core<PortalManagerCore>
	, public EventReceiver
{
public:
	PortalManagerCore();

	void OnEntityAdded(Entity& NewEntity) override;
	void OnEntityRemoved(Entity& InEntity) override;

#if ME_EDITOR
	void OnEditorInspect() override;
#endif

	void Update(const UpdateContext& inUpdateContext) override;



	bool OnEvent(const BaseEvent& evt) override;

private:
	void Init() override;
	void OnStart() override;
	void OnStop() override;

	void RecusiveDelete(Entity& ent, Transform* trans);

	void HandleTravelling(Entity& primaryPortal, Entity& otherPortal);
	void HandleCamera(Entity& primaryPortal, Entity& otherPortal, EntityHandle& portalCamera);

	float ProtectScreenFromClipping(Transform& portal, const Vector3& viewPoint);

	EntityHandle BluePortalCamera;
	EntityHandle OrangePortalCamera;

	Entity BluePortal;
	Entity OrangePortal;

	SharedPtr<Moonlight::Texture> BluePortalTexture;
	SharedPtr<Moonlight::Texture> OrangePortalTexture;
	EntityHandle TestEnt;

	float ObliquePlaneOffset = -0.09f;
};

ME_REGISTER_CORE(PortalManagerCore)