#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "MathGeoLib/include/Math/Quat.h"
#include "MathGeoLib/include/Math/float3.h"
#include "ComponentTransform.h"
#include "ComponentCamera.h"
#include"GameWindow.h"
#include "SceneWindow.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Camera3D";


	camState = CamStates::NORMAL;

	X = float3(1.0f, 0.0f, 0.0f);
	Y = float3(0.0f, 1.0f, 0.0f);
	Z = float3(0.0f, 0.0f, 1.0f);

	Position = float3(0.0f, 1.0f, 10.0f);
	Reference = float3(0.0f, 0.0f, 0.0f);
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	camera = new ComponentCamera();
	camera->frustum.pos = float3(-1, 2, -10);

	gameCamera = new GameObject(App->scene->root);
	gameCamera->name = "MainCamera";
	ComponentCamera* cc = new ComponentCamera();
	gameCamera->AddComponent(cc);
	
	gameCamera->transform->setPosition(float3(-1, 2, -10));


	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	delete camera;

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	// Implement a debug camera with keys and mouse
	// Now we can make this movememnt frame rate independant!
	float3 newPos(0, 0, 0);
	Quat direction = Quat::identity;

	//Speed
	float sensitivity = 25.f * dt;
	float speed = 8.0f * dt;
	if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
		speed *= 2;

	//Mouse scrolls
	int dx = -App->input->GetMouseXMotion();
	int dy = -App->input->GetMouseYMotion();
	int dw = -App->input->GetMouseZ(); //wheel


	//Camera states
	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT) {
		camState = CamStates::FOCUSED;
	}
	else if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT) {
		camState = CamStates::FLYING;
	}
	else {
		camState = CamStates::NORMAL;
	}

	//Focus on object
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
		//Look at object
		camera->LookAt(SelectedObjectPos());
	}

	//Camera states behaviour
	switch (camState) {

		//WASDQE + mouse "fps like" movement
	case CamStates::FLYING:
	{
		//WASDQE movement
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) camera->frustum.pos += camera->frustum.front * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) camera->frustum.pos -= camera->frustum.front * speed;

		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) camera->frustum.pos -= camera->frustum.WorldRight() * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) camera->frustum.pos += camera->frustum.WorldRight() * speed;

		//Move vertically independently of camera rotation
		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) camera->frustum.pos.y += speed;
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) camera->frustum.pos.y -= speed;

		//Rotate
		MouseRotation(dx, dy, sensitivity);
	}
	break;

	//Static cam, move arround reference
	case CamStates::FOCUSED:
	{
		camera->LookAt(SelectedObjectPos());

		//Calculate distance
		float len = float3(camera->reference - camera->frustum.pos).Length();

		//Rotate
		MouseRotation(dx, dy, sensitivity);

		//Go back to distance
		camera->frustum.pos = camera->reference + (camera->frustum.front * -len);
	}
	break;

	//(Mouse-wheel-click) move and (mouse-wheel-scroll) zoom
	case CamStates::NORMAL:
	{
		//Mouse Wheel click
		if (App->input->GetMouseButton(SDL_BUTTON_MIDDLE) == KEY_REPEAT) {
			camera->frustum.pos += camera->frustum.WorldRight() * (speed * dx / 2);
			camera->frustum.pos -= camera->frustum.up * (speed * dy / 2);
		}
		//Mouse wheel scroll
		if (dw != 0) {
			camera->frustum.pos += camera->frustum.front * speed * -dw;
		}
	}
	break;
	}

	return UPDATE_CONTINUE;


}

void ModuleCamera3D::MouseRotation(float dx, float dy, float sensitivity)
{
	//Rotation
	Quat dir;
	camera->frustum.WorldMatrix().Decompose(float3(), dir, float3());

	//Mouse look direction
	if (dx != 0)
	{
		float DeltaX = (float)dx * sensitivity;
		Quat X = Quat::identity;
		X.SetFromAxisAngle(float3(0.0f, 1.0f, 0.0f), DeltaX * DEGTORAD);
		dir = X * dir;
	}

	if (dy != 0)
	{
		float DeltaY = (float)dy * sensitivity;
		Quat Y = Quat::identity;
		Y.SetFromAxisAngle(float3(1.0f, 0.0f, 0.0f), DeltaY * DEGTORAD);
		dir = dir * Y;
	}

	//Set direction
	float4x4 rm = camera->frustum.WorldMatrix();
	rm.SetRotatePart(dir.Normalized());
	camera->frustum.SetWorldMatrix(rm.Float3x4Part());
}

float3 ModuleCamera3D::SelectedObjectPos()
{
	float3 SelectedObject = { 0,0,0 };
	if (App->hierarchy->objSelected != nullptr) {
		SelectedObject = App->hierarchy->objSelected->transform->getPosition(true);
	}
	return SelectedObject;
}



bool ModuleCamera3D::SaveConfig(JsonParser& node) const
{

	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "X.x", X.x);
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "X.y", X.y);
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "X.z", X.z);

	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Y.x", camera->frustum.up.x);
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Y.y", camera->frustum.up.y);
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Y.z", camera->frustum.up.z);

	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Z.x", camera->frustum.front.x);
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Z.y", camera->frustum.front.y);
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Z.z", camera->frustum.front.z);

	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Reference.x", camera->reference.x);
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Reference.y", camera->reference.y);
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Reference.z", camera->reference.z);

	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Position.x", camera->frustum.pos.x);
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Position.y", camera->frustum.pos.y);
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "Position.z", camera->frustum.pos.z);


	return true;
}

bool ModuleCamera3D::LoadConfig(JsonParser& node)
{

	//X.x = node.JsonValToNumber("X.x");
	//X.y = node.JsonValToNumber("X.y");
	//X.z = node.JsonValToNumber("X.z");

	camera->frustum.up.x = node.JsonValToNumber("Y.x");
	camera->frustum.up.y = node.JsonValToNumber("Y.y");
	camera->frustum.up.z = node.JsonValToNumber("Y.z");

	camera->frustum.front.x = node.JsonValToNumber("Z.x");
	camera->frustum.front.y = node.JsonValToNumber("Z.y");
	camera->frustum.front.z = node.JsonValToNumber("Z.z");

	camera->frustum.pos.x = node.JsonValToNumber("Position.x");
	camera->frustum.pos.y = node.JsonValToNumber("Position.y");
	camera->frustum.pos.z = node.JsonValToNumber("Position.z");

	camera->reference.x =node.JsonValToNumber("Reference.x");
	camera->reference.y =node.JsonValToNumber("Reference.y");
	camera->reference.z =node.JsonValToNumber("Reference.z");

	return true;
}