#pragma once
#include "Graphics/Material.h"

class PortalMaterial
	: public Moonlight::Material
{
public:
	PortalMaterial();

	void Init() override;

	void Use() override;

	SharedPtr<Material> CreateInstance() override;

	Vector2 ScreenSize;

	bgfx::UniformHandle s_diffuse;
	bgfx::UniformHandle s_tiling;
	bgfx::UniformHandle s_screenSize;

	uint64_t GetRenderState(uint64_t state) const final;
};

ME_REGISTER_MATERIAL_NAME_FOLDER(PortalMaterial, "Portal", "Game");