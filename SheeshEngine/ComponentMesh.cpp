#include "ComponentMesh.h"
#include "Application.h"

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

	//for (int i = 0; i < meshes.size(); i++) {
	//	Application::GetInstance()->meshRenderer->DeleteMesh(meshes[i]);
	//}
	meshes.clear();
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
