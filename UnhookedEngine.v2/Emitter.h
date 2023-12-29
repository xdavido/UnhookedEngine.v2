#pragma once
#include "Globals.h"
#include "Component.h"
#include "ImGui/imgui.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "ParticleSystem.h"

class GameObject;
class Component;

class CEmitter : public Component
{
public:

	CEmitter();
	CEmitter(GameObject* owner);
	~CEmitter();

	void Update();

	void RenderParticles();

	void PrintInspector();

	void RefreshParticleText();

	bool isActive;

	bool PrintText = true;

	uint textureID = 0;

	ParticleProps particleProps;
	ParticleSystem particleSystem;
};