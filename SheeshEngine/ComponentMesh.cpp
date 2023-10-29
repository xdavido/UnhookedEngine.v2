#include "ComponentMesh.h"

ComponentMesh::ComponentMesh(GameObject* owner) : Component(owner)
{
	mOwner = owner;
	mesh = nullptr;
}

ComponentMesh::ComponentMesh::~ComponentMesh()
{
	App->assimpMeshes->DeleteMesh(mesh);
	mesh = nullptr;
}

void ComponentMesh::PrintInspector()
{
	if (ImGui::CollapsingHeader("Mesh"))
	{
		if (mesh == nullptr) return;
		ImGui::LabelText("##%f", "Number of vertex:");
		ImGui::SameLine();
		ImGui::Text("%d", mesh->vertexCount);
		ImGui::LabelText("##%f", "Number of index:");
		ImGui::SameLine();
		ImGui::Text("%d", mesh->indexCount);
		ImGui::Checkbox("Face Normals", &faceNormals);
	}
}
