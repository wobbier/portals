#pragma once
#include "ECS/ComponentDetail.h"
#include "ECS/Component.h"

class Portal
	: public Component<Portal>
{
public:
	enum class PortalType : uint8_t
	{
		Blue = 0,
		Orange = 1
	};

	Portal()
		: Component("Portal")
	{

	}

	Portal(PortalType InType)
		: Component("Portal")
		, Type(InType)
	{

	}

	void Init() final
	{
	}
	PortalType Type = PortalType::Blue;
};

ME_REGISTER_COMPONENT(Portal)