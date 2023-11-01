#include "GameObject.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"


GameObject::GameObject()
{
	name = "GameObject";
	mParent = nullptr;
	transform = new ComponentTransform();
	isTimetoDelete = false;

	mComponents.push_back(transform);
}

GameObject::~GameObject()
{
	name = "";
	transform = nullptr;

	//Unbind with parent
	if (isTimetoDelete && mParent != nullptr) {
		mParent->DeleteChild(this);
	}

	// pilota (delete del reves?)

	for (size_t i = 0; i < mComponents.size(); ++i)
	{
		delete mComponents[i];
		mComponents[i] = nullptr;
	}

	for (size_t i = 0; i < mChildren.size(); ++i)
	{
		delete mChildren[i];
		mChildren[i] = nullptr;
	}
	/*for (size_t i = mComponents.size(); i >= 0; --i)
	{
		delete mComponents[i];
		mComponents[i] = nullptr;
	}

	for (size_t i = mChildren.size(); i >= 0; --i)
	{
		delete mChildren[i];
		mChildren[i] = nullptr;
	}*/

	mComponents.clear();

}

GameObject::GameObject(GameObject* parent)
{
	name = "GameObject";
	this->mParent = parent;
	isTimetoDelete = false;

	if (parent != nullptr)
	{
		parent->mChildren.push_back(this);
	}

	transform = new ComponentTransform();

	mComponents.push_back(transform);
}

void GameObject::AddComponent(Component* component)
{
	mComponents.push_back(component);
	component->mOwner = this;
}


void GameObject::Update()
{
	// No tots els children o components tenen un update
	for (size_t i = 0; i < mChildren.size(); ++i)
	{
		mChildren[i]->Update();
	}

	for (size_t i = 0; i < mComponents.size(); ++i)
	{
		mComponents[i]->Update();
	}
}

GameObject* GameObject::GetParent()
{
	return mParent;
}

ComponentMesh* GameObject::GetMeshComponent()
{
	for (size_t i = 0; i < mComponents.size(); i++)
	{
		if (mComponents[i]->type == ComponentType::MESH)
		{
			return (ComponentMesh*)mComponents[i];
		}
	}
	return nullptr;
}

ComponentMaterial* GameObject::GetComponentTexture()
{
	for (size_t i = 0; i < mComponents.size(); i++)
	{
		if (mComponents[i]->type == ComponentType::MATERIAL) return (ComponentMaterial*)mComponents[i];
	}
	return nullptr;
}


bool GameObject::CheckChildOf(GameObject* parent)
{
	if (parent == this)
	{
		return true;
	}
	if (parent->mChildren.empty())
	{
		return false;
	}
	for (size_t i = 0; i < parent->mChildren.size(); i++)
	{
		if (CheckChildOf(parent->mChildren[i])) return true;
	}
	return false;
}

void GameObject::DeleteChild(GameObject* child)
{
	for (int i = 0; i < mChildren.size(); i++) {
		if (mChildren[i] == child) {
			mChildren.erase(mChildren.begin() + i);
			child->mParent = nullptr;
		}
	}
}

bool GameObject::SetNewParent(GameObject* newParent)
{
	if (mParent != nullptr) {
		if (newParent->CheckChildOf(this)) return false;

		mParent->DeleteChild(this);
	}

	mParent = newParent;
	newParent->mChildren.push_back(this);

	return true;
}

bool GameObject::SetAsChildOf(GameObject* gameObject)
{
	if (CheckChildOf(gameObject))
	{
		return false;
	}
	gameObject->mParent = this;
	mChildren.push_back(gameObject);
	return true;
}


void GameObject::PrintInspector()
{


	char* listComponenets[]{ "Add Component", "Mesh Component", "Texture Component" };
	char aux[255] = { ' ' }; 


	if (mParent != nullptr)
	{
		strcpy(aux, this->name.c_str());

		//name i enable
		//ImGui::Text(name.c_str());
		

		ImGui::BulletText("Name:");
		ImGui::SameLine();

		//input the name of the Game Object
		ImGui::InputText("##Name", aux, 255, ImGuiInputTextFlags_EnterReturnsTrue);

		if (ImGui::IsKeyDown(ImGuiKey_Enter))
			name = aux;


		if (ImGui::Button("Delete")) {
			
			isTimetoDelete = true;
			delete App->hierarchy->objSelected;
			App->hierarchy->objSelected = nullptr;
		}

		for (size_t i = 0; i < mComponents.size(); i++)
		{
			ImGui::Separator();

			mComponents[i]->PrintInspector();

			//strcpy(*listComponenets, components[i]->nameComponent.c_str());
		}

		ImGui::Separator();
		ImGui::Text("");
		ImGui::Text("");
		ImGui::Text("");

		ImGui::Text("");
		ImGui::SameLine(ImGui::GetWindowWidth() / 6);
		//if (ImGui::Combo("##AddComponent", &componentNum, listComponenets, 3)) //number of total components u can give to a GO
		//{
		//	switch (componentNum) {
		//	case 1:
		//	{
		//		//Mesh component
		//		if (GetMeshComponent() == nullptr) {
		//			ComponentMesh* cm = new ComponentMesh();
		//			AddComponent(cm);
		//		}
		//		else {
		//			LOG("Mesh Component already added, can't duplicate.")
		//		}
		//	}
		//	break;
		//	case 2:
		//	{
		//		//Texture component
		//		if (GetTextureComponent() == nullptr) {
		//			ComponentTexture* ct = new ComponentTexture();
		//			AddComponent(ct);
		//		}
		//		else {
		//			LOG("Texture Component already added, can't duplicate.")
		//		}
		//	}
		//	break;
		//	}
		//	componentNum = 0;
		//}

	}


}
