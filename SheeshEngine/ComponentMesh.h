#pragma once
#include "Component.h"
#include "ModuleAssimpMeshes.h"
#include "Application.h"

class ComponentMesh : public Component
{
public:
	ComponentMesh(GameObject* owner);
	~ComponentMesh();

	Mesh* mesh;

	void OnEditor();
};