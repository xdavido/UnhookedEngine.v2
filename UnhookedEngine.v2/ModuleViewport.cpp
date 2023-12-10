#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleViewport.h"
#include "ModuleRenderer3D.h"
#include "../UnhookedEngine.v2/Glew/include/glew.h"
#include "Primitive.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"

ModuleViewport::ModuleViewport(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}
ModuleViewport::~ModuleViewport()
{}

bool ModuleViewport::Start()
{
	bool ret = true;
	return ret;
}
update_status ModuleViewport::PreUpdate(float dt)
{
	switch (TimeStatus) {
	case 1: //playing
		deltaT = (float)GameTimer.Read() / 1000.0f;
		GameTimer.Stop();
		GameTimer.Start();
		break;
	case 2: //stop
		GameTimer.Stop();
		App->renderer3D->mainCam->mOwner->transform->rotation.y = 0;
		App->renderer3D->mainCam->mOwner->transform->UpdateMatrixFromInspector();
		deltaT = 0;
		rotation = 0;
		playing = false;
	case 3: //pause	
		playing = false;
		break;
	case 4:
		break;
	}
	return UPDATE_CONTINUE;
}
update_status ModuleViewport::Update(float dt)
{
	rotation = 1;
	f += deltaT;

	if (TimeStatus == 1)
	{
		if (f > 0.03f) {
			App->renderer3D->mainCam->mOwner->transform->rotation.y += rotation;
			App->renderer3D->mainCam->mOwner->transform->UpdateMatrixFromInspector();
			if (App->renderer3D->mainCam->mOwner->transform->rotation.y == 360) {
				App->renderer3D->mainCam->mOwner->transform->rotation.y = 0;
			}
			f = 0.0f;
		}
	}
	return UPDATE_CONTINUE;
}
update_status ModuleViewport::PostUpdate(float dt)
{
	return UPDATE_CONTINUE;
}
bool ModuleViewport::CleanUp()
{
	return true;
}
void ModuleViewport::Play()
{
	if (playing == false) {
		LOG("PLAYING");
		playing = true;
		TimeStatus = 1;
	}
	else if (playing == true) { //stopping the game if the game was already playing
		LOG("NOT PLAYING");
		TimeStatus = 2;
		deltaT = 0;
		GameTimer.Stop();
		playing = false;
	}
}

void ModuleViewport::Stop()
{
	deltaT = 0;
	ImGui::SetWindowFocus("Scene");
	TimeStatus = 2;
}

void ModuleViewport::Pause()
{
	if (playing == true) {
		TimeStatus = 3;
		playing = false;
	}
	else {
		TimeStatus = 1;
		playing = true;
	}
}

void ModuleViewport::Step()
{}