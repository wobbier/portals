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
#include "Components/PortalTraveller.h"
#include <SDL_stdinc.h>
#include "Components/Physics/CharacterController.h"
#include "Events/PlatformEvents.h"

PortalManagerCore::PortalManagerCore()
    : Base(ComponentFilter().Requires<Transform>().Requires<Portal>())
{
    std::vector<TypeId> events;
    events.push_back(WindowResizedEvent::GetEventId());
    EventManager::GetInstance().RegisterReceiver(this, events);
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
                CamData->Buffer->MatchMainBufferSize = true;
                BluePortalTexture->UpdateBuffer(CamData->Buffer);

                meshComp.MeshMaterial->SetTexture(Moonlight::TextureType::Diffuse, BluePortalTexture);
                meshComp.MeshMaterial->Tiling = Vector2(cam.OutputSize.x / CamData->Buffer->Width, cam.OutputSize.y / CamData->Buffer->Height);

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
                CamData->Buffer->MatchMainBufferSize = true;
                OrangePortalTexture->UpdateBuffer(CamData->Buffer);

                meshComp.MeshMaterial->SetTexture(Moonlight::TextureType::Diffuse, OrangePortalTexture);
                meshComp.MeshMaterial->Tiling = Vector2(cam.OutputSize.x / CamData->Buffer->Width, cam.OutputSize.y / CamData->Buffer->Height);

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

    ImGui::DragFloat("Oblique Plane Offset", &ObliquePlaneOffset);
}

#endif

void PortalManagerCore::Update(const UpdateContext& inUpdateContext)
{
    if (BluePortal && OrangePortal)
    {
        Portal& bluePortalComp = BluePortal.GetComponent<Portal>();
        Portal& orangePortalComp = OrangePortal.GetComponent<Portal>();
        if (bluePortalComp.Travellers.empty() && GetEngine().GetInput().WasKeyPressed(KeyCode::R))
        {
            auto& mainCam = Camera::CurrentCamera->Parent;
            auto& playerObject = *mainCam->GetComponent<Transform>().GetParentTransform()->Parent.Get();
            playerObject.GetComponent<PortalTraveller>().PreviousOffsetFromPortal = playerObject.GetComponent<Transform>().GetWorldPosition() - BluePortal.GetComponent<Transform>().GetWorldPosition();
            bluePortalComp.Travellers.push_back(playerObject);
        }
        HandleTravelling(BluePortal, OrangePortal);
        HandleCamera(BluePortal, OrangePortal, OrangePortalCamera);
        HandleCamera(OrangePortal, BluePortal, BluePortalCamera);
        //ProtectScreenFromClipping(BluePortal.GetComponent<Transform>(),);
        //ProtectScreenFromClipping(OrangePortal.GetComponent<Transform>());
    }
}

bool PortalManagerCore::OnEvent(const BaseEvent& evt)
{
    if (evt.GetEventId() == WindowResizedEvent::GetEventId())
    {
        const WindowResizedEvent& event = static_cast<const WindowResizedEvent&>(evt);
        if (OrangePortalCamera)
        {
            Camera& cam = OrangePortalCamera->GetComponent<Camera>();
            Moonlight::CameraData* CamData = GetEngine().GetRenderer().GetCameraCache().Get(cam.GetCameraId());
            OrangePortalTexture->UpdateBuffer(CamData->Buffer);
        }
        if (BluePortalCamera)
        {
            Camera& cam = BluePortalCamera->GetComponent<Camera>();
            Moonlight::CameraData* CamData = GetEngine().GetRenderer().GetCameraCache().Get(cam.GetCameraId());
            BluePortalTexture->UpdateBuffer(CamData->Buffer);
        }
    }
    return false;
}

