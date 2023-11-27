#include "ComponentTransform.h"
#include "Globals.h"
#include "GameObject.h"

ComponentTransform::ComponentTransform(bool enabled) : Component(mOwner) {
	type = ComponentType::TRANSFORM;
	
	resetMatrix();
}

ComponentTransform::~ComponentTransform()
{
}



float4x4 ComponentTransform::getGlobalMatrix()
{
	if (mOwner->GetParent() == nullptr) return getLocalMatrix();

	return  matrix * mOwner->GetParent()->transform->getGlobalMatrix();
}

float4x4 ComponentTransform::getLocalMatrix()
{
	return matrix;
}

void ComponentTransform::resetMatrix()
{
	matrix = { 1, 0, 0, 0,
			   0, 1, 0, 0,
			   0, 0, 1, 0,
				0, 0, 0, 1 };

	position = { 0, 0, 0 };
	rotation = { 0, 0, 0 };
	scale = { 1, 1, 1 };
}

float3 ComponentTransform::getPosition(bool globalPosition)
{
	if (!globalPosition)return float3(position);

	float4x4 m = getGlobalMatrix();
	return m.Row3(3);
}

void ComponentTransform::setPosition(float3 pos)
{
	position = pos;
	calculateMatrix();
}

float3 ComponentTransform::getRotation()
{
	return rotation;
}

void ComponentTransform::setRotation(float3 rot)
{
	rotation = rot;
	calculateMatrix();
}

float3 ComponentTransform::getScale()
{
	return scale;
}

void ComponentTransform::setScale(float3 sca)
{
	scale = sca;
	calculateMatrix();
}



void ComponentTransform::calculateMatrix()
{

	float rx = rotation.x * DEGTORAD;
	float ry = rotation.y * DEGTORAD;
	float rz = rotation.z * DEGTORAD;

	Quat q;
	q = Quat::FromEulerXYZ(rx, ry, rz);
	matrix = float4x4::FromTRS(position, q, scale).Transposed();


}

void ComponentTransform:: PrintInspector() {

	if (ImGui::CollapsingHeader("Transform"))
	{
		ImGui::Text("X\t\t Y\t\t Z");
		ImGui::InputFloat3("Position", position.ptr());

		ImGui::Text("X\t\t Y\t\t Z");
		ImGui::InputFloat3("Rotation", rotation.ptr());

		ImGui::Text("X\t\t Y\t\t Z");
		ImGui::InputFloat3("Scale", scale.ptr());
	}


	calculateMatrix();


}