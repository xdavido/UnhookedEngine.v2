#pragma once

#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "ImGui/imgui.h"
#include <vector>
#include <string>

class ComponentTransform : public Component
{
public:
	ComponentTransform(bool enabled = true);
	~ComponentTransform();



	float4x4 getGlobalMatrix();
	float4x4 getLocalMatrix();

	
	void resetMatrix();

	
	float3 getPosition(bool globalPosition = false);
	void setPosition(float3 pos);
	float3 getRotation();
	void setRotation(float3 rot);
	float3 getScale();
	void setScale(float3 sca);

	void PrintInspector();
	void ApplyTransformToGameObject();
	void UpdateMatrixFromInspector();

	float4x4 matrix;
	float3 position;
	float3 rotation;
	float3 scale;

private:


	
};