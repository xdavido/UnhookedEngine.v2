#pragma once
#include "Module.h"
#include "Globals.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "MathGeoLib/include/Geometry/Frustum.h"

#include "glmath.h"

class ComponentCamera;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	ComponentCamera* sceneCam;

	bool Start();
	update_status Update(float dt);
	bool CleanUp();
	float mouseSens = 0.50f;

	void Look(const float3& Position, const float3& Reference, bool RotateAroundReference = false);
	void Move(const float3& Movement);
	float* GetViewMatrix();
	void FocusCameraToSelectedObject();
	void RotationAroundCamera();
	void OrbitSelectedObject(float dt);
	float3 RotateVector(const float3& u, float angle, const float3& v);

private:

	void CalculateViewMatrix();

public:

	Frustum frustum;

	float3 X, Y, Z, Position, Reference;

private:

	mat4x4 ViewMatrix;
	bool click = false;

};