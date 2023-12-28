#pragma once
enum class ComponentType
{
	NONE, 
	TRANSFORM,
	MESH,
	MATERIAL,
	CAMERA,
	EMITTER,

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