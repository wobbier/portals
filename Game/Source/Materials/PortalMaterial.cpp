#include "PortalMaterial.h"

PortalMaterial::PortalMaterial()
	: Moonlight::Material("PortalMaterial", "Assets/Shaders/ScreenSpaceShader.hlsl")
{

}

void PortalMaterial::Init()
{
}

void PortalMaterial::Use()
{
}

SharedPtr<Moonlight::Material> PortalMaterial::CreateInstance()
{
	return std::make_shared<PortalMaterial>(*this);
}

