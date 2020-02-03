#include "PortalManagerCore.h"
#include "Engine/Engine.h"
#include "Components/Portal.h"
#include "Components/Graphics/Model.h"
#include "Components/Physics/Rigidbody.h"
#include "Components/Camera.h"
#include "Components/Transform.h"
#include "Engine/World.h"
#include "Components/Graphics/Mesh.h"
#include "Graphics/Texture.h"
#include "Camera/CameraData.h"
#include "Renderer.h"

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
	{
		BluePortal = NewEntity;

		if (BluePortal.HasComponent<Mesh>())
		{
			Mesh& meshComp = BluePortal.GetComponent<Mesh>();

			Camera& cam = BluePortalCamera->GetComponent<Camera>();

			Moonlight::CameraData& CamData = GetEngine().GetRenderer().GetCamera(cam.GetCameraId());
			BluePortalTexture->UpdateBuffer(CamData.Buffer);

			meshComp.MeshMaterial->SetTexture(Moonlight::TextureType::Diffuse, BluePortalTexture);
		}

		break;
	}
	case Portal::PortalType::Orange:
	default:
	{
		OrangePortal = NewEntity;
		if (OrangePortal.HasComponent<Mesh>())
		{
			Mesh& meshComp = OrangePortal.GetComponent<Mesh>();

			Camera& cam = OrangePortalCamera->GetComponent<Camera>();

			Moonlight::CameraData& CamData = GetEngine().GetRenderer().GetCamera(cam.GetCameraId());
			OrangePortalTexture->UpdateBuffer(CamData.Buffer);

			meshComp.MeshMaterial->SetTexture(Moonlight::TextureType::Diffuse, OrangePortalTexture);
		}

		break;
	}
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
	if(BluePortal && OrangePortal)
	{
		HandleCamera(BluePortal, OrangePortal, BluePortalCamera);
		HandleCamera(OrangePortal, BluePortal, OrangePortalCamera);
	}
}

void PortalManagerCore::HandleCamera(Entity& primaryPortal, Entity& otherPortal, SharedPtr<Entity>& portalCamera)
{
	auto world = GetEngine().GetWorld().lock();
	Transform& transform = portalCamera->GetComponent<Transform>();
	Portal& portal = primaryPortal.GetComponent<Portal>();

	{
		Vector3 offset =  otherPortal.GetComponent<Transform>().GetWorldPosition() - portal.Observer->GetWorldPosition();
		transform.SetWorldPosition(primaryPortal.GetComponent<Transform>().GetWorldPosition() + offset);


		float thing = 0;
		float difference = Quaternion::Angle(primaryPortal.GetComponent<Transform>().InternalRotation, otherPortal.GetComponent<Transform>().InternalRotation);// DirectX::Angle//DirectX::XMQuaternionToAxisAngle(Vector3::Up.GetInternalVec(), &thing, );

		Quaternion portalRotationDistance = Quaternion::AngleAxis(difference, Vector3::Up);
		Vector3 newCameraDirection =  portalRotationDistance* Camera::CurrentCamera->Front;

		Vector3 forward = (portal.Observer->GetWorldPosition() - otherPortal.GetComponent<Transform>().GetWorldPosition()).Normalized();// primaryPortal.GetComponent<Transform>().GetPo - t_camera).normalized();

		//left_camera = up.cross(forward_camera).normalized();

		//up_camera = forward_camera.cross(left_camera).normalized();

		//translation_camera = t_look;

		portalCamera->GetComponent<Camera>().Front = forward;
		transform.LookAt(forward);
	}

	if (false)
	{
		Vector3 offset = portal.Observer->GetWorldPosition() - otherPortal.GetComponent<Transform>().GetWorldPosition();

		transform.SetWorldPosition(primaryPortal.GetComponent<Transform>().GetWorldPosition() + offset);

		float thing = 0;
		float difference = Quaternion::Angle(primaryPortal.GetComponent<Transform>().InternalRotation, otherPortal.GetComponent<Transform>().InternalRotation);// DirectX::Angle//DirectX::XMQuaternionToAxisAngle(Vector3::Up.GetInternalVec(), &thing, );

		Quaternion portalRotationDistance = Quaternion::AngleAxis(difference, Vector3::Up);
		Vector3 newCameraDirection = portalRotationDistance * Camera::CurrentCamera->Front;

		Vector3 forward = newCameraDirection.Normalized();// primaryPortal.GetComponent<Transform>().GetPo - t_camera).normalized();

		//left_camera = up.cross(forward_camera).normalized();

		//up_camera = forward_camera.cross(left_camera).normalized();

		//translation_camera = t_look;

		portalCamera->GetComponent<Camera>().Front = forward;
		transform.LookAt(forward);
	}
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
		BluePortalTexture = std::make_shared<Moonlight::Texture>(nullptr);
	}
	{
		OrangePortalCamera = world->CreateEntity().lock();
		Transform& portalCamera = OrangePortalCamera->AddComponent<Transform>("Orange Portal Camera");
		portalCamera.SetPosition(Vector3(0, 5, -10));
		OrangePortalCamera->AddComponent<Camera>();
		OrangePortalTexture = std::make_shared<Moonlight::Texture>(nullptr);
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
