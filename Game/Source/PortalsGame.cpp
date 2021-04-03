#include "PortalsGame.h"
#include "Engine/Engine.h"
#include "ECS/Component.h"
#include "Engine/Clock.h"
#include "Components/Transform.h"
#include "ECS/Entity.h"
#include <string>
#include "Engine/Input.h"
#include "Components/Camera.h"
#include "Components/Physics/Rigidbody.h"
#include "Components/Graphics/Model.h"
#include "Components/Lighting/Light.h"
#include "Components/Cameras/FlyingCamera.h"
#include "Cores/TestCore.h"
#include <memory>
#include "Engine/World.h"
#include "Path.h"
#include "Game.h"
#include "Events/SceneEvents.h"

PortalsGame::PortalsGame(int argc, char** argv)
	: Game(argc, argv)
{
}

PortalsGame::~PortalsGame()
{
}

void PortalsGame::OnStart()
{
}

void PortalsGame::OnUpdate(float DeltaTime)
{
	Input& Instance = GetEngine().GetInput();
}

void PortalsGame::OnEnd()
{
}

void PortalsGame::OnInitialize()
{
	NewSceneEvent evt;
	evt.Fire();
	GetEngine().GetWorld().lock()->Start();
	GetEngine().LoadScene("Assets/Main.lvl");
	GetEngine().GetWorld().lock()->Simulate();
	GetEngine().GetWorld().lock()->Start();
}

void PortalsGame::PostRender()
{
}