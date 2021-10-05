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
#include "Mathf.h"
#include <BGFXRenderer.h>
#include <Utils/CommandCache.h>
#include "Materials/PortalMaterial.h"
#include "Window/IWindow.h"
#include <Materials/DiffuseMaterial.h>

PortalManagerCore::PortalManagerCore()
	: Base(ComponentFilter().Requires<Transform>().Requires<Portal>())
{

}

void PortalManagerCore::OnEntityAdded(Entity& NewEntity)
{
	auto world = GetEngine().GetWorld().lock();
	Portal& portalComponent = NewEntity.GetComponent<Portal>();
	if (BluePortal && portalComponent.Type == Portal::PortalType::Blue)
	{
		//BluePortal.GetComponent<Transform>().RemoveChild(&BluePortalCamera->GetComponent<Transform>());
		RecusiveDelete(BluePortal, &BluePortal.GetComponent<Transform>());
	}
	if (OrangePortal && portalComponent.Type == Portal::PortalType::Orange)
	{
		//OrangePortal.GetComponent<Transform>().RemoveChild(&OrangePortalCamera->GetComponent<Transform>());
		RecusiveDelete(OrangePortal, &OrangePortal.GetComponent<Transform>());
	}

	Transform& portalObject = NewEntity.GetComponent<Transform>();
	switch (portalComponent.Type)
	{
	case Portal::PortalType::Blue:
	{
		BluePortal = NewEntity;
		Transform* ttt = portalObject.GetChildByName("Mesh");
		if (ttt && ttt->Parent->HasComponent<Mesh>())
		{
			Mesh& meshComp = ttt->Parent->GetComponent<Mesh>();

			Camera& cam = BluePortalCamera->GetComponent<Camera>();
			cam.Skybox = Camera::CurrentCamera->Skybox;
			cam.ClearType = Camera::CurrentCamera->ClearType;
			cam.OutputSize = Camera::CurrentCamera->OutputSize;

			Moonlight::CameraData* CamData = GetEngine().GetRenderer().GetCameraCache().Get(cam.GetCameraId());
			BluePortalTexture->UpdateBuffer(CamData->Buffer);

			meshComp.MeshMaterial->SetTexture(Moonlight::TextureType::Diffuse, BluePortalTexture);
			//meshComp.MeshMaterial->Tiling = Vector2(cam.OutputSize.x / CamData->Buffer->Width, cam.OutputSize.y / CamData->Buffer->Height);

			//BluePortalCamera->GetComponent<Transform>().SetParent(portalObject);
		}

		break;
	}
	case Portal::PortalType::Orange:
	default:
	{
		OrangePortal = NewEntity;
		Transform* ttt = portalObject.GetChildByName("Mesh");
		if (ttt && ttt->Parent->HasComponent<Mesh>())
		{
			Mesh& meshComp = ttt->Parent->GetComponent<Mesh>();

			Camera& cam = OrangePortalCamera->GetComponent<Camera>();
			cam.Skybox = Camera::CurrentCamera->Skybox;
			cam.ClearType = Camera::CurrentCamera->ClearType;
			cam.OutputSize = Camera::CurrentCamera->OutputSize;
			Moonlight::CameraData* CamData = GetEngine().GetRenderer().GetCameraCache().Get(cam.GetCameraId());
			OrangePortalTexture->UpdateBuffer(CamData->Buffer);

			meshComp.MeshMaterial->SetTexture(Moonlight::TextureType::Diffuse, OrangePortalTexture);
			//meshComp.MeshMaterial->Tiling = Vector2(cam.OutputSize.x / CamData->Buffer->Width, cam.OutputSize.y / CamData->Buffer->Height);

			//OrangePortalCamera->GetComponent<Transform>().SetParent(portalObject);
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
		HandleCamera(BluePortal, OrangePortal, OrangePortalCamera);
		HandleCamera(OrangePortal, BluePortal, BluePortalCamera);
	}
}

void PortalManagerCore::HandleCamera(Entity& primaryPortal, Entity& otherPortal, EntityHandle& portalCamera)
{
	auto world = GetEngine().GetWorld().lock();
	Transform& transform = portalCamera->GetComponent<Transform>();
	Portal& portal = primaryPortal.GetComponent<Portal>();

	auto& mainCam = Camera::CurrentCamera->Parent;
	//if (false)
	{
		Transform& primaryPortalTransform = primaryPortal.GetComponent<Transform>();
		Transform& otherPortalTransform = otherPortal.GetComponent<Transform>();
		//primaryPortalTransform.UpdateWorldTransform();
		//otherPortalTransform.UpdateWorldTransform();

		Matrix4 cameraMatrix = Matrix4(
			primaryPortalTransform.GetLocalToWorldMatrix().GetInternalMatrix()
			* 
			otherPortalTransform.GetWorldToLocalMatrix().GetInternalMatrix()
			* 
			mainCam->GetComponent<Transform>().GetLocalToWorldMatrix().GetInternalMatrix()
		);// ;

		//transform.SetWorldTransform(cameraMatrix);
		transform.SetPosition(cameraMatrix.GetPosition());
		transform.SetRotation(cameraMatrix.GetRotation());

		Transform* ttt = primaryPortalTransform.GetChildByName("Mesh");
		if (ttt && ttt->Parent->HasComponent<Mesh>())
		{
			Mesh& meshComp = ttt->Parent->GetComponent<Mesh>();
			Moonlight::CameraData* CamData = GetEngine().GetRenderer().GetCameraCache().Get(Camera::CurrentCamera->GetCameraId());

			static_cast<PortalMaterial*>(meshComp.MeshMaterial.get())->ScreenSize = GetEngine().GetWindow()->GetSize();// Vector2(OrangePortalTexture->mWidth, OrangePortalTexture->mHeight);// Vector2(mainCam.OutputSize.x / CamData->Buffer->Width, cam.OutputSize.y / CamData->Buffer->Height);
		}
	}
	//if(false)
	//{
	//	Vector3 offset = portal.Observer->GetWorldPosition() - otherPortal.GetComponent<Transform>().GetWorldPosition();
	//	offset = (primaryPortal.GetComponent<Transform>().GetWorldPosition() - offset);
	//	offset.SetY(portal.Observer->GetWorldPosition().Y());
	//	Vector3 offset2 = portal.Observer->GetWorldPosition() - primaryPortal.GetComponent<Transform>().GetWorldPosition();
	//	transform.SetWorldPosition(offset);


	//	float thing = 0;
	//	float difference = Quaternion::Angle(primaryPortal.GetComponent<Transform>().InternalRotation, otherPortal.GetComponent<Transform>().InternalRotation);// DirectX::Angle//DirectX::XMQuaternionToAxisAngle(Vector3::Up.GetInternalVec(), &thing, );

	//	Quaternion portalRotationDistance = Quaternion::AngleAxis(180.0f, Vector3::Up);
	//	Vector3 newCameraDirection =  portalRotationDistance* transform.Front();

	//	Vector3 forward = newCameraDirection;// (portal.Observer->GetWorldPosition() - otherPortal.GetComponent<Transform>().GetWorldPosition()).Normalized();// primaryPortal.GetComponent<Transform>().GetPo - t_camera).normalized();

	//	portalCamera->GetComponent<Camera>().Near = Mathf::Abs(offset2.Dot(primaryPortal.GetComponent<Transform>().Front()));

	//	transform.LookAt(forward);
	//}
	//if (false)
	//{
	//	Vector3 offset = portal.Observer->GetWorldPosition() - otherPortal.GetComponent<Transform>().GetWorldPosition();

	//	transform.SetWorldPosition(primaryPortal.GetComponent<Transform>().GetWorldPosition() + offset);

	//	float thing = 0;
	//	float difference = Quaternion::Angle(primaryPortal.GetComponent<Transform>().InternalRotation, otherPortal.GetComponent<Transform>().InternalRotation);// DirectX::Angle//DirectX::XMQuaternionToAxisAngle(Vector3::Up.GetInternalVec(), &thing, );

	//	Quaternion portalRotationDistance = Quaternion::AngleAxis(difference, Vector3::Up);
	//	Vector3 newCameraDirection = portalRotationDistance * transform.Front();

	//	Vector3 forward = newCameraDirection.Normalized();

	//	transform.LookAt(forward);
	//}
}

void PortalManagerCore::Init()
{

}

void PortalManagerCore::OnStart()
{
	auto world = GetEngine().GetWorld().lock();
	SharedPtr<Moonlight::Texture> defaultDiffuse = ResourceCache::GetInstance().Get<Moonlight::Texture>(Path("Assets/Textures/DefaultAlpha.png"));

	{
		BluePortalCamera = world->CreateEntity();
		Transform& portalCamera = BluePortalCamera->AddComponent<Transform>("Blue Portal Camera");
		portalCamera.SetPosition(Vector3(0, 5, -10));
		Camera& cam = BluePortalCamera->AddComponent<Camera>();
		cam.Skybox = Camera::CurrentCamera->Skybox;
		BluePortalTexture = std::make_shared<Moonlight::Texture>(nullptr);

		// Debug camera view
		{
			portalCamera.SetScale({ .1f, .1f, .3f });
			Vector3 diffuseColor = { 0.f, .82f, 1.f };
			DiffuseMaterial* mat = new DiffuseMaterial();
			Mesh& meshComp = BluePortalCamera->AddComponent<Mesh>(Moonlight::MeshType::Cube, mat);
			meshComp.MeshMaterial->DiffuseColor = diffuseColor;
			meshComp.MeshMaterial->SetTexture(Moonlight::TextureType::Diffuse, defaultDiffuse);
		}
	}
	{
		OrangePortalCamera = world->CreateEntity();
		Transform& portalCamera = OrangePortalCamera->AddComponent<Transform>("Orange Portal Camera");
		portalCamera.SetPosition(Vector3(0, 5, -10));
		Camera& cam = OrangePortalCamera->AddComponent<Camera>();
		cam.Skybox = Camera::CurrentCamera->Skybox;
		OrangePortalTexture = std::make_shared<Moonlight::Texture>(nullptr);

		// Debug camera view
		{
			portalCamera.SetScale({ .1f, .1f, .3f });
			Vector3 diffuseColor = { 1.f, .5f, 0.f };

			DiffuseMaterial* mat = new DiffuseMaterial();
			Mesh& meshComp = OrangePortalCamera->AddComponent<Mesh>(Moonlight::MeshType::Cube, mat);
			meshComp.MeshMaterial->DiffuseColor = diffuseColor;
			meshComp.MeshMaterial->SetTexture(Moonlight::TextureType::Diffuse, defaultDiffuse);
		}
	}
}

void PortalManagerCore::OnStop()
{

}

void PortalManagerCore::RecusiveDelete(Entity& ent, Transform* trans)
{
	if (!trans)
	{
		return;
	}
	for (auto child : trans->GetChildren())
	{
		RecusiveDelete(*child->Parent.Get(), child.get());
	}
	ent.MarkForDelete();
}
