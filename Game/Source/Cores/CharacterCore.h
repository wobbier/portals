#pragma once
#include "ECS/Core.h"
#include "Components/Transform.h"
#include "Math/Random.h"

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
	bool FirePortal(bool IsBluePortal);

	virtual void OnStart() override;

	void OnDeserialize(const json& inJson) override;
#if ME_EDITOR
	void OnSerialize(json& outJson) override;
#endif

	float m_movementSpeed = 100.f;

	float LookSensitivity = 6.f;
	AudioSource* m_orangePortalShot = nullptr;
	AudioSource* m_bluePortalShot = nullptr;
	std::vector<AudioSource*> m_invalidPortalSounds;
	Camera* m_camera = nullptr;
	Transform* m_playerTransform = nullptr;
	Transform* m_cameraTransform = nullptr;
	CharacterController* m_controller = nullptr;
	Random64 random;
};

ME_REGISTER_CORE(CharacterCore)