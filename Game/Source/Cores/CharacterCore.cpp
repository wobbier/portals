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
	bool isPrimaryFireDown = Input::GetInstance().GetMouseState().leftButton;
	if (isPrimaryFireDown && !m_prevPrimaryFireDown)
	{
		m_bluePortalShot->Play(false);
	}
	m_prevPrimaryFireDown = isPrimaryFireDown;

	bool isSecondaryFireDown = Input::GetInstance().GetMouseState().rightButton;
	if (isSecondaryFireDown && !m_prevSecondaryFireDown)
	{
		m_orangePortalShot->Play(false);
	}
	m_prevSecondaryFireDown = isSecondaryFireDown;
}

void CharacterCore::HandleMouseLook(float dt)
{
	Vector2 MousePosition = Input::GetInstance().GetMousePosition();
	if (MousePosition == Vector2(0, 0))
	{
		return;
	}

	if (m_firstUpdate || Input::GetInstance().GetKeyboardState().R)
	{
		m_lastX = MousePosition.X();
		m_lastY = MousePosition.Y();
		m_firstUpdate = false;
	}

	float XOffset = MousePosition.X() - m_lastX;
	float YOffest = m_lastY - MousePosition.Y();
	m_lastX = MousePosition.X();
	m_lastY = MousePosition.Y();

	XOffset *= LookSensitivity;
	YOffest *= LookSensitivity;

	const float Yaw = m_camera->Yaw += XOffset;
	float Pitch = m_camera->Pitch += YOffest;

	if (Pitch > 89.0f)
		Pitch = 89.0f;
	if (Pitch < -89.0f)
		Pitch = -89.0f;

	Vector3 Front;
	Front.SetX(cos(Mathf::Radians(Yaw - 90.0f)) * cos(Mathf::Radians(Pitch)));
	Front.SetY(sin(Mathf::Radians(Pitch)));
	Front.SetZ(sin(Mathf::Radians(Yaw - 90.0f)) * cos(Mathf::Radians(Pitch)));
	m_camera->Front = Front.Normalized();
	//m_playerTransform->Rotation.GetInternalVec().Forward = Front.GetInternalVec();
	m_cameraTransform->SetRotation(Vector3(Mathf::Radians(Pitch), 0.0f, 0.0f));
	m_playerTransform->SetRotation(Vector3(0.0f, -Mathf::Radians(Yaw), 0.0f));
	if (Input::GetInstance().GetKeyboardState().W)
	{
		//m_playerTransform->Translate(Front.Cross(Vector3::Up).Cross(-Vector3::Up).Normalized() * m_movementSpeed * dt);
		m_controller->Walk(Front.Cross(Vector3::Up).Cross(-Vector3::Up).Normalized() * m_movementSpeed * dt);
		//m_playerTransform->SetPosition(m_playerTransform->GetPosition() - Vector3(0.0f, 0.0f, m_movementSpeed * dt));
	}
	if (Input::GetInstance().GetKeyboardState().S)
	{
		//m_playerTransform->Translate(Front.Cross(Vector3::Up).Cross(Vector3::Up).Normalized() * m_movementSpeed * dt);
		m_controller->Walk(Front.Cross(Vector3::Up).Cross(Vector3::Up).Normalized() * m_movementSpeed * dt);
	}

	if (Input::GetInstance().GetKeyboardState().D)
	{
		//m_playerTransform->Translate(Front.Cross(Vector3::Up).Normalized() * m_movementSpeed * dt);
		m_controller->Walk(Front.Cross(Vector3::Up).Normalized() * m_movementSpeed * dt);
	}
	if (Input::GetInstance().GetKeyboardState().A)
	{
		//m_playerTransform->Translate(Front.Cross(-Vector3::Up).Normalized() * m_movementSpeed * dt);
		m_controller->Walk(Front.Cross(-Vector3::Up).Normalized() * m_movementSpeed * dt);
	}
	if (Input::GetInstance().GetKeyboardState().Space)
	{
		m_controller->Jump();
	}
}
