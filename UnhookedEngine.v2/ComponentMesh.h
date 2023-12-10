#pragma once
#include "Component.h"
#include "Application.h"
#include "ImGui/imgui.h"

struct Mesh;

class ComponentMesh : public Component
{
public:
	ComponentMesh();
	ComponentMesh(GameObject* owner);
	~ComponentMesh();

	Mesh* mesh;
	
	std::vector<Mesh*> meshes;

	bool faceNormals;

	void PrintInspector();
	void UpdateAABB();
};