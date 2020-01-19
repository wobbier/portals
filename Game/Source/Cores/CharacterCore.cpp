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

CharacterCore::CharacterCore()
	: Base(ComponentFilter().Requires<Transform>().Requires<Character>().Requires<CharacterController>())
{
	m_bluePortalShot = new AudioSource("Assets/Sounds/portalgun_shoot_blue1.wav");
	m_orangePortalShot = new AudioSource("Assets/Sounds/portalgun_shoot_red1.wav");

	AudioCore* audioCore = static_cast<AudioCore*>(GetEngine().GetWorld().lock()->GetCore(AudioCore::GetTypeId()));
	audioCore->InitComponent(*m_bluePortalShot);
	audioCore->InitComponent(*m_orangePortalShot);

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
		m_bluePortalShot->Play(false);
		FirePortal();
	}
	m_prevPrimaryFireDown = isPrimaryFireDown;

	bool isSecondaryFireDown = input.GetMouseState().rightButton;
	if (isSecondaryFireDown && !m_prevSecondaryFireDown)
	{
		m_orangePortalShot->Play(false);
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

	Vector2 newPos = GetEngine().MainCamera.OutputSize / 2.f;

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
		m_cameraTransform->SetRotation(Vector3(Mathf::Radians(Pitch), 0.0f, 0.0f));
		m_playerTransform->SetRotation(Vector3(0.0f, -Mathf::Radians(Yaw), 0.0f));

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

void CharacterCore::FirePortal()
{
	auto world = GetEngine().GetWorld().lock();
	PhysicsCore* physics = static_cast<PhysicsCore*>(world->GetCore(PhysicsCore::GetTypeId()));

	RaycastHit ray;
	if (physics->Raycast(m_cameraTransform->GetWorldPosition(), m_cameraTransform->GetWorldPosition() + m_camera->Front * 20.0f, ray))
	{
		auto ent = world->GetEntity(ray.What->Parent);
		Transform& trans = ent.lock()->GetComponent<Transform>();
		BRUH("HIT" + trans.Name);

		auto hitEnt = world->CreateEntity().lock();
		Transform& hitEntTransform = hitEnt->AddComponent<Transform>("Portal");
		hitEntTransform.SetWorldPosition(ray.Position);
		hitEntTransform.SetScale(0.1f);
		hitEnt->AddComponent<Model>("Assets/Cube.fbx");
	}
}

void CharacterCore::OnStart()
{
	GetEngine().GetInput().SetMouseCapture(true);
}
