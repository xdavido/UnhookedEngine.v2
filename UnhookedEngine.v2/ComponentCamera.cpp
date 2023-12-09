#include "Application.h"
#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "GameObject.h"
#include "ModuleRenderer3D.h"



ComponentCamera::ComponentCamera() : Component(nullptr)
{
	type = ComponentType::CAMERA;
	mOwner = nullptr;
	printCount = 0;
	SetCam();
	GenBuffer();
}

ComponentCamera::ComponentCamera(GameObject* owner) : Component(owner)
{
	type = ComponentType::CAMERA;
	this->mOwner = owner;
	printCount = 0;
	SetCam();
	GenBuffer();
}

ComponentCamera::~ComponentCamera()
{
	glDeleteFramebuffers(1, &cameraBuffer);
	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteFramebuffers(1, &renderObjBuffer);
}

void ComponentCamera::Update()
{
	if (mOwner == nullptr) return;

	FrustumCam.pos = mOwner->transform->getPosition();
	float4x4 m = mOwner->transform->getGlobalMatrix();
	FrustumCam.up = m.RotatePart().Col(1).Normalized();
	FrustumCam.front = m.RotatePart().Col(2).Normalized();
}

void ComponentCamera::SetCam()
{
	FrustumCam.type = FrustumType::PerspectiveFrustum;
	FrustumCam.nearPlaneDistance = nearDistance;
	FrustumCam.farPlaneDistance = farDistance;
	FrustumCam.front = float3::unitZ;
	FrustumCam.up = float3::unitY;

	FrustumCam.verticalFov = FOV * DEGTORAD;
	FrustumCam.horizontalFov = 2.0f * atanf(tanf(FrustumCam.verticalFov / 2.0f) * 1.7f);

	FrustumCam.pos = float3(0, 0, 0);
}

void ComponentCamera::GenBuffer()
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
		LOG("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
bool ComponentCamera::FrustumCulling(Mesh* mesh)
{
	float3 boxPoints[8];
	Plane frustumPlanes[6];

	mesh->GlobalAABB.GetCornerPoints(boxPoints);
	FrustumCam.GetPlanes(frustumPlanes);

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
float* ComponentCamera::GetViewMatrix()
{

	viewMatrix = FrustumCam.ViewMatrix();

	viewMatrix.Transpose();

	return viewMatrix.ptr();
}

float* ComponentCamera::GetProjectionMatrix()
{

	projectionMatrix = FrustumCam.ProjectionMatrix();

	projectionMatrix.Transpose();

	return projectionMatrix.ptr();
}

void ComponentCamera::LookAt(const float3& target)
{
	FrustumCam.front = (target - FrustumCam.pos).Normalized();
	float3 X = float3(0, 1, 0).Cross(FrustumCam.front).Normalized();
	FrustumCam.up = FrustumCam.front.Cross(X);
}

void ComponentCamera::TransformCam()
{
	//if is the scene cam it will move with input controls (ModuleCamera3D)
	if (mOwner == nullptr) return;

	//if not, move with gameObj transform
	FrustumCam.pos = mOwner->transform->position;

	//owner's global transform matrix
	float4x4 matrix = mOwner->transform->getGlobalMatrix();

	//Column 0 -> eix X -> worldRight || Column 1 -> eix Y -> up || Column 2  -> eix Z -> front || Column 3 -> pos
	FrustumCam.up = matrix.RotatePart().Col(1).Normalized();
	FrustumCam.front = matrix.RotatePart().Col(2).Normalized();
}

void ComponentCamera::PrintInspector()
{
	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::Text("");
		ImGui::Text("Rendered objects: %d", printCount);
		ImGui::Text("");

		if (ImGui::SliderInt("FOV", &FOV, 5, 180)) {
			FrustumCam.verticalFov = FOV * DEGTORAD;
			FrustumCam.horizontalFov = 2.0f * atanf(tanf(FrustumCam.verticalFov / 2.0f) * 1.7f);
		}
		if (ImGui::Button("Reset FOV")) {
			FOV = 60.0f;

			FrustumCam.verticalFov = FOV * DEGTORAD;
			FrustumCam.horizontalFov = 2.0f * atanf(tanf(FrustumCam.verticalFov / 2.0f) * 1.7f);
		}


		ImGui::SliderFloat("Near Distance", &FrustumCam.nearPlaneDistance, 0.1f, FrustumCam.farPlaneDistance);
		if (ImGui::Button("Reset Near Distance")) {
			FrustumCam.nearPlaneDistance = 0.1f;
		}

		ImGui::InputFloat("Far Distance", &FrustumCam.farPlaneDistance);
		if (ImGui::Button("Reset Far Distance")) {
			FrustumCam.farPlaneDistance = 500.f;
		}
		ImGui::Text("");
	}
}


bool ComponentCamera::ContainsAaBox(Mesh* refBox)
{
	float3 vCorner[8];
	int iTotalIn = 0;
	refBox->localAABB.GetCornerPoints(vCorner);

	Plane m_plane[6];
	FrustumCam.GetPlanes(m_plane);

	for (int p = 0; p < 6; ++p) {
		int iInCount = 8;
		int iPtIn = 1;
		for (int i = 0; i < 8; ++i) {
			
			if (m_plane[p].IsOnPositiveSide(vCorner[i])) {
				iPtIn = 0;
				--iInCount;
			}
		}
		
		if (iInCount == 0)
			return false;
		
		iTotalIn += iPtIn;
	}
	if (iTotalIn == 6)
		return true;
}