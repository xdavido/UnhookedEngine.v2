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

void ComponentMesh::OnEditor()
{

}
