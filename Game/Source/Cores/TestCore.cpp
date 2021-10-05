#include "TestCore.h"
#include "Components/Transform.h"
#include "Components/Camera.h"
#include "Engine/Input.h"
#include "Events/EventManager.h"
#include "optick.h"
#include "Engine/World.h"
#include "Components/Graphics/Model.h"

TestCore::TestCore() : Base(ComponentFilter().Requires<Transform>())
{

}

void TestCore::OnEntityAdded(Entity& NewEntity)
{
}

void TestCore::OnEntityRemoved(Entity& InEntity)
{
}

#if ME_EDITOR
void TestCore::OnEditorInspect()
{
}
#endif

void TestCore::Update(float dt)
{
}

void TestCore::Init()
{
}

void TestCore::OnStart()
{
	{
		auto ent = GameWorld->CreateEntity();
		auto& trans = ent->AddComponent<Transform>("Red");
		ent->AddComponent<Model>("Assets/Cube.fbx");
		trans.SetPosition({-1,0,0});
	}
	{
		auto ent = GameWorld->CreateEntity();
		auto& trans = ent->AddComponent<Transform>("Blue");
		ent->AddComponent<Model>("Assets/Cube.fbx");
		trans.SetPosition({ 1,0,0 });
		trans.Rotate({ 0,90,0 });
	}
}
