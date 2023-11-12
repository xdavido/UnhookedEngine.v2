#pragma once
#include "Component.h"

#include "Application.h"
#include "ImGui/imgui.h"
#include <vector>
#include <string>



struct Mesh;

class ComponentMesh : public Component
{
public:
	ComponentMesh();
	ComponentMesh(GameObject* owner);
	~ComponentMesh();

	Mesh* mesh;
	
	bool faceNormals;

	vector<Mesh*> meshes; 

	void PrintInspector();
};