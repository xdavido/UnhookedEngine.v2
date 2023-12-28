#include "Application.h"
#include "ComponentCamera.h"
#include "GameObject.h"
#include "ComponentTransform.h"
#include "ModuleRenderer3D.h"

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

	CreateFrameBuffer();
}

ComponentCamera::~ComponentCamera()
{
	if (isMainCamera) App->renderer3D->SetMainCamera(nullptr);

	glDeleteFramebuffers(1, &cameraBuffer);
	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteFramebuffers(1, &renderObjBuffer);
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
		ImGui::LabelText("##Game mesh", "Rendering Meshes:");
		ImGui::SameLine();
		ImGui::Text("%d", App->assimpMeshes->renderedGameMeshes);

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
			App->renderer3D->SetMainCamera(this);

			
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

void ComponentCamera::CreateFrameBuffer()
{
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	
	glGenTextures(1, &cameraBuffer);
	glBindTexture(GL_TEXTURE_2D, cameraBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	float color[4] = { 0.1,0.1,0.1,0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cameraBuffer, 0);
	glGenRenderbuffers(1, &renderObjBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderObjBuffer);	
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCREEN_WIDTH, SCREEN_HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderObjBuffer);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		LOG( "ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	App->renderer3D->SetMainCamera(this);
}
bool ComponentCamera::FrustumCulling(Mesh* mesh)
{
	float3 boxPoints[8];
	Plane frustumPlanes[6];

	mesh->GlobalAABB.GetCornerPoints(boxPoints);
	frustum.GetPlanes(frustumPlanes);

	for (size_t i = 0; i < 6; i++)
	{
		int p = 0;

		
		for (size_t j = 0; j < 8; j++)
		{
			if (frustumPlanes[i].IsOnPositiveSide(boxPoints[j]))
				p++;
		}

		
		if (p == 8) {
			
			return false;
		}
	}

	
	return true;
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