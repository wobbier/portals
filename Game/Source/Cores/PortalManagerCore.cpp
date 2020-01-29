#include "PortalManagerCore.h"
#include "Engine/Engine.h"
#include "Components/Portal.h"
#include "Components/Graphics/Model.h"
#include "Components/Physics/Rigidbody.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Engine/World.h"

PortalManagerCore::PortalManagerCore()
	: Base(ComponentFilter().Requires<Transform>().Requires<Portal>())
{

}

void PortalManagerCore::OnEntityAdded(Entity& NewEntity)
{
	auto world = GetEngine().GetWorld().lock();
	Portal& portalComponent = NewEntity.GetComponent<Portal>();
	if (world->GetEntity(BluePortal.GetId()).lock() && portalComponent.Type == Portal::PortalType::Blue)
	{
		RecusiveDelete(BluePortal, &BluePortal.GetComponent<Transform>());
	}
	if (world->GetEntity(OrangePortal.GetId()).lock() && portalComponent.Type == Portal::PortalType::Orange)
	{
		RecusiveDelete(OrangePortal, &OrangePortal.GetComponent<Transform>());
	}

	Transform& portalObject = NewEntity.GetComponent<Transform>();
	switch (portalComponent.Type)
	{
	case Portal::PortalType::Blue:
		BluePortal = NewEntity;
		break;
	case Portal::PortalType::Orange:
	default:
		OrangePortal = NewEntity;
		break;

	}
}

void PortalManagerCore::OnEntityRemoved(Entity& InEntity)
{

}

#if ME_EDITOR

void PortalManagerCore::OnEditorInspect()
{
	Base::OnEditorInspect();
}

#endif

void PortalManagerCore::Update(float dt)
{

}

void PortalManagerCore::Init()
{

}

void PortalManagerCore::OnStart()
{
	auto world = GetEngine().GetWorld().lock();
	
	{
		BluePortalCamera = world->CreateEntity().lock();
		Transform& portalCamera = BluePortalCamera->AddComponent<Transform>("Blue Portal Camera");
		portalCamera.SetPosition(Vector3(0, 5, -10));
		BluePortalCamera->AddComponent<Camera>();
	}
	{
		OrangePortalCamera = world->CreateEntity().lock();
		Transform& portalCamera = OrangePortalCamera->AddComponent<Transform>("Orange Portal Camera");
		portalCamera.SetPosition(Vector3(0, 5, -10));
		OrangePortalCamera->AddComponent<Camera>();
	}
}

void PortalManagerCore::OnStop()
{

}

void PortalManagerCore::RecusiveDelete(Entity ent, Transform* trans)
{
	if (!trans)
	{
		return;
	}
	for (auto child : trans->GetChildren())
	{
		RecusiveDelete(*GetEngine().GetWorld().lock()->GetEntity(child->Parent).lock(), child);
	}
	ent.MarkForDelete();
}
