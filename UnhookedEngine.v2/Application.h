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
#include"ModuleParticles.h"
#include "JsonParser.h"


#include<vector>

enum class GameState
{
	PLAY,
	PAUSE,
	STOP
};

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
	ModuleParticles* particle;


private:

	Timer	ms_timer;
	Timer game_timer;
	float gamedt;
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

	float GetGameDT();
	void SetGameDT();
	void StopGameDT();
	void PauseGameDT();


public:

	Application();
	~Application();

	static Application* GetApp();


	bool Init();
	update_status Update();
	bool CleanUp();

	void LOGToEditor(const char* text);

	float GetDt() const { return dt; }
	float GetFrameRate() const { return 1.f / dt; }

	void SaveConfig();
	void LoadConfig();

	void SetDT(float dt);
	float DTG();
	bool IsRunning();
	bool IsPaused();
	GameState GetState();
	void SetState(GameState gameState);

	float dtG;
	GameState gameState = GameState::STOP;

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

extern Application* App;