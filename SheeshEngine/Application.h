#pragma once

#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleEditor.h"
#include "ModuleAssimpMeshes.h"
#include "ModuleTexture.h"
#include "ModuleScene.h"
#include"ModuleHierarchy.h"
#include "JsonParser.h"

#include<vector>

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleEditor* editor;
	ModuleAssimpMeshes* assimpMeshes;
	ModuleTexture* textures;
	ModuleScene* scene;
	ModuleHierarchy* hierarchy;

private:

	Timer	ms_timer;
	std::vector<Module*> list_modules;

public:
	float	dt;
	uint	maxFrameRate;
	float	msLastFrame;

	JsonParser jsonFile;

	bool saveRequested;
	bool loadRequested;

	void SaveConfigRequest() { saveRequested = true; };
	void LoadConfigRequest() { loadRequested = true; }


public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	void LOGToEditor(const char* text);

	float GetDt() const { return dt; }
	float GetFrameRate() const { return 1.f / dt; }

	void SaveConfig();
	void LoadConfig();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

extern Application* App;