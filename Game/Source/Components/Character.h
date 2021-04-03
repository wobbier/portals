#pragma once

class Character
	: public Component<Character>
{
public:
	Character()
		: Component("Character")
	{

	}

#if ME_EDITOR
	virtual void OnEditorInspect() final
	{

	}
#endif

	virtual void Init() final
	{
	}

private:
	void OnSerialize(json& outJson) final
	{
	}

	void OnDeserialize(const json& inJson) final
	{
	}
};

ME_REGISTER_COMPONENT(Character)
