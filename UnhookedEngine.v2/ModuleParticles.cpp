#include "Globals.h"
#include "Application.h"
#include "ModuleParticles.h"
#include "ComponentTransform.h"
#include "Emitter.h"
#include "ParticleSystem.h"
#include "Random.h"
#include <fstream>

ModuleParticles::ModuleParticles(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	//fireworksList.resize(100);
}

ModuleParticles::~ModuleParticles()
{

}

bool ModuleParticles::Start()
{
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
update_status ModuleParticles::PreUpdate(float dt)
{


	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
update_status ModuleParticles::Update(float dt)
{

	for (int i = 0; i < emitterVector.size(); i++) {
		emitterVector[i]->Update();
	}

	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) {
		LOG("Pressed 1");
		castFirework();
	}


	for (int i = 0; i < fireworksList.size(); i++)
	{
		if (!fireworksList[i]->isActive)
			continue;

		if (fireworksList[i]->LifeTime <= 0.0f)
		{
			if (!fireworksList[i]->explosion) {
				fireworksList[i]->speed = float3::zero;
				fireworksList[i]->GetComponentEmitter()->particleProps.speed = float3(1.0f, 1.0f, 1.0f);
				fireworksList[i]->GetComponentEmitter()->particleProps.speedVariation = float3(50.0f, 50.0f, 50.0f);
				fireworksList[i]->GetComponentEmitter()->particleProps.LifeTime = 10.0f;
				fireworksList[i]->explosion = true;
				fireworksList[i]->LifeTime = 25.0f;
			}
			else {
				fireworksList[i]->isActive = false;
				fireworksList[i]->GetComponentEmitter()->isActive = false;
				continue;
			}
		}

		fireworksList[i]->LifeTime -= Application::GetApp()->dt;
		fireworksList[i]->transform->position += fireworksList[i]->speed * Application::GetApp()->dt;
		fireworksList[i]->transform->calculateMatrix();
	}

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
update_status ModuleParticles::PostUpdate(float dt)
{


	return UPDATE_CONTINUE;
}

bool ModuleParticles::CleanUp()
{
	//Delete emitter array
	for (int i = 0; i < emitterVector.size(); i++) {
		delete emitterVector[i];
		emitterVector[i] = nullptr;
	}
	emitterVector.clear();

	return true;
}



bool ModuleParticles::SaveParticleSystem(const char* filename)
{
	LOG("Saving particle system");

	std::ofstream outputFile(filename);

	if (outputFile.is_open())
	{
		// Save particle system state
		outputFile << "Emitters\n{\n";

		for (int i = 0; i < emitterVector.size(); ++i)
		{
			outputFile << "  Emitter " << i << "\n";
			emitterVector[i]->SaveEmitterState(outputFile);
		}

		outputFile << "}\n";

		outputFile.close();
		return true;
	}
	else
	{
		LOG("Unable to open file for writing: %s", filename);
		return false;
	}
}

void ModuleParticles::castFirework() {
	LOG("Casting Firework");

	GameObject* f = new GameObject();

	f->name = "firework";

	float posX = Random::RandomFloat() * 10;
	float posZ = Random::RandomFloat() * 10;

	f->transform->position = float3(posX, 2.0f, posZ);
	f->transform->calculateMatrix();
	CEmitter* emitter2 = new CEmitter(f);

	int randomText = 1 + (rand() % 4);
	switch (randomText) {
	case 1:
		emitter2->textureID = App->textures->LoadParticleTexture("Assets/VFX/fire1.png");
		break;
	case 2:
		emitter2->textureID = App->textures->LoadParticleTexture("Assets/VFX/fire2.png");
		break;
	case 3:
		emitter2->textureID = App->textures->LoadParticleTexture("Assets/VFX/fire3.png");
		break;
	case 4:
		emitter2->textureID = App->textures->LoadParticleTexture("Assets/VFX/fire4.png");
		break;
	default:
		emitter2->textureID = App->textures->LoadParticleTexture("Assets/VFX/fire5.png");
		break;
	}

	emitter2->RefreshParticleText();

	emitter2->particleProps.speed = float3(0.0f, -10.0f, 0.0f);
	emitter2->particleProps.speedVariation = float3(1.0f, -1.0f, 1.0f);
	emitter2->particleProps.LifeTime = 5.0f;

	float Objspd = Random::RandomFloat() * 50;

	f->speed = float3(0.0f, Objspd, 0.0f);

	f->LifeTime = 50.0f;
	f->isActive = true;

	f->mComponents.push_back(emitter2);

	fireworksList.push_back(f);

}

bool ModuleParticles::LoadParticleSystem(const char* filename)
{
	LOG("Loading particle system");

	std::ifstream inputFile(filename);

	if (inputFile.is_open())
	{
		// Load particle system state
		std::string line;
		while (std::getline(inputFile, line))
		{
			// Parse each line to identify emitters and load their states
			if (line.find("Emitter") != std::string::npos)
			{
				int emitterIndex;
				if (sscanf(line.c_str(), "  Emitter %d", &emitterIndex) == 1 && emitterIndex >= 0 && emitterIndex < emitterVector.size())
				{
					emitterVector[emitterIndex]->LoadEmitterState(inputFile);
				}
			}
		}

		inputFile.close();
		return true;
	}
	else
	{
		LOG("Unable to open file for reading: %s", filename);
		return false;
	}
}