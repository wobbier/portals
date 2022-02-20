#pragma once
#include "Game.h"
#include "Pointers.h"
#include "ECS/Entity.h"
#include "Cores/TestCore.h"
#include "ComponentRegistry.h"

class PortalsGame
	: public Game
{
public:
	PortalsGame(int argc, char** argv);
	~PortalsGame();

	virtual void OnInitialize() override;

	virtual void OnStart() override;
	virtual void OnUpdate(const UpdateContext& inUpdateContext) override;
	virtual void OnEnd() override;

	virtual void PostRender() override;
};