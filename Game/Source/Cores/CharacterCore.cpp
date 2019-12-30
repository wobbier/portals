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

CharacterCore::CharacterCore()
	: Base(ComponentFilter().Requires<Transform>().Requires<Character>())
{
	m_bluePortalShot = new AudioSource("Assets/Sounds/portalgun_shoot_blue1.wav");
	m_orangePortalShot = new AudioSource("Assets/Sounds/portalgun_shoot_red1.wav");

	AudioCore* audioCore = static_cast<AudioCore*>(GetEngine().GetWorld().lock()->GetCore(AudioCore::GetTypeId()));
	audioCore->InitComponent(*m_bluePortalShot);
	audioCore->InitComponent(*m_orangePortalShot);

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
	Base::OnEditorInspect();
}
#endif

void CharacterCore::Update(float dt)
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

void CharacterCore::Init()
{
}
