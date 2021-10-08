#pragma once
#include "ECS/ComponentDetail.h"
#include "ECS/Component.h"

class PortalTraveller
	: public Component<PortalTraveller>
{
public:
	PortalTraveller()
		: Component("PortalTraveller")
	{
	}

	void Init() final
	{
	}

	Vector3 PreviousOffsetFromPortal;

private:
	void OnSerialize(json& outJson) override
	{
	}

	void OnDeserialize(const json& inJson) override
	{
	}
};

ME_REGISTER_COMPONENT(PortalTraveller)