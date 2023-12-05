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
	UpdateMatrixFromInspector();
}

float3 ComponentTransform::getRotation()
{
	return rotation;
}

void ComponentTransform::setRotation(float3 rot)
{
	rotation = rot;
	UpdateMatrixFromInspector();
}

float3 ComponentTransform::getScale()
{
	return scale;
}

void ComponentTransform::setScale(float3 sca)
{
	scale = sca;
	UpdateMatrixFromInspector();
}

void ComponentTransform::UpdateMatrixFromInspector()
{
	float rx = rotation.x * DEGTORAD;
	float ry = rotation.y * DEGTORAD;
	float rz = rotation.z * DEGTORAD;

	Quat q;
	q = Quat::FromEulerXYZ(rx, ry, rz);
	matrix = float4x4::FromTRS(position, q, scale).Transposed();
	//ApplyTransformToGameObject();
}

void ComponentTransform::ApplyTransformToGameObject()
{
	// Ensure the owner (GameObject) exists
	if (mOwner != nullptr)
	{
		// Apply the matrix to the GameObject's transform
		mOwner->transform->matrix = getGlobalMatrix();
	}
}

void ComponentTransform::PrintInspector()
{
	if (ImGui::CollapsingHeader("Transform"))
	{
		ImGui::Text("X\t\t Y\t\t Z");
		if (ImGui::InputFloat3("Position", position.ptr()))
		{
			// Position changed, trigger matrix update
			UpdateMatrixFromInspector();
		}

		ImGui::Text("X\t\t Y\t\t Z");
		if (ImGui::InputFloat3("Rotation", rotation.ptr()))
		{
			// Rotation changed, trigger matrix update
			UpdateMatrixFromInspector();
		}

		ImGui::Text("X\t\t Y\t\t Z");
		if (ImGui::InputFloat3("Scale", scale.ptr()))
		{
			// Scale changed, trigger matrix update
			UpdateMatrixFromInspector();
		}

		if (ImGui::Button("Update Matrix"))
		{
			// Manual button to update the matrix
			UpdateMatrixFromInspector();
		}
	}
}


//float4x4 ComponentTransform::GetTransformMatrix()
//{
//	if (mOwner->mParent == nullptr) {
//		return matrix;
//	}
//	else {
//		return matrix * mOwner->mParent->transform->GetTransformMatrix();
//	}
//}