#include "Application.h"
#include "ComponentCamera.h"
#include "GameObject.h"
#include "ComponentTransform.h"

ComponentCamera::ComponentCamera():Component(nullptr)
{
	
	type = ComponentType::CAMERA;
	 mOwner= nullptr;

	frustum.type = PerspectiveFrustum;
	frustum.nearPlaneDistance = nearDistance;
	frustum.farPlaneDistance = farDistance;
	frustum.front = float3::unitZ;
	frustum.up = float3::unitY;
	frustum.verticalFov = cameraFOV * DEGTORAD;
	frustum.horizontalFov = 2.0f * atanf(tanf(frustum.verticalFov / 2.0f) * 1.7f); 
	frustum.pos = float3(0, 0, 0);
}

ComponentCamera::~ComponentCamera()
{
}

void ComponentCamera::PrintInspector()
{
	//Roger tu turno
	const char* listType[]{ "Perspective", "Orthographic" };

	if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		if (isMainCamera)
		{
			ImGui::SameLine();
			ImGui::Text(" ( Main Camera )");
		}

		ImGui::Text("");

		ImGui::Text("Camera type: ");
		ImGui::SameLine();
		if (ImGui::Combo("##CameraType", &typeCameraSelected, listType, IM_ARRAYSIZE(listType)))
		{
			if (typeCameraSelected == 0)
				frustum.type = PerspectiveFrustum;

			if (typeCameraSelected == 1)
				frustum.type = OrthographicFrustum;

		}

		ImGui::Text("");

		ImGui::Text("FOV\t\t  ");
		ImGui::SameLine();
		if (ImGui::SliderInt("##FOVert", &cameraFOV, 10, 170))
		{
			frustum.verticalFov = cameraFOV * DEGTORAD;
			frustum.horizontalFov = 2.0f * atanf(tanf(frustum.verticalFov / 2.0f) * 1.7f);
		}

		ImGui::Text("");

		ImGui::Text("Near Distance\t");
		ImGui::SameLine();
		if (ImGui::InputFloat("##nearDistance", &nearDistance))
		{
			if (nearDistance >= farDistance)
			{
				farDistance = nearDistance + 1;
				frustum.farPlaneDistance = farDistance;
			}

			frustum.nearPlaneDistance = nearDistance;
		}

		ImGui::Text("Distance\t ");
		ImGui::SameLine();
		if (ImGui::SliderFloat("##farDistance", &farDistance, 50, 1000))
		{
			frustum.farPlaneDistance = farDistance;
		}

		ImGui::Text("");

		ImGui::Text("");
		ImGui::SameLine((ImGui::GetWindowWidth() / 2) - 75);
		if (ImGui::Button("Set as main camera", ImVec2(150, 25)))
		{
			isMainCamera = true;

			
		}
	}
}

void ComponentCamera::Update()
{
	if (mOwner == nullptr) return;

	frustum.pos = mOwner->transform->getPosition();
	float4x4 m = mOwner->transform->getGlobalMatrix();
	frustum.up = m.RotatePart().Col(1).Normalized();
	frustum.front = m.RotatePart().Col(2).Normalized();
}

void ComponentCamera::Look(const float3& Position, const float3& Reference)
{
	frustum.pos = Position;
	reference = Reference;

	LookAt(reference);
}

void ComponentCamera::LookAt(const float3& Spot)
{
	reference = Spot;
	frustum.front = (Spot - frustum.pos).Normalized();
	float3 X = float3(0, 1, 0).Cross(frustum.front).Normalized();
	frustum.up = frustum.front.Cross(X);
}

void ComponentCamera::Move(const float3& Movement)
{
	frustum.pos += Movement;
}
void ComponentCamera::SetAspectRatio(float aspectRatio)
{
	frustum.verticalFov = cameraFOV * DEGTORAD;
	frustum.horizontalFov = 2.0f * atanf(tanf(frustum.verticalFov / 2.0f) * aspectRatio);
}

float* ComponentCamera::GetViewMatrix()
{
	viewMatrix = frustum.ViewMatrix();
	viewMatrix.Transpose();
	return viewMatrix.ptr();
}

float* ComponentCamera::GetProjetionMatrix()
{
	projectionMatrix = frustum.ProjectionMatrix();
	projectionMatrix.Transpose();
	return projectionMatrix.ptr();
}