void PortalManagerCore::HandleTravelling(Entity& primaryPortal, Entity& otherPortal)
{
    for (Entity& it : primaryPortal.GetComponent<Portal>().Travellers)
    {
        if (it.HasComponent<PortalTraveller>())
        {
            Transform& transform = it.GetComponent<Transform>();
            PortalTraveller& traveller = it.GetComponent<PortalTraveller>();
            Transform& primaryPortalTransform = primaryPortal.GetComponent<Transform>();
            Transform& otherPortalTransform = otherPortal.GetComponent<Transform>();

            Matrix4 m = otherPortalTransform.GetLocalToWorldMatrix().GetInternalMatrix() * primaryPortalTransform.GetWorldToLocalMatrix().GetInternalMatrix() * transform.GetLocalToWorldMatrix().GetInternalMatrix();

            Vector3 offsetFromPortal = transform.GetWorldPosition() - primaryPortalTransform.GetWorldPosition();
            int portalSide = Mathf::Sign(offsetFromPortal.Dot(primaryPortalTransform.Front()));
            int portalSideOld = Mathf::Sign(traveller.PreviousOffsetFromPortal.Dot(transform.Front()));

            if (portalSide != portalSideOld)
            {
                transform.SetWorldPosition(m.GetPosition());
                transform.SetWorldRotation(m.GetRotation());
                it.GetComponent<CharacterController>().Teleport(m.GetPosition(), m.GetRotation());
                primaryPortal.GetComponent<Portal>().Travellers.clear();
                //otherPortal.GetComponent<Portal>().Travellers.push_back(it);
                return;
            }
            else
            {
                traveller.PreviousOffsetFromPortal = offsetFromPortal;
            }
        }
    }
}

void PortalManagerCore::HandleCamera(Entity& primaryPortal, Entity& otherPortal, EntityHandle& portalCamera)
{
    auto world = GetEngine().GetWorld().lock();
    Transform& transform = portalCamera->GetComponent<Transform>();
    Portal& portal = primaryPortal.GetComponent<Portal>();

    auto& mainCam = Camera::CurrentCamera->Parent;
    Transform& primaryPortalTransform = primaryPortal.GetComponent<Transform>();
    Transform& otherPortalTransform = otherPortal.GetComponent<Transform>();

    Matrix4 cameraMatrix = Matrix4(
        primaryPortalTransform.GetLocalToWorldMatrix().GetInternalMatrix()
        *
        otherPortalTransform.GetWorldToLocalMatrix().GetInternalMatrix()
        *
        mainCam->GetComponent<Transform>().GetLocalToWorldMatrix().GetInternalMatrix()
    );

    //transform.SetWorldTransform(cameraMatrix);
    transform.SetPosition(cameraMatrix.GetPosition());
    transform.SetRotation(cameraMatrix.GetRotation());

    portalCamera->GetComponent<Camera>().OutputSize = Camera::CurrentCamera->OutputSize;
    // Oblique Matrix
    {
        int dot = Mathf::Sign(primaryPortalTransform.Front().Dot(primaryPortalTransform.GetWorldPosition() - transform.GetWorldPosition()));

        // SOOOOOOOOOOOO The CameraCore will eventually set the Camera::CameraToWorld matrix, but we need it NOW...
        Vector3 eye = transform.GetWorldPosition();
        Vector3 at = transform.GetWorldPosition() + transform.Front();
        Vector3 up = transform.Up();

        Matrix4 camShit = Matrix4(glm::lookAtLH(eye.InternalVector, at.InternalVector, up.InternalVector));

        Vector3 camSpacePos = camShit.TransformPoint(primaryPortalTransform.GetWorldPosition());
        Vector3 camSpaceNormal = camShit.TransformVector(primaryPortalTransform.Front()) * dot;
        float camSpaceDst = -camSpacePos.Dot(camSpaceNormal) + ObliquePlaneOffset;

        if (Mathf::Abs(camSpaceDst) > 0.2f)
        {
            glm::vec4 clipPlaneCameraSpace = glm::vec4(camSpaceNormal.x, camSpaceNormal.y, camSpaceNormal.z, camSpaceDst);
            portalCamera->GetComponent<Camera>().SetObliqueMatrixData(clipPlaneCameraSpace);
        }
        else
        {
            portalCamera->GetComponent<Camera>().ClearObliqueMatrixData();
        }
        TestEnt->GetComponent<Transform>().SetPosition(camSpacePos);
    }

    Transform* ttt = primaryPortalTransform.GetChildByName("Mesh");
    if (ttt && ttt->Parent->HasComponent<Mesh>())
    {
        Mesh& meshComp = ttt->Parent->GetComponent<Mesh>();
        Moonlight::CameraData* CamData = GetEngine().GetRenderer().GetCameraCache().Get(Camera::CurrentCamera->GetCameraId());
        meshComp.MeshMaterial->Tiling = Vector2(Camera::CurrentCamera->OutputSize.x / CamData->Buffer->Width, Camera::CurrentCamera->OutputSize.y / CamData->Buffer->Height);

#if ME_EDITOR
        static_cast<PortalMaterial*>(meshComp.MeshMaterial.get())->ScreenSize = CamData->OutputSize;
#else
    // Should be OutputSize?
        static_cast<PortalMaterial*>(meshComp.MeshMaterial.get())->ScreenSize = GetEngine().GetWindow()->GetSize();
#endif
    }
    ProtectScreenFromClipping(primaryPortalTransform, mainCam->GetComponent<Transform>().GetWorldPosition());
}

