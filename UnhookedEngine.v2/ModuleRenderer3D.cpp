#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "ComponentCamera.h"
#include "ComponentTransform.h"
#include "Primitive.h"
#include "ModuleAssimpMeshes.h"
#include "ModuleEditor.h"
#include "Glew/include/glew.h"

#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glu32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Glew/libx86/glew32.lib")

#ifdef _DEBUG
#pragma comment (lib, "MathGeoLib/libx86/Debug2/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#else
#pragma comment (lib, "MathGeoLib/libx86/Release2/MathGeoLib.lib") /* link Microsoft OpenGL lib   */
#endif // _DEBUG

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;

	//Create context
	context = SDL_GL_CreateContext(App->window->window);

	if (context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	//Initialize Glew
	glewInit();

	if (ret == true)
	{
		//Use Vsync
		if (VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		GLfloat LightModelAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(1.20f, 1.0f, 1.0f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, -2.5f);
		lights[0].Init();

		GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		SetDepthTestAttribute(depthTestAttribute);
		SetCullFaceAttribute(cullFaceAttribute);
		lights[0].Active(true);
		SetLightingAttribute(lightingAttribute);
		SetColorMaterialAttribute(colorMaterialAttribute);
		SetCullFaceAttribute(cullFaceAttribute);
		SetTexture2DAttribute(texture2DAttribute);
		SetBlendAttribute(blendAttribute);
		SetAlphaTestAttribute(alphaTestAttribute);
		SetLineSmoothAttribute(lineSmoothAttribute);
		SetPointSmoothAttribute(pointSmoothAttribute);
		SetPolygonSmoothAttribute(polygonSmoothAttribute);
	}

	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	Grid.axis = true;

	return ret;
}

bool ModuleRenderer3D::Start()
{
	LOG("Render Start");
	bool ret = true;

	GameCamera = new GameObject(App->scene->root);

	GameCamera->name = "Main Camera";

	ComponentCamera* cam = new ComponentCamera(GameCamera);
	mainCam = cam;

	GameCamera->mComponents.push_back(cam);
	GameCamera->transform->position = float3(0, 2, -10);
	GameCamera->transform->UpdateMatrixFromInspector();

	return true;
}


// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(App->camera->sceneCam->GetProjectionMatrix());

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->sceneCam->GetViewMatrix());

	glBindFramebuffer(GL_FRAMEBUFFER, App->camera->sceneCam->frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	lights[0].SetPos(0, 0, 0);

	for (uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	App->assimpMeshes->RenderScene();

	if (mainCam != nullptr) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf(mainCam->GetProjectionMatrix());

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf(mainCam->GetViewMatrix());

		glBindFramebuffer(GL_FRAMEBUFFER, mainCam->frameBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		App->assimpMeshes->RenderGameWindow();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (App->editor->DrawEditor() == UPDATE_STOP)
	{
		return UPDATE_STOP;
	}

	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

void ModuleRenderer3D::DirectModeTriangleDrawing()
{
	//Draw test here
	glLineWidth(2.0f);
	glBegin(GL_TRIANGLES);

	//Mirando hacia Z negativo https://technology.cpm.org/general/3dgraph/ + Viendo como estan orientado los ejes en nuestro engine
	//Si queremos que la normal de las cara sea visible hacia nosotros: vertices en sentido de las agujas del reloj
	//Si queremos que la normal de la cara sea visible hacia delante de nosotros: verticas en sentido contrario a las agujas del reloj
	//Para los planos en otra direccion, funciona igual

	glVertex3d(3, 3, 3); glVertex3d(4, 4, 3); glVertex3d(4, 3, 3);
	glVertex3d(3, 3, 3); glVertex3d(3, 4, 3); glVertex3d(4, 4, 3);

	glVertex3d(3, 3, 3); glVertex3d(3, 4, 4); glVertex3d(3, 4, 3);
	glVertex3d(3, 3, 4); glVertex3d(3, 4, 4); glVertex3d(3, 3, 3);

	glVertex3d(4, 3, 4); glVertex3d(4, 4, 3); glVertex3d(4, 4, 4);
	glVertex3d(4, 3, 4); glVertex3d(4, 3, 3); glVertex3d(4, 4, 3);

	glVertex3d(3, 3, 4); glVertex3d(4, 3, 4); glVertex3d(4, 4, 4);
	glVertex3d(3, 3, 4); glVertex3d(4, 4, 4); glVertex3d(3, 4, 4);

	glVertex3d(3, 3, 3); glVertex3d(4, 3, 3); glVertex3d(3, 3, 4);
	glVertex3d(4, 3, 3); glVertex3d(4, 3, 4); glVertex3d(3, 3, 4);

	glVertex3d(3, 4, 3); glVertex3d(3, 4, 4); glVertex3d(4, 4, 3);
	glVertex3d(4, 4, 4); glVertex3d(4, 4, 3); glVertex3d(3, 4, 4);

	glEnd();
	glLineWidth(1.0f);
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");
	delete GameCamera;
	//delete mainCam;

	SDL_GL_DeleteContext(context);
	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// Setter functions for renderer attributes with logging
void ModuleRenderer3D::SetDepthTestAttribute(bool enable) {
	depthTestAttribute = enable;

	depthTestAttribute ? glDisable(GL_DEPTH_TEST) : glEnable(GL_DEPTH_TEST);

	std::string aux = "Renderer depthTestAttribute updated to: " + std::string(enable ? "false" : "true");
	LOG(aux.c_str());
}

void ModuleRenderer3D::SetCullFaceAttribute(bool enable) {
	cullFaceAttribute = enable;

	cullFaceAttribute ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);

	std::string aux = "Renderer cullFaceAttribute updated to: " + std::string(enable ? "true" : "false");
	LOG(aux.c_str());
}

void ModuleRenderer3D::SetLightingAttribute(bool enable) {
	lightingAttribute = enable;

	lightingAttribute ? glDisable(GL_LIGHTING) : glEnable(GL_LIGHTING);

	std::string aux = "Renderer lightingAttribute updated to: " + std::string(enable ? "false" : "true");
	LOG(aux.c_str());
}

void ModuleRenderer3D::SetColorMaterialAttribute(bool enable) {
	colorMaterialAttribute = enable;

	colorMaterialAttribute ? glEnable(GL_COLOR_MATERIAL) : glDisable(GL_COLOR_MATERIAL);

	std::string aux = "Renderer coloMaterialAttribute updated to: " + std::string(enable ? "true" : "false");
	LOG(aux.c_str());
}

void ModuleRenderer3D::SetTexture2DAttribute(bool enable) {
	texture2DAttribute = enable;

	texture2DAttribute ? glEnable(GL_TEXTURE_2D) : glDisable(GL_TEXTURE_2D);

	std::string aux = "Renderer texture2DAttribute updated to: " + std::string(texture2DAttribute ? "true" : "false");
	LOG(aux.c_str());
}

void ModuleRenderer3D::SetBlendAttribute(bool enable) {
	blendAttribute = enable;

	blendAttribute ? glEnable(GL_BLEND) : glDisable(GL_BLEND);

	std::string aux = "Renderer blendAttribute updated to: " + std::string(blendAttribute ? "true" : "false");
	LOG(aux.c_str());
}

void ModuleRenderer3D::SetAlphaTestAttribute(bool enable) {
	alphaTestAttribute = enable;

	alphaTestAttribute ? glEnable(GL_ALPHA_TEST) : glDisable(GL_ALPHA_TEST);

	std::string aux = "Renderer alphaTestAttribute updated to: " + std::string(alphaTestAttribute ? "true" : "false");
	LOG(aux.c_str());
}

void ModuleRenderer3D::SetLineSmoothAttribute(bool enable) {
	lineSmoothAttribute = enable;

	lineSmoothAttribute ? glEnable(GL_LINE_SMOOTH) : glDisable(GL_LINE_SMOOTH);

	std::string aux = "Renderer lineSmoothAttribute updated to: " + std::string(lineSmoothAttribute ? "true" : "false");
	LOG(aux.c_str());
}

void ModuleRenderer3D::SetPointSmoothAttribute(bool enable) {
	pointSmoothAttribute = enable;

	pointSmoothAttribute ? glEnable(GL_POINT_SMOOTH) : glDisable(GL_POINT_SMOOTH);

	std::string aux = "Renderer pointSmoothAttribute updated to: " + std::string(pointSmoothAttribute ? "true" : "false");
	LOG(aux.c_str());
}

void ModuleRenderer3D::SetPolygonSmoothAttribute(bool enable) {
	polygonSmoothAttribute = enable;

	polygonSmoothAttribute ? glEnable(GL_POLYGON_SMOOTH) : glDisable(GL_POLYGON_SMOOTH);

	std::string aux = "Renderer polygonSmoothAttribute updated to: " + std::string(polygonSmoothAttribute ? "true" : "false");
	LOG(aux.c_str());
}

void ModuleRenderer3D::SetVsync(bool vsync) {

	this->vsync = vsync;
	if (vsync)
	{
		SDL_GL_SetSwapInterval(1);
	}
	else
	{
		SDL_GL_SetSwapInterval(0);
	}

	std::string aux = "Renderer VSYNC attribute updated to: " + std::string(vsync ? "true" : "false");
	LOG(aux.c_str());
}

void ModuleRenderer3D::SetWireframeMode(bool enable)
{
	wireframeMode = enable;

	std::string aux = "Wireframe Mode updated to: " + std::string(enable ? "true" : "false");
	LOG(aux.c_str());
}

void ModuleRenderer3D::DrawWithWireframe()
{
	wireframeMode ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

