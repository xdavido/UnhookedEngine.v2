#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleAssimpMeshes.h"
#include "ModuleHierarchy.h"
#include "ModuleRenderer3D.h"
#include"ModuleScene.h"

#include "Primitive.h"

ModuleHierarchy::ModuleHierarchy(Application* app, bool start_enabled) : Module(app, start_enabled)
{

	TargetDropped = nullptr;
	objSelected = nullptr;

	name = "Hierarchy";
	
}

ModuleHierarchy::~ModuleHierarchy()
{}

//-----------------------------------------------------------------
bool ModuleHierarchy::Start()
{
	
	bool ret = true;


	return ret;
}

// -----------------------------------------------------------------
bool ModuleHierarchy::CleanUp()
{
	LOG("Cleaning Hierarchy");
	
	return true;
}

// -----------------------------------------------------------------
update_status ModuleHierarchy::PreUpdate(float dt)
{


	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
update_status ModuleHierarchy::Update(float dt)
{

	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN)
	{
		if (objSelected != nullptr) {
			objSelected->isTimetoDelete = true;
		}
		delete objSelected;
		objSelected = nullptr;
	}




	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
update_status ModuleHierarchy::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}



void ModuleHierarchy::GameObjectTree(GameObject* obj, int index)
{
	ImGuiTreeNodeFlags flag_TNode = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_OpenOnArrow;

	bool isSelected;

	if (obj->GetParent() == nullptr) {
		flag_TNode |= ImGuiTreeNodeFlags_DefaultOpen;
	}
	else {
		flag_TNode |= ImGuiTreeNodeFlags_OpenOnArrow;
	}

	if (obj == objSelected) {
		flag_TNode |= ImGuiTreeNodeFlags_Selected;
	}

	if (obj->mChildren.size() != 0)
		isSelected = ImGui::TreeNodeEx((void*)(intptr_t)index, flag_TNode, obj->name.c_str(), index);

	else {
		flag_TNode |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx((void*)(intptr_t)index, flag_TNode, obj->name.c_str(), index);
		isSelected = false;
	}
	if (obj->GetParent() != nullptr) {

		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("GameObject", obj, sizeof(GameObject*));

			TargetDropped = obj;
			ImGui::Text("Changing...");
			ImGui::EndDragDropSource();
		}

		if (ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByActiveItem))
		{
			objHovered = obj;
			if (ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
			{
				SetGameObject(obj);
			}
		}

	}

	if (ImGui::IsWindowHovered())
	{
		if (!ImGui::IsAnyItemHovered())
		{
			if (ImGui::IsMouseClicked(ImGuiMouseButton_::ImGuiMouseButton_Left))
			{
				objSelected = nullptr;
			}
		}
	}

	if (ImGui::BeginDragDropTarget())
	{
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GameObject")) {

			
				TargetDropped->SetNewParent(objHovered);
			
		}
		ImGui::EndDragDropTarget();
	}

	if (isSelected)
	{
		if (!obj->mChildren.empty()) {
			for (unsigned int i = 0; i < obj->mChildren.size(); i++)
			{
				GameObjectTree(obj->mChildren[i], i);
			}
		}
		ImGui::TreePop();
	}

}

void ModuleHierarchy::SetGameObject(GameObject* obj)
{
	objSelected = obj;
}