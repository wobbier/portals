#include "CharacterCore.h"
#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Engine/Input.h"
#include "Events/EventManager.h"
#include "optick.h"
#include "Components/Character.h"
#include "Components/Audio/AudioSource.h"
#include "Cores/AudioCore.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Mathf.h"
#include "imgui.h"
#include "Components/Physics/CharacterController.h"
#include "CLog.h"
#include "Cores/PhysicsCore.h"
#include "Cores/SceneGraph.h"
#include "Components/Graphics/Model.h"
#include "Components/Physics/Rigidbody.h"
#include "Components/Graphics/Mesh.h"
#include "Components/Portal.h"
#include "ECS/EntityHandle.h"
#include "Materials/PortalMaterial.h"
#include <Materials/DiffuseMaterial.h>

CharacterCore::CharacterCore()
	: Base(ComponentFilter().Requires<Transform>().Requires<Character>().Requires<CharacterController>())
{
	m_bluePortalShot = new AudioSource("Assets/Sounds/portalgun_shoot_blue1.wav");
	m_orangePortalShot = new AudioSource("Assets/Sounds/portalgun_shoot_red1.wav");

	m_invalidPortalSounds.reserve(4);
	for (int i = 0; i < 4; ++i)
	{
		m_invalidPortalSounds.push_back(new AudioSource("Assets/Sounds/Gun/portal_invalid_surface_0" + std::to_string(i + 1) + ".wav"));
	}

	AudioCore* audioCore = static_cast<AudioCore*>(GetEngine().GetWorld().lock()->GetCore(AudioCore::GetTypeId()));
	audioCore->InitComponent(*m_bluePortalShot);
	audioCore->InitComponent(*m_orangePortalShot);

	for (int i = 0; i < m_invalidPortalSounds.size(); ++i)
	{
		audioCore->InitComponent(*m_invalidPortalSounds[i]);
	}
}

void CharacterCore::Init()
{
}

void CharacterCore::OnEntityAdded(Entity& NewEntity)
{
	if (m_camera)
	{
		BRUH("It looks like we already have one character.");
		return;
	}

	m_playerTransform = &NewEntity.GetComponent<Transform>();
	m_controller = &NewEntity.GetComponent<CharacterController>();
	Transform* camera = m_playerTransform->GetChildByName("Camera");
	EntityHandle& cameraEnt = camera->Parent;
	m_camera = &cameraEnt->GetComponent<Camera>();
	m_cameraTransform = &cameraEnt->GetComponent<Transform>();
}

void CharacterCore::OnEntityRemoved(Entity& InEntity)
{
}

void CharacterCore::Update(float dt)
{
	HandlePortalShots();
	HandleMouseLook(dt);
	return;
}

#if ME_EDITOR
void CharacterCore::OnEditorInspect()
{
	Base::OnEditorInspect();
	ImGui::DragFloat("Movement Speed", &m_movementSpeed);
	ImGui::DragFloat("Look Sensitivity", &LookSensitivity);
}
#endif

void CharacterCore::HandlePortalShots()
{
	Input& input = GetEngine().GetInput();
	if (/*input.WasMouseButtonPressed(MouseButton::Left) || */input.WasKeyPressed(KeyCode::E))
	{
		if (FirePortal(true))
		{
			m_bluePortalShot->Play(false);
		}
		else
		{
			m_invalidPortalSounds[random(0, m_invalidPortalSounds.size() - 1)]->Play();
		}
	}
	if (input.WasMouseButtonPressed(MouseButton::Right) || input.WasKeyPressed(KeyCode::Q))
	{
		if (FirePortal(false))
		{
			m_orangePortalShot->Play(false);
		}
		else
		{
			m_invalidPortalSounds[random(0, m_invalidPortalSounds.size() - 1)]->Play();
		}
	}
}

