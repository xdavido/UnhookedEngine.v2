#include "ComponentMesh.h"
#include "ModuleAssimpMeshes.h"

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

void ComponentMesh::UpdateAABB()
{
	for (int i = 0; i < App->assimpMeshes->meshes.size(); i++) {
		if (App->assimpMeshes->meshes[i] == nullptr)return;

		App->assimpMeshes->meshes[i]->obb = App->assimpMeshes->meshes[i]->localAABB;
		//App->assimpMeshes->meshes[i]->obb.Transform(mOwner->transform->GetTransformMatrix().Transposed());

		App->assimpMeshes->meshes[i]->aabb.SetNegativeInfinity();
		App->assimpMeshes->meshes[i]->aabb.Enclose(App->assimpMeshes->meshes[i]->obb);
	}
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
