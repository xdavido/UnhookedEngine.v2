#pragma once
#include "Module.h"
#include <vector>
#include <string>
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"

enum class ComponentType;
class Component;
class ComponentTransform;
class ComponentMesh;
class GameObject
{
public:

	GameObject();
	GameObject(GameObject* parent);
	~GameObject();

	void AddComponent(ComponentType type);
	void Update();
	GameObject* GetParent();
	bool SetNewParent(GameObject* newParent);
	bool CheckChildOf(GameObject* parent);
	void DeleteChild(GameObject* child);


	ComponentMesh* GetMeshComponent();


	std::string name;

	GameObject* mParent;
	std::vector<GameObject*> mChildren;
	std::vector<Component*> mComponents;

	ComponentTransform* transform;

};