void CharacterCore::HandleMouseLook(float dt)
{
	Vector2 newPos = m_camera->OutputSize / 2.f;
	Input& input = GetEngine().GetInput();

	Vector2 currentState = input.GetRelativeMousePosition();
	float XOffset = ((currentState.x) * LookSensitivity) * dt;
	float YOffest = ((currentState.y) * LookSensitivity) * dt;

	float Yaw = m_camera->Yaw -= XOffset;
	float Pitch = m_camera->Pitch + YOffest;

	if (Pitch > 89.0f)
		Pitch = 89.0f;
	if (Pitch < -89.0f)
		Pitch = -89.0f;
	m_camera->Pitch = Pitch;

	m_cameraTransform->SetRotation(Vector3(Pitch, 0.0f, 0.0f));
	m_playerTransform->SetRotation(Vector3(0.0f, -Yaw, 0.0f));
	Vector3 Front = m_playerTransform->Front();

	if (input.IsKeyDown(KeyCode::W))
	{
		m_controller->Walk(Front.Normalized() * m_movementSpeed * dt);
	}
	if (input.IsKeyDown(KeyCode::S))
	{
		m_controller->Walk(-Front.Normalized() * m_movementSpeed * dt);
	}

	if (input.IsKeyDown(KeyCode::D))
	{
		m_controller->Walk(m_playerTransform->Right().Normalized() * m_movementSpeed * dt);
	}
	if (input.IsKeyDown(KeyCode::A))
	{
		m_controller->Walk(-m_playerTransform->Right().Normalized() * m_movementSpeed * dt);
	}
	if (input.IsKeyDown(KeyCode::Space))
	{
		m_controller->Jump();
	}
}

bool CharacterCore::FirePortal(bool IsBluePortal)
{
	PhysicsCore* physics = static_cast<PhysicsCore*>(GameWorld->GetCore(PhysicsCore::GetTypeId()));

	std::vector<Vector3> directions = {
		{1.f, 0.f, 0.f}, // right
		{-1.f, 0.f, 0.f}, // left
		{0.f, 1.f, 0.f}, // up
		{0.f, -1.f, 0.f}, // down
	};

	std::vector<Vector3> directionsHit;
	directionsHit.reserve(4);
	bool canFitPortal = true;
	RaycastHit ray;
	if (physics->Raycast(m_cameraTransform->GetWorldPosition(), m_cameraTransform->GetWorldPosition() + m_cameraTransform->Front() * 20.0f, ray))
	{
		Transform& trans = ray.What->Parent->GetComponent<Transform>();
		BRUH("HIT" + trans.GetName());

		for (int i = 0; i < directions.size(); ++i)
		{
			Vector3 cross = directions[i].Cross(ray.Normal);
			Vector3 position = ray.Position + ((!cross.IsZero()) ? cross.Normalized() : Vector3());
			RaycastHit boundsRay;
			canFitPortal = canFitPortal && (physics->Raycast(position + ray.Normal, position - ray.Normal * 20.0f, boundsRay));
			directionsHit.push_back(boundsRay.Position);
		}
	}
	else
	{
		canFitPortal = false;
	}

	if (canFitPortal)
	{
		//for (int i = 0; i < directionsHit.size(); ++i)
		//{
		//	auto hitEnt = world->CreateEntity();
		//	Transform& hitEntTransform = hitEnt->AddComponent<Transform>("PortalBounds");
		//	hitEntTransform.SetWorldPosition(directionsHit[i]);
		//	hitEntTransform.SetScale(0.1f);
		//	hitEnt->AddComponent<Model>("Assets/Cube.fbx");
		//}

		//{
		//	auto hitEnt = world->CreateEntity().lock();
		//	Transform& hitEntTransform = hitEnt->AddComponent<Transform>("PortalNormal");
		//	hitEntTransform.SetWorldPosition(ray.Position + ray.Normal);
		//	hitEntTransform.SetScale(0.1f);
		//	hitEnt->AddComponent<Model>("Assets/Cube.fbx");
		//}

		SpawnPortal(ray.Position, ray.Normal, IsBluePortal);

	}

	return canFitPortal;
}

