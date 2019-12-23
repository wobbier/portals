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
	virtual void OnEditorInspect() override
	{

	}
#endif

	virtual void Init() override
	{
	}

};

ME_REGISTER_COMPONENT(Character)
