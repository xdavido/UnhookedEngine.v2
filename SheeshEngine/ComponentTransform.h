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



	//Get transform matrix
	float4x4 getGlobalMatrix();
	float4x4 getLocalMatrix();

	//Put everything to 0
	void resetMatrix();

	//Getters / Setters
	float3 getPosition(bool globalPosition = false);
	void setPosition(float3 pos);
	float3 getRotation();
	void setRotation(float3 rot);
	float3 getScale();
	void setScale(float3 sca);

	void PrintInspector();


	

private:
	void calculateMatrix();

	float4x4 matrix;
	float3 position;
	float3 rotation;
	float3 scale;
};