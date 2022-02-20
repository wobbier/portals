#include "PortalMaterial.h"

PortalMaterial::PortalMaterial()
	: Moonlight::Material("PortalMaterial", "Assets/Shaders/ScreenSpaceShader")
	, ScreenSize(1, 1)
{

}

void PortalMaterial::Init()
{
	s_diffuse = bgfx::createUniform("s_diffuse", bgfx::UniformType::Vec4);
	s_tiling = bgfx::createUniform("s_tiling", bgfx::UniformType::Vec4);
	s_screenSize = bgfx::createUniform("s_screenSize", bgfx::UniformType::Vec4);
}

void PortalMaterial::Use()
{
	bgfx::setUniform(s_diffuse, &DiffuseColor.x);
	bgfx::setUniform(s_tiling, &Tiling.x);
	bgfx::setUniform(s_screenSize, &ScreenSize.x);
}

SharedPtr<Moonlight::Material> PortalMaterial::CreateInstance()
{
	return std::make_shared<PortalMaterial>(*this);
}

uint64_t PortalMaterial::GetRenderState(uint64_t state) const
{
	//state &= ~(BGFX_STATE_CULL_CCW);
	return state;
}

