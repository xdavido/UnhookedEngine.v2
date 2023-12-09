#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "MathGeoLib/include/Math/Quat.h"
#include "MathGeoLib/include/Math/float3.h"
#include "ComponentTransform.h"
#include "MathGeoLib/include/Geometry/Frustum.h"
#include "ComponentCamera.h"
#include "ModuleRenderer3D.h"
#include "ModuleInput.h"


ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	/*X = float3(1.0f, 0.0f, 0.0f);
	Y = float3(0.0f, 1.0f, 0.0f);
	Z = float3(0.0f, 0.0f, 1.0f);

	Position = float3(0.0f, 10.0f, 5.0f);
	Reference = float3(0.0f, 0.0f, 0.0f);
	ViewMatrix = IdentityMatrix;*/

	//CalculateViewMatrix();
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;



	ComponentCamera* cam = new ComponentCamera(SceneCamera);
	sceneCam = cam;
	sceneCam->FrustumCam.pos = float3(0, 2, -10);
	

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	delete sceneCam;
	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!

	//float3 newPos(0, 0, 0);
	float speed = 3.0f * dt;
	int wheel = -App->input->GetMouseZ();

	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed = 8.0f * 2 * dt;

	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) sceneCam->FrustumCam.pos.y += speed;
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) sceneCam->FrustumCam.pos.y -= speed;

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) sceneCam->FrustumCam.pos += sceneCam->FrustumCam.front * speed;
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) sceneCam->FrustumCam.pos -= sceneCam->FrustumCam.front * speed;


	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) sceneCam->FrustumCam.pos -= sceneCam->FrustumCam.WorldRight() * speed;
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) sceneCam->FrustumCam.pos += sceneCam->FrustumCam.WorldRight() * speed;

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) RotationAroundCamera();

	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		if (App->hierarchy->objSelected != nullptr) {
			float3 target = App->hierarchy->objSelected->transform->position;
			sceneCam->LookAt(target);

			float TargetDist = sceneCam->FrustumCam.pos.Distance(target);

			RotationAroundCamera();

			sceneCam->FrustumCam.pos = target + (sceneCam->FrustumCam.front * -TargetDist);
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		if (App->hierarchy->objSelected != nullptr) {
			float3 target = App->hierarchy->objSelected->transform->position;

			sceneCam->LookAt(target);
		}
	}

	if (wheel != 0) sceneCam->FrustumCam.pos += sceneCam->FrustumCam.front * 10 * -wheel;

	//newPos -= App->input->GetMouseZ() * Z;

	//Position += newPos;
	//Reference += newPos;

	//OrbitSelectedObject(dt);

	//LookAt(Reference);

	//// Recalculate matrix -------------
	//CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const float3& Position, const float3& Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = (Position - Reference).Normalized();
	X = (float3(0.0f, 1.0f, 0.0f).Cross(Z)).Normalized();
	Y = Z.Cross(X);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const float3& Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return ViewMatrix.M;
}

void ModuleCamera3D::FocusCameraToSelectedObject()
{

	//float3 focusObjectPosition;
	////Get the GameObject selected in hierarchy
	//if (App->hierarchy->objSelected != nullptr)
	//{
	//	focusObjectPosition = App->hierarchy->objSelected->transform->getPosition();
	//	LookAt(focusObjectPosition);//esta en ComponentCamera
	//}

}

void ModuleCamera3D::OrbitSelectedObject(float dt)
{
	float3 pivot = float3(0, 0, 0);
	GameObject* gameObject = App->hierarchy->objSelected;

	float3 posGO = { 0, 0, 0 };

	if (gameObject != nullptr)posGO = gameObject->transform->getPosition();

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();
		float Sensitivity = 0.5f * dt;

		if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
		{

			if (gameObject != nullptr)
			{
				pivot = float3(posGO.x, posGO.y, posGO.z);
			}
			else
			{
				return;
			}
		}
		else
		{
			return;
		}

		Position -= pivot;

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			X = RotateVector(X, DeltaX, float3(0.0f, 1.0f, 0.0f));
			Y = RotateVector(Y, DeltaX, float3(0.0f, 1.0f, 0.0f));
			Z = RotateVector(Z, DeltaX, float3(0.0f, 1.0f, 0.0f));
		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			Y = RotateVector(Y, DeltaY, X);
			Z = RotateVector(Z, DeltaY, X);

			if (Y.y < 0.0f)
			{
				Z = float3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = Cross(Z, X);

			}
		}
		Position = pivot + Z * Length(Position);
		Reference = pivot;

	}

}

void ModuleCamera3D::RotationAroundCamera()
{
	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();

	Quat dir;
	sceneCam->FrustumCam.WorldMatrix().Decompose(float3(), dir, float3());

	if (dy != 0) {
		float DeltaY = (float)dy * mouseSens;

		Quat Y = Quat::identity;
		Y.SetFromAxisAngle(float3(1.0f, 0.0f, 0.0f), DeltaY * DEGTORAD);

		dir = dir * Y;
	}

	if (dx != 0) {
		float DeltaX = (float)dx * mouseSens;

		Quat X = Quat::identity;
		X.SetFromAxisAngle(float3(0.0f, 1.0f, 0.0f), DeltaX * DEGTORAD);

		dir = X * dir;
	}

	float4x4 matrix = sceneCam->FrustumCam.WorldMatrix();
	matrix.SetRotatePart(dir.Normalized());
	sceneCam->FrustumCam.SetWorldMatrix(matrix.Float3x4Part());
}

float3 ModuleCamera3D::RotateVector(const float3& u, float angle, const float3& v)
{
	// Crear un cuaterni�n de rotaci�n a partir del eje y el �ngulo
	Quat rotationQuat = Quat::RotateAxisAngle(v, angle);

	// Aplicar la rotaci�n al vector
	return rotationQuat * u;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	//todo: USE MATHGEOLIB here BEFORE 1st delivery! (TIP: Use MathGeoLib/Geometry/Frustum.h, view and projection matrices are managed internally.)
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -(X.Dot(Position)), -(Y.Dot(Position)), -(Z.Dot(Position)), 1.0f);

	//something like this
	// frustum.SetFromViewProj(ViewMatrix, App->renderer3D->GetProjectionMatrix());
}