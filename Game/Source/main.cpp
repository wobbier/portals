#include "PortalsGame.h"
#include "Engine/Engine.h"
#include "ComponentRegistry.h"
int main()
{
	GetEngine().Init(new PortalsGame());
	GetEngine().Run();

	return 0;
}