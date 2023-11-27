#include "ComponentMesh.h"

ComponentMesh::ComponentMesh() : Component(nullptr)
{
	type = ComponentType::MESH;
	mesh = nullptr;
}
ComponentMesh::ComponentMesh(GameObject* owner) : Component(owner)
{
	type = ComponentType::MESH;
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
		ImGui::Checkbox("Face Normals", &faceNormals);
	}
}
