#pragma once
enum class ComponentType
{
	NONE,
	CAMERA,
	TRANSFORM,
	MESH,
	MATERIAL
	

};

class GameObject;

class Component {
	
public:
	Component(GameObject* owner);
	virtual ~Component();

	virtual void PrintInspector();
	virtual void Update();

	void EnableComponent();
	void DisableComponent();
	
	ComponentType type;
	bool isActive;

	GameObject*	mOwner;

	
 };