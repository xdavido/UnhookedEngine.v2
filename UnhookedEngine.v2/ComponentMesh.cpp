#include "ComponentMesh.h"

ComponentMesh::ComponentMesh() : Component(nullptr)
{
	type = ComponentType::MESH;

	faceNormals = false;
}
ComponentMesh::ComponentMesh(GameObject* owner) : Component(owner)
{
	type = ComponentType::MESH;
	mOwner = owner;
	
	faceNormals = false;
}

ComponentMesh::ComponentMesh::~ComponentMesh()
{
	for (int i = 0; i < meshes.size(); i++)
	{
		App->assimpMeshes->DeleteMesh(meshes[i]);
	}
	meshes.clear();
	
}

void ComponentMesh::PrintInspector()
{
	if (ImGui::CollapsingHeader("Mesh"))
	{
		if (meshes.empty()) return;
		for (int i = 0; i < meshes.size(); i++)
		{
			ImGui::LabelText("##%f", "Number of vertex:");
			ImGui::SameLine();
			ImGui::Text("%d", meshes[i]->vertexCount);
			ImGui::LabelText("##%f", "Number of index:");
			ImGui::Checkbox("Face Normals", &faceNormals);
		}
		
	}
}
