#pragma once
#include "Module.h"
#include "Globals.h"
#include "GameObject.h"
#include "Imgui/imgui.h"
#include "ImGui/backends/imgui_impl_sdl2.h"
#include "Imgui/backends/imgui_impl_opengl3.h"

class ModuleHierarchy : public Module
{
public:
	ModuleHierarchy(Application* app, bool start_enabled = true);
	~ModuleHierarchy();

	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void GameObjectTree(GameObject* obj, int index);
	void SetGameObject(GameObject* obj);

	GameObject* TargetDropped;	
	GameObject* objSelected;
	GameObject* objHovered;
};