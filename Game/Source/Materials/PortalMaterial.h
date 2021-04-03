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
};

ME_REGISTER_MATERIAL_NAME_FOLDER(PortalMaterial, "Portal", "Game");