#pragma once
#include "ECS/Core.h"
#include "Components/Transform.h"

class AudioSource;
class Camera;
class Transform;

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
	void HandleMouseLook();
	void HandlePortalShots();

	bool m_prevPrimaryFireDown = false;
	bool m_prevSecondaryFireDown = false;
	bool m_firstUpdate = false;

	float LookSensitivity = .15f;
	float m_lastX = 0.0f;
	float m_lastY = 0.0f;
	AudioSource* m_orangePortalShot = nullptr;
	AudioSource* m_bluePortalShot = nullptr;
	Camera* m_camera = nullptr;
	Transform* m_playerTransform = nullptr;
	Transform* m_cameraTransform = nullptr;
};

ME_REGISTER_CORE(CharacterCore)