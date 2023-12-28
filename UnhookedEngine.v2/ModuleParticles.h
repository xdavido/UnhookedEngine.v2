#pragma once
#include "Module.h"
#include "Globals.h"
#include "ModuleRenderer3D.h"
#include "PartycleSystem.h"
#include "Emitter.h"
#include "GameObject.h"

#include <vector>

class ModuleParticles : public Module
{
public:
	ModuleParticles(Application* app, bool start_enabled = true);
	~ModuleParticles();

	bool Start();


private:

};