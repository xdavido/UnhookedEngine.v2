#include "Globals.h"
#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleViewport.h"
#include "ModuleRenderer3D.h"
#include "../UnhookedEngine.v2/Glew/include/glew.h"
#include "Primitive.h"
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
		GameTimer.Start();
		break;
	case 2: //stop
		GameTimer.Stop();
		deltaT = 0;
		break;
	case 3: //pause	
		GameTimer.Stop();
		break;
	case 4:
		//Step();
		break;
	}

	return UPDATE_CONTINUE;
}
update_status ModuleViewport::Update(float dt)
{
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
		deltaT = (float)GameTimer.Read() / 1000.0f;
	}
	else if (playing == true) { //stopping the game if the game was already playing
		LOG("NOT PLAYING");
		TimeStatus = 2;
		playing = false;
	}

}

void ModuleViewport::Stop()
{
	TimeStatus = 2;

}

void ModuleViewport::Pause()
{
	if (paused == true) {
		TimeStatus = 1;
		paused = false;
		deltaT = (float)GameTimer.Read() / 1000.0f;
	}
	else {
		TimeStatus = 3;
		paused = true;
	}
}



void ModuleViewport::Step()
{
			/*Play();
			Stop();*/
}

	

