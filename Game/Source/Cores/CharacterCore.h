#pragma once
#include "ECS/Core.h"
#include "Components/Transform.h"

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

};

ME_REGISTER_CORE(CharacterCore)