float PortalManagerCore::ProtectScreenFromClipping(Transform& portal, const Vector3& viewPoint)
{
    auto& mainCam = Camera::CurrentCamera->Parent;
    const float halfHeight = Camera::CurrentCamera->Near * std::tan(Camera::CurrentCamera->GetFOV() * .5f * (M_PI / 180.f));
    const float halfWidth = halfHeight * Camera::CurrentCamera->GetAspectRatio();
    const float dstToNearClipPlaneCorner = Vector3(halfWidth, halfHeight, Camera::CurrentCamera->Near).Length();
    const float screenThickness = dstToNearClipPlaneCorner;

    Transform* portalMesh = portal.GetChildByName("Mesh");
    bool camFacingSameDirAsPortal = portal.Front().Dot(portal.GetWorldPosition() - viewPoint) > 0.f;
    portalMesh->SetScale({ portalMesh->GetScale().x, portalMesh->GetScale().y, screenThickness });
    portalMesh->SetPosition(Vector3::Front * screenThickness * ((camFacingSameDirAsPortal) ? .5f : -.5f));

    return screenThickness;
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
        cam.OutputSize = Camera::CurrentCamera->OutputSize;
        BluePortalTexture = std::make_shared<Moonlight::Texture>(nullptr);

        // Debug camera view
// 		{
// 			portalCamera.SetScale({ .1f, .1f, .3f });
// 			Vector3 diffuseColor = { 0.f, .82f, 1.f };
// 			DiffuseMaterial* mat = new DiffuseMaterial();
// 			Mesh& meshComp = BluePortalCamera->AddComponent<Mesh>(Moonlight::MeshType::Cube, mat);
// 			meshComp.MeshMaterial->DiffuseColor = diffuseColor;
// 			meshComp.MeshMaterial->SetTexture(Moonlight::TextureType::Diffuse, defaultDiffuse);
// 		}
    }
    {
        OrangePortalCamera = world->CreateEntity();
        Transform& portalCamera = OrangePortalCamera->AddComponent<Transform>("Orange Portal Camera");
        portalCamera.SetPosition(Vector3(0, 5, -10));
        Camera& cam = OrangePortalCamera->AddComponent<Camera>();
        cam.Skybox = Camera::CurrentCamera->Skybox;
        cam.OutputSize = Camera::CurrentCamera->OutputSize;
        OrangePortalTexture = std::make_shared<Moonlight::Texture>(nullptr);

        // Debug camera view
// 		{
// 			portalCamera.SetScale({ .1f, .1f, .3f });
// 			Vector3 diffuseColor = { 1.f, .5f, 0.f };
// 
// 			DiffuseMaterial* mat = new DiffuseMaterial();
// 			Mesh& meshComp = OrangePortalCamera->AddComponent<Mesh>(Moonlight::MeshType::Cube, mat);
// 			meshComp.MeshMaterial->DiffuseColor = diffuseColor;
// 			meshComp.MeshMaterial->SetTexture(Moonlight::TextureType::Diffuse, defaultDiffuse);
// 		}
    }
    {
        TestEnt = world->CreateEntity();
        TestEnt->AddComponent<Transform>("TestCube");

        DiffuseMaterial* mat = new DiffuseMaterial();
        Mesh& meshComp = TestEnt->AddComponent<Mesh>(Moonlight::MeshType::Cube, mat);
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