void CharacterCore::SpawnPortal(Vector3 position, Vector3 normal, bool IsBluePortal)
{
	auto hitEnt = GameWorld->CreateEntity();
	Transform& hitEntTransform = hitEnt->AddComponent<Transform>("Portal");
	hitEntTransform.SetWorldPosition(position + Vector3( 0.f, 4.f, 0.f ));

	if (normal.y >= 0.98f)
	{
		hitEntTransform.SetRotation(m_playerTransform->GetRotationEuler());
	}
	else
	{
		//Quaternion q;
		//q.SetLookRotation(normal);
		//hitEntTransform.SetRotation(q);
		hitEntTransform.LookAt(normal.Normalized());
		//hitEntTransform.Rotate(Vector3(90.f, 0.f, 0.f), TransformSpace::Self);
	}
	Portal& portal = hitEnt->AddComponent<Portal>(IsBluePortal ? Portal::PortalType::Blue : Portal::PortalType::Orange);
	portal.Observer = m_cameraTransform;

	//Rigidbody& rigidbody = hitEnt->AddComponent<Rigidbody>();
	//rigidbody.SetMass(0.f);

	SharedPtr<Moonlight::Texture> defaultDiffuse = ResourceCache::GetInstance().Get<Moonlight::Texture>(Path("Assets/Textures/DefaultAlpha.png"));

	Vector3 diffuseColor;
	if (IsBluePortal)
	{
		diffuseColor = { 0.f, .82f, 1.f };
	}
	else
	{
		diffuseColor = { 1.f, .5f, 0.f };
	}

	const float portalWidth = 2.1f;
	const float portalHeight = 4.2f;
	const float portalDepth = 0.1f;

	// Portal Border
	{
		auto borderEnt = GameWorld->CreateEntity();
		Transform& hitEntMeshTrans2 = borderEnt->AddComponent<Transform>("BorderTop");
		hitEntMeshTrans2.SetScale(Vector3(portalWidth, .1f, portalDepth));
		hitEntMeshTrans2.SetPosition({0.f, 4.1f, 0.f});
		hitEntMeshTrans2.SetParent(hitEntTransform);

		DiffuseMaterial* mat = new DiffuseMaterial();
		Mesh& meshComp = borderEnt->AddComponent<Mesh>(Moonlight::MeshType::Cube, mat);

		//mat->ScreenSize = m_camera->OutputSize;
		meshComp.MeshMaterial->DiffuseColor = diffuseColor;
		meshComp.MeshMaterial->Tiling = { 1.f, 1.f };
		meshComp.MeshMaterial->SetTexture(Moonlight::TextureType::Diffuse, defaultDiffuse);
	}
	{
		auto borderEnt = GameWorld->CreateEntity();
		Transform& hitEntMeshTrans2 = borderEnt->AddComponent<Transform>("BorderLeft");
		hitEntMeshTrans2.SetScale(Vector3(.1f, portalHeight, portalDepth));
		hitEntMeshTrans2.SetPosition({ -2.1f, 0.f, 0.f });
		hitEntMeshTrans2.SetParent(hitEntTransform);

		DiffuseMaterial* mat = new DiffuseMaterial();
		Mesh& meshComp = borderEnt->AddComponent<Mesh>(Moonlight::MeshType::Cube, mat);

		//mat->ScreenSize = m_camera->OutputSize;
		meshComp.MeshMaterial->DiffuseColor = diffuseColor;
		meshComp.MeshMaterial->Tiling = { 1.f, 1.f };
		meshComp.MeshMaterial->SetTexture(Moonlight::TextureType::Diffuse, defaultDiffuse);
	}
	{
		auto borderEnt = GameWorld->CreateEntity();
		Transform& hitEntMeshTrans2 = borderEnt->AddComponent<Transform>("BorderRight");
		hitEntMeshTrans2.SetScale(Vector3(.1f, portalHeight, portalDepth));
		hitEntMeshTrans2.SetPosition({ 2.1f, 0.f, 0.f });
		hitEntMeshTrans2.SetParent(hitEntTransform);

		DiffuseMaterial* mat = new DiffuseMaterial();
		Mesh& meshComp = borderEnt->AddComponent<Mesh>(Moonlight::MeshType::Cube, mat);

		//mat->ScreenSize = m_camera->OutputSize;
		meshComp.MeshMaterial->DiffuseColor = diffuseColor;
		meshComp.MeshMaterial->Tiling = { 1.f, 1.f };
		meshComp.MeshMaterial->SetTexture(Moonlight::TextureType::Diffuse, defaultDiffuse);
	}

	{
		auto hitEntMesh = GameWorld->CreateEntity();
		Transform& hitEntMeshTrans = hitEntMesh->AddComponent<Transform>("Mesh");
		hitEntMeshTrans.SetScale(Vector3(2.f, 4.f, 1.f));
		hitEntMeshTrans.SetParent(hitEntTransform);

		PortalMaterial* mat = new PortalMaterial();
		Mesh& meshComp = hitEntMesh->AddComponent<Mesh>(Moonlight::MeshType::Cube, mat);

		mat->ScreenSize = m_camera->OutputSize;
		meshComp.MeshMaterial->DiffuseColor = { 1.f, 1.f, 1.f };
	}

}

void CharacterCore::OnStart()
{
	GetEngine().GetInput().SetMouseCapture(true);

	//SpawnPortal({ 5, 2, 0 }, Vector3::Up, true);
	//SpawnPortal({ -5, 2, 0 }, Vector3::Up, false);

}

void CharacterCore::OnDeserialize(const json& inJson)
{
	if (inJson.contains("MovementSpeed"))
	{
		m_movementSpeed = inJson["MovementSpeed"];
	}
}

#if ME_EDITOR

void CharacterCore::OnSerialize(json& outJson)
{
	outJson["MovementSpeed"] = m_movementSpeed;
}

#endif
