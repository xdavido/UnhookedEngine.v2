#pragma once
enum class ComponentType
{
	NONE, 
	TRANSFORM,
	MESH,
	MATERIAL

};

class GameObject;

class Component {
	
public:
	Component(GameObject* owner);
	virtual ~Component();

	virtual void OnEditor();
	virtual void Update();

	void EnableComponent();
	void DisableComponent();
	
	ComponentType type;
	bool isActive;

	GameObject*	mOwner;

	
 };