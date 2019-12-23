#include "CharacterCore.h"
#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Engine/Input.h"
#include "Events/EventManager.h"
#include "optick.h"

CharacterCore::CharacterCore()
	: Base(ComponentFilter().Requires<Transform>())
{

}

void CharacterCore::OnEntityAdded(Entity& NewEntity)
{
}

void CharacterCore::OnEntityRemoved(Entity& InEntity)
{
}

#if ME_EDITOR
void CharacterCore::OnEditorInspect()
{
}
#endif

void CharacterCore::Update(float dt)
{
}

void CharacterCore::Init()
{
}
