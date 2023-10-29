#pragma once
#include "ComponentTransform.h"
#include "ModuleAssimpMeshes.h"
#include <vector>
#include <string>

enum class ComponentType;
class Component;
class ComponentTransform;
class ComponentMesh;

class GameObject
{
public:
	GameObject(GameObject* parent);
	~GameObject();

	void Update();

	GameObject* GetParent();
	void SetParent(GameObject* parent);
	
	std::vector<GameObject*> GetChildren();
	void AddChild(GameObject* child);

	std::vector<Component*> GetComponents();
	void CreateComponent(ComponentType type);

	void DeleteChild(GameObject* child);

	bool ChangeParent(GameObject* NewParent);

	bool CheckChildOf(GameObject* parent);

	ComponentMesh* GetComponentMesh();
	

public:
	std::string name;
	ComponentTransform* transform;


private:
	GameObject* mParent;
	std::vector<GameObject*>	mChildren;
	std::vector<Component*>		mComponents;
	
	 
};