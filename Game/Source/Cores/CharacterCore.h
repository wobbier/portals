#pragma once
#include "ECS/Core.h"
#include "Components/Transform.h"

class AudioSource;

class CharacterCore final
	: public Core<CharacterCore>
{
public:
	CharacterCore();
	~CharacterCore() {}

	virtual void OnEntityAdded(Entity& NewEntity) override;

	virtual void OnEntityRemoved(Entity& InEntity) override;

#if ME_EDITOR
	virtual void OnEditorInspect() override;
#endif

	virtual void Update(float dt) override;

	virtual void Init() override;
private:
	bool m_prevPrimaryFireDown = false;
	bool m_prevSecondaryFireDown = false;
	AudioSource* m_orangePortalShot;
	AudioSource* m_bluePortalShot;
};

ME_REGISTER_CORE(CharacterCore)