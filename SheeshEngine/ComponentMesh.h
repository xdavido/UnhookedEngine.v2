#pragma once
#include "Component.h"

#include "Application.h"
#include "ImGui/imgui.h"

struct Mesh;

class ComponentMesh : public Component
{
public:
	ComponentMesh(GameObject* owner);
	~ComponentMesh();

	Mesh* mesh;

	bool faceNormals;

	void PrintInspector();
};