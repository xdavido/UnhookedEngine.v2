#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include"ComponentCamera.h"

#include "glmath.h"

enum class CamStates {
	NORMAL,	//Nothing clicked, default camera movement
	FLYING,	//WASD and mouse "fps like" movement
	FOCUSED,//ALT clicked, mouse movement and rotation
};


class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look();
	void LookAt(const float3& Spot);
	void Move(const float3& Movement);
	float* GetViewMatrix();
	void FocusCameraToSelectedObject();
	void RotationAroundCamera(float dt);
	void OrbitSelectedObject(float dt);
	float3 RotateVector(const float3& u, float angle, const float3& v);

	bool SaveConfig(JsonParser& node) const;
	void MouseRotation(float dx, float dy, float sensitivity);
	float3 SelectedObjectPos();
	bool LoadConfig(JsonParser& node);
private:

	void CalculateViewMatrix();
	

public:

	
	float3 X, Y, Z, Position, Reference;
	ComponentCamera* cam;
	CamStates camState;

private:

	mat4x4 ViewMatrix;
	
};