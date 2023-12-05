#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "GameObject.h"
#include "ImGui/imgui.h"
#include "ImGui/backends/imgui_impl_sdl2.h"
#include "ImGui/backends/imgui_impl_opengl3.h"

class ModuleViewport : public Module
{
public:
	ModuleViewport(Application* app, bool start_enabled = true);
	~ModuleViewport();

	bool Start();

	update_status PreUpdate(float dt);
	update_status Update(float dt);
	update_status PostUpdate(float dt);

	bool CleanUp();

	void Play();
	void Stop();
	void Pause();
	void Step();

	Timer GameTimer;
	float rotation;


	bool paused = false;
	bool playing = false;
	
	int TimeStatus = 2; //default is paused
	float deltaT = 0.f;
	float f;

};