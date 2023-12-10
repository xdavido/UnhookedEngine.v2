#pragma once
#include "Module.h"
#include "Globals.h"
#include "ModuleAssimpMeshes.h"
#include "ModuleRenderer3D.h"
#include "ImGui/imgui.h"
#include<string>

using namespace std;

class GameObject;

enum class GeometryType;

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Init();
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void SceneWindow();
	void GameWindow();
	void Raycasting();

	GameObject* CreateGameObject(GameObject* parent);
	ImVec2 NormalizeMouse(float x, float y, float w, float h, ImVec2 pos);

	ImVec2 WindowSize;

	void OnSave();

	GameObject* root = nullptr;
	GameObject* root_Assets = nullptr;
	bool sceneIsSelected = false;

	GameObject* prova1;
	GameObject* prova2;
	GameObject* prova3;
	GameObject* prova4;
	GameObject* prova5;
	GameObject* prova6;

private:

	GameObject* objdebug;
};