#include "Application.h"
#include "Emitter.h"
#include "ComponentTransform.h"
#include "ModuleParticles.h"
#include "GameObject.h"


CEmitter::CEmitter() : Component(nullptr)
{
	//type = ComponentType::EMITTER;
	//mOwner = nullptr;

	////Default Particle
	//particleProps.pos = float3::zero;
	//particleProps.rot = float3::zero;
	//particleProps.beginScale = float3::one;
	//particleProps.endScale = float3::zero;
	//particleProps.speed = float3(0.0f, 1.0f, 0.0f);
	//particleProps.speedVariation = float3::one;
	//particleProps.Color = float4(255.0f, 255.0f, 255.0f, 1.0f); //r g b a
	//particleProps.endColor = float4(0.0f, 0.0f, 0.0f, 0.0f); //r g b a

	//particleProps.LifeTime = 1.0f;

	//textureID = Application::GetApp()->textures->checkersID;
	//particleSystem.textID = Application::GetApp()->textures->checkersID;
	//particleSystem.ParticleBuffer();

	//isActive = true;

	//Application::GetApp()->particle->emitterVector.push_back(this);

}

CEmitter::CEmitter(GameObject* owner) : Component(owner)
{
	type = ComponentType::EMITTER;
	this->mOwner = owner;

	//Default Particle
	particleProps.pos = owner->transform->position;
	particleProps.rot = float3::zero;
	particleProps.beginScale = float3::one;
	particleProps.endScale = float3::zero;
	particleProps.speed = float3(0.0f, 1.0f, 0.0f);
	particleProps.speedVariation = float3::one;
	particleProps.Color = float4(255.0f, 255.0f, 255.0f, 1.0f); //r g b a
	particleProps.endColor = float4(0.0f, 0.0f, 0.0f, 0.0f); //r g b a

	particleProps.LifeTime = 1.0f;

	textureID = Application::GetApp()->textures->checkersID;
	particleSystem.textID = Application::GetApp()->textures->checkersID;
	particleSystem.ParticleBuffer();

	isActive = true;

	Application::GetApp()->particle->emitterVector.push_back(this);

}

CEmitter::~CEmitter()
{

}

void CEmitter::Update()
{
	if (isActive) {
		particleProps.pos = mOwner->transform->position;

		particleSystem.Emit(particleProps);

		particleSystem.Update();
	}



}

void CEmitter::RenderParticles()
{
	if (isActive) {
		particleSystem.Render();
	}
}

void CEmitter::PrintInspector()
{

	if (ImGui::CollapsingHeader("Particle Emitter"))
	{
		ImGui::Text("X\t\t Y\t\t Z");
		ImGui::InputFloat3("Particle Begin Scale", particleProps.beginScale.ptr());
		ImGui::Text("X\t\t Y\t\t Z");
		ImGui::InputFloat3("Particle End Scale", particleProps.endScale.ptr());
		ImGui::NewLine();
		ImGui::Text("X\t\t Y\t\t Z");
		ImGui::InputFloat3("Speed", particleProps.speed.ptr());
		ImGui::Text("X\t\t Y\t\t Z");
		ImGui::InputFloat3("Speed Variaton", particleProps.speedVariation.ptr());
		ImGui::InputFloat("Life Time", &particleProps.LifeTime);
		ImGui::NewLine();
		ImGui::ColorEdit4("Color", particleProps.Color.ptr());
		ImGui::ColorEdit4("End Color", particleProps.endColor.ptr());
		ImGui::Checkbox("Particle Texture\t", &particleProps.texture);
		ImGui::NewLine();
		ImGui::Checkbox("Active\t", &isActive);

		if (ImGui::Button("Save Emitter State"))
		{
			SaveEmitterStateToFile("emitter_state.txt");
		}

		if (ImGui::Button("Load Emitter State"))
		{
			LoadEmitterStateFromFile("emitter_state.txt");
		}
	}
	//RefreshParticleText();

	ImGui::NewLine();
	ImGui::Separator();
	ImGui::NewLine();
}

void CEmitter::SaveEmitterStateToFile(const char* filename)
{
	std::ofstream outputFile(filename);

	if (outputFile.is_open())
	{
		SaveEmitterState(outputFile);
		outputFile.close();
	}
}

void CEmitter::LoadEmitterStateFromFile(const char* filename)
{
	std::ifstream inputFile(filename);

	if (inputFile.is_open())
	{
		LoadEmitterState(inputFile);
		inputFile.close();
	}
}

void CEmitter::RefreshParticleText()
{
	particleSystem.textID = textureID;
}

void CEmitter::SaveEmitterState(std::ofstream& outputFile)
{
	outputFile << "    BeginEmitter\n";
	outputFile << "      Position " << particleProps.pos.x << " " << particleProps.pos.y << " " << particleProps.pos.z << "\n";
	outputFile << "      BeginScale " << particleProps.beginScale.x << " " << particleProps.beginScale.y << " " << particleProps.beginScale.z << "\n";
	outputFile << "      EndScale " << particleProps.endScale.x << " " << particleProps.endScale.y << " " << particleProps.endScale.z << "\n";
	outputFile << "      Speed " << particleProps.speed.x << " " << particleProps.speed.y << " " << particleProps.speed.z << "\n";
	outputFile << "      SpeedVariation " << particleProps.speedVariation.x << " " << particleProps.speedVariation.y << " " << particleProps.speedVariation.z << "\n";
	outputFile << "      LifeTime " << particleProps.LifeTime << "\n";
	outputFile << "    EndEmitter\n";
}

void CEmitter::LoadEmitterState(std::ifstream& inputFile)
{
	std::string line;
	std::string propertyName;

	while (std::getline(inputFile, line))
	{
		// Parse each line to identify properties and load their values
		std::istringstream iss(line);
		iss >> propertyName;

		if (propertyName == "EndEmitter")
		{
			break; // End of emitter data
		}
		else if (propertyName == "Position")
		{
			iss >> particleProps.pos.x >> particleProps.pos.y >> particleProps.pos.z;
		}
		else if (propertyName == "BeginScale")
		{
			iss >> particleProps.beginScale.x >> particleProps.beginScale.y >> particleProps.beginScale.z;
		}
		else if (propertyName == "EndScale")
		{
			iss >> particleProps.endScale.x >> particleProps.endScale.y >> particleProps.endScale.z;
		}
		else if (propertyName == "Speed")
		{
			iss >> particleProps.speed.x >> particleProps.speed.y >> particleProps.speed.z;
		}
		else if (propertyName == "SpeedVariation")
		{
			iss >> particleProps.speedVariation.x >> particleProps.speedVariation.y >> particleProps.speedVariation.z;
		}
		else if (propertyName == "LifeTime")
		{
			iss >> particleProps.LifeTime;
		}
		// Add more properties as needed
	}
}