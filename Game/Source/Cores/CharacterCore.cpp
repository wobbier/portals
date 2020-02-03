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
#include "Logger.h"
#include "Cores/PhysicsCore.h"
#include "Cores/SceneGraph.h"
#include "Components/Graphics/Model.h"
#include "Components/Physics/Rigidbody.h"
#include "Components/Graphics/Mesh.h"
#include "Components/Portal.h"
#include "Graphics/ShaderCommand.h"
#include "Graphics/Material.h"
#include "RenderCommands.h"

CharacterCore::CharacterCore()
	: Base(ComponentFilter().Requires<Transform>().Requires<Character>().Requires<CharacterController>())
{
	m_bluePortalShot = new AudioSource("Assets/Sounds/portalgun_shoot_blue1.wav");
	m_orangePortalShot = new AudioSource("Assets/Sounds/portalgun_shoot_red1.wav");

	m_invalidPortalSounds.reserve(4);
	for (int i = 0; i < 4; ++i)
	{
		m_invalidPortalSounds.push_back(new AudioSource("Assets/Sounds/Gun/portal_invalid_surface_0" + std::to_string(i+1) + ".wav"));
	}

	AudioCore* audioCore = static_cast<AudioCore*>(GetEngine().GetWorld().lock()->GetCore(AudioCore::GetTypeId()));
	audioCore->InitComponent(*m_bluePortalShot);
	audioCore->InitComponent(*m_orangePortalShot);

	for (int i = 0; i < 4; ++i)
	{
		AudioSource* source = new AudioSource("Assets/Sounds/Gun/portal_invalid_surface_0" + std::to_string(i + 1) + ".wav");
		m_invalidPortalSounds.push_back(source);
		audioCore->InitComponent(*source);
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
	WeakPtr<Entity> cameraEnt = GetEngine().GetWorld().lock()->GetEntity(camera->Parent);
	m_camera = &cameraEnt.lock()->GetComponent<Camera>();
	m_cameraTransform = &cameraEnt.lock()->GetComponent<Transform>();
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
}
#endif

void CharacterCore::HandlePortalShots()
{
	Input& input = GetEngine().GetInput();
	bool isPrimaryFireDown = input.GetMouseState().leftButton;
	if (isPrimaryFireDown && !m_prevPrimaryFireDown)
	{
		if (FirePortal(true))
		{
			m_bluePortalShot->Play(false);
		}
		else
		{
			m_invalidPortalSounds[random(0, m_invalidPortalSounds.size()-1)]->Play();
		}
	}
	m_prevPrimaryFireDown = isPrimaryFireDown;

	bool isSecondaryFireDown = input.GetMouseState().rightButton;
	if (isSecondaryFireDown && !m_prevSecondaryFireDown)
	{
		if (FirePortal(false))
		{
			m_orangePortalShot->Play(false);
		}
		else
		{
			m_invalidPortalSounds[random(0, m_invalidPortalSounds.size()-1)]->Play();
		}
	}
	m_prevSecondaryFireDown = isSecondaryFireDown;
}

void CharacterCore::HandleMouseLook(float dt)
{
	//Vector2 MousePosition = GetEngine().GetInput().GetMousePosition();
	//if (MousePosition == Vector2(0, 0))
	//{
	//	return;
	//}

	Vector2 newPos = m_camera->OutputSize / 2.f;

	DirectX::Mouse::State currentState = GetEngine().GetInput().GetMouseState();
	if (m_firstUpdate || m_previousMouseState.positionMode != currentState.positionMode || GetEngine().GetInput().GetKeyboardState().R)
	{
		GetEngine().GetInput().SetMousePosition(newPos);
		m_previousMouseState = GetEngine().GetInput().GetMouseState();
		currentState = m_previousMouseState;
		//MousePosition = GetEngine().GetInput().GetMousePosition();
		m_lastX = m_previousMouseState.x;
		m_lastY = m_previousMouseState.y;
		m_firstUpdate = false;
		return;
	}


	float XOffset = currentState.x;
	float YOffest = -currentState.y;

	m_lastX = currentState.x;
	m_lastY = currentState.y;
// 	YIKES("X: " + std::to_string(XOffset));
// 	YIKES("Y: " + std::to_string(YOffest));
	Vector3 Front = m_camera->Front;
	//if(currentState.x != m_previousMouseState.x || currentState.y != m_previousMouseState.y)
	{
		XOffset *= LookSensitivity;
		YOffest *= LookSensitivity;

		float Yaw = m_camera->Yaw + XOffset;
		float Pitch = m_camera->Pitch + YOffest;

		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;

		m_camera->Yaw = Yaw;
		m_camera->Pitch = Pitch;

		Front.SetX(cos(Mathf::Radians(Yaw - 90.0f)) * cos(Mathf::Radians(Pitch)));
		Front.SetY(sin(Mathf::Radians(Pitch)));
		Front.SetZ(sin(Mathf::Radians(Yaw - 90.0f)) * cos(Mathf::Radians(Pitch)));
		m_camera->Front = Front.Normalized();
		m_cameraTransform->SetRotation(Vector3(Pitch, 0.0f, 0.0f));
		m_playerTransform->SetRotation(Vector3(0.0f, -Yaw, 0.0f));

		m_previousMouseState = currentState;
	}

	Input& input = GetEngine().GetInput();
	if (input.GetKeyboardState().W)
	{
		m_controller->Walk(Front.Cross(Vector3::Up).Cross(-Vector3::Up).Normalized() * m_movementSpeed * dt);
	}
	if (input.GetKeyboardState().S)
	{
		m_controller->Walk(Front.Cross(Vector3::Up).Cross(Vector3::Up).Normalized() * m_movementSpeed * dt);
	}

	if (input.GetKeyboardState().D)
	{
		m_controller->Walk(Front.Cross(Vector3::Up).Normalized() * m_movementSpeed * dt);
	}
	if (input.GetKeyboardState().A)
	{
		m_controller->Walk(Front.Cross(-Vector3::Up).Normalized() * m_movementSpeed * dt);
	}
	if (input.GetKeyboardState().Space)
	{
		m_controller->Jump();
	}

	GetEngine().GetInput().SetMousePosition(newPos);
	//MousePosition = GetEngine().GetInput().GetMousePosition();
}

bool CharacterCore::FirePortal(bool IsBluePortal)
{
	auto world = GetEngine().GetWorld().lock();
	PhysicsCore* physics = static_cast<PhysicsCore*>(world->GetCore(PhysicsCore::GetTypeId()));

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
	if (physics->Raycast(m_cameraTransform->GetWorldPosition(), m_cameraTransform->GetWorldPosition() + m_camera->Front * 20.0f, ray))
	{
		auto ent = world->GetEntity(ray.What->Parent);
		Transform& trans = ent.lock()->GetComponent<Transform>();
		BRUH("HIT" + trans.Name);
		
		for (int i = 0; i < directions.size(); ++i)
		{
			Vector3 position = ray.Position + directions[i].Cross(ray.Normal).Normalized();
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
		//	auto hitEnt = world->CreateEntity().lock();
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

		auto hitEnt = world->CreateEntity().lock();
		Transform& hitEntTransform = hitEnt->AddComponent<Transform>("Portal");
		hitEntTransform.SetWorldPosition(ray.Position + (ray.Normal * .01f));
		hitEntTransform.SetScale(Vector3(2.f, 2.5f, 0.01f));

		if (ray.Normal.Y() >= 0.98f)
		{
			hitEntTransform.SetRotation(Vector3(90.f, m_playerTransform->GetRotation().Y(), 0.f));
		}
		else
		{
			hitEntTransform.LookAt(ray.Normal);
		}
		Portal& portal = hitEnt->AddComponent<Portal>(IsBluePortal ? Portal::PortalType::Blue : Portal::PortalType::Orange);
		portal.Observer = m_cameraTransform;

		Rigidbody& rigidbody = hitEnt->AddComponent<Rigidbody>();
		rigidbody.SetMass(0.f);

		Moonlight::Material* mat = new Moonlight::Material();
		Moonlight::ShaderCommand* shader = new Moonlight::ShaderCommand("Assets/Shaders/ScreenSpaceShader.hlsl");
		Mesh& meshComp = hitEnt->AddComponent<Mesh>(Moonlight::MeshType::Plane, mat, shader);

		meshComp.MeshMaterial->DiffuseColor = { 1.f, 1.f, 1.f };
	}

	return canFitPortal;
}

void CharacterCore::OnStart()
{
	GetEngine().GetInput().SetMouseCapture(true);

}
