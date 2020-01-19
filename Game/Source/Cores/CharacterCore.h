#pragma once
#include "ECS/Core.h"
#include "Components/Transform.h"
#include <Mouse.h>

class AudioSource;
class Camera;
class Transform;
class CharacterController;

class CharacterCore final
	: public Core<CharacterCore>
{
public:
	CharacterCore();
	~CharacterCore() = default;

	virtual void Init() override;

	virtual void OnEntityAdded(Entity& NewEntity) override;

	virtual void OnEntityRemoved(Entity& InEntity) override;

	virtual void Update(float dt) override;

#if ME_EDITOR
	virtual void OnEditorInspect() override;
#endif

private:
	void HandleMouseLook(float dt);
	void HandlePortalShots();
	void FirePortal();

	virtual void OnStart() override;

	float m_movementSpeed = 10.f;

	bool m_prevPrimaryFireDown = false;
	bool m_prevSecondaryFireDown = false;
	bool m_firstUpdate = true;

	float LookSensitivity = .3f;
	float m_lastX = 0.0f;
	float m_lastY = 0.0f;
	AudioSource* m_orangePortalShot = nullptr;
	AudioSource* m_bluePortalShot = nullptr;
	Camera* m_camera = nullptr;
	Transform* m_playerTransform = nullptr;
	Transform* m_cameraTransform = nullptr;
	CharacterController* m_controller = nullptr;

	DirectX::Mouse::State m_previousMouseState;
};

ME_REGISTER_CORE(CharacterCore)