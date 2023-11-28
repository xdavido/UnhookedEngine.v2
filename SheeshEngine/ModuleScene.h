#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include <string>

class GameObject;
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

	GameObject* CreateGameObject(GameObject* parent);
	GameObject* root = nullptr;
	GameObject* prova1;
	GameObject* prova2;
	GameObject* prova3;
	GameObject* prova4;
	GameObject* prova5;
	GameObject* prova6;

	void SaveSceneRequest() { saveSceneRequest = true; }
	void LoadSceneRequest() { loadSceneRequest = true; }

private:
	void UpdateGameObjects();
	bool SaveScene();
	void SaveGameObjects(GameObject* parent, JsonParser& rootFile);
	GameObject* LoadGameObject(JsonParser parent, GameObject* father = nullptr);
	void LoadComponents(JsonParser& parent, std::string num, GameObject* gamObj);

	float4x4 strMatrixToF4x4(const char* convert);
	
	const char* FormatComponentType(GameObject* parentGO, const size_t& i);
	bool LoadScene();

	JsonParser jsonFile;
	JSON_Value* rootFile;
	JsonParser rootGO;

	bool saveSceneRequest = false;
	bool loadSceneRequest = false;
};