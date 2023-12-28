#include "Globals.h"
#include "Application.h"
#include "ModuleParticles.h"
#include "Transform.h"
#include "Random.h"

ModuleParticles::ModuleParticles(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleParticles::~ModuleParticles()
{

}

bool ModuleParticles::Start()
{
	bool ret = true;

	return ret;
}