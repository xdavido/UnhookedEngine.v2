#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "SDL\include\SDL_opengl.h"
#include"ComponentCamera.h"
#include <gl/GL.h>
#include <gl/GLU.h>

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
	name = "Renderer3D";

	mainGameCamera = nullptr;
	ProjectionMatrix.SetIdentity();
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

static const GLfloat g_vertex_buffer_data[] = {
	// Cara frontal
	-7.0f, -7.0f,  7.0f,  // Vértice 1
	 7.0f, -7.0f,  7.0f,  // Vértice 2
	 7.0f,  7.0f,  7.0f,  // Vértice 3
	-7.0f,  7.0f,  7.0f,  // Vértice 4

	// Cara trasera
	-7.0f, -7.0f, -7.0f,  // Vértice 5
	 7.0f, -7.0f, -7.0f,  // Vértice 6
	 7.0f,  7.0f, -7.0f,  // Vértice 7
	-7.0f,  7.0f, -7.0f,  // Vértice 8

	// Caras laterales
	-7.0f, -7.0f, -7.0f,  // Vértice 9
	-7.0f, -7.0f,  7.0f,  // Vértice 10
	-7.0f,  7.0f,  7.0f,  // Vértice 11
	-7.0f,  7.0f, -7.0f,  // Vértice 12

	 7.0f, -7.0f, -7.0f,  // Vértice 13
	 7.0f, -7.0f,  7.0f,  // Vértice 14
	 7.0f,  7.0f,  7.0f,  // Vértice 15
	 7.0f,  7.0f, -7.0f,  // Vértice 16

	 // Cara superior
	 -7.0f,  7.0f,  7.0f,  // Vértice 17
	  7.0f,  7.0f,  7.0f,  // Vértice 18
	  7.0f,  7.0f, -7.0f,  // Vértice 19
	 -7.0f,  7.0f, -7.0f,  // Vértice 20

	 // Cara inferior
	 -7.0f, -7.0f, -7.0f,  // Vértice 21
	  7.0f, -7.0f, -7.0f,  // Vértice 22
	  7.0f, -7.0f,  7.0f,  // Vértice 23
	 -7.0f, -7.0f,  7.0f   // Vértice 24
};

// Called before render is available
static const GLfloat CubeVertices[] = {
	-1, -1, -1,
	1, -1, -1,
	1, 1, -1,
	-1, 1, -1,
	-1, -1, 1,
	1, -1, 1,
	1, 1, 1,
	-1, 1, 1
};
static const GLuint CubeIndices[] = {
	0, 1, 3, 3, 1, 2,
	1, 5, 2, 2, 5, 6,
	5, 4, 6, 6, 4, 7,
	4, 0, 7, 7, 0, 3,
	3, 2, 7, 7, 2, 6,
	4, 5, 0, 0, 5, 1
};

// Array de vertices para rectangulo
float vertices[] = {
	-3.0f, -3.0f, -6.0f,
	3.0f, -3.0f, -6.0f,
	3.0f, 3.0f, -6.0f,
	-3.0f, 3.0f, -6.0f,
};

// Array de indices para rectangulo
unsigned int indices[] = {
  0, 1, 2,
  2, 3, 0,
};

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	if(ret == true)
	{
		//Use Vsync
		if(VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
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
		if(error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		SetDepthTestAttribute(App->editor->depthTest);
		SetCullFaceAttribute(App->editor->cullFace);
		lights[0].Active(true);
		SetLightingAttribute(App->editor->lighting);
		SetColorMaterialAttribute(App->editor->coloMaterial);
		SetCullFaceAttribute(App->editor->cullFace);
		SetTexture2DAttribute(App->editor->texture2D);
		SetBlendAttribute(App->editor->blend);
		SetAlphaTestAttribute(App->editor->alphaTest);
		SetLineSmoothAttribute(App->editor->lineSmooth);
		SetPointSmoothAttribute(App->editor->pointSmooth);
		SetPolygonSmoothAttribute(App->editor->polygonSmooth);
		
		glewInit();
	}


	Grid.axis = true;

	
	
	glGenBuffers(1, &test);
	glBindBuffer(GL_ARRAY_BUFFER, test);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	VBO = 0;
	EBO = 0;
	VAO = 0;
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(CubeVertices), CubeVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(CubeIndices), CubeIndices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); 


	VAORect = 0;
	VBORect = 0;
	EBORect = 0;
	
	
	glGenBuffers(1, &VBORect);
	glGenBuffers(1, &EBORect);
	glGenVertexArrays(1, &VAORect);

	


	glBindVertexArray(VAORect);
	glBindBuffer(GL_ARRAY_BUFFER, VBORect);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBORect);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	return ret;
}

bool ModuleRenderer3D::Start()
{
	
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);
			
	return true;
}


// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	BindCameraBuffer(App->camera->camera);

	// light 0 on cam pos
	lights[0].SetPos(0, 0, 0);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();



	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	Grid.Render();
	DrawWithWireframe();

	//--- DRAW BAKE HOUSE ---
	App->assimpMeshes->RenderScene();

	if (mainGameCamera != nullptr) {
		//Only polygon fill
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//Bind buffer
		BindCameraBuffer(mainGameCamera);

		//Render Game Camera
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

	if (VBO != 0)
	{
		glDeleteBuffers(1, &VBO);
		VBO = 0;
	}

	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer3D::SetMainCamera(ComponentCamera* cam)
{
	//No main camera
	if (cam == nullptr) {
		mainGameCamera = nullptr;
		LOG("No existing GAME camera");
		return;
	}

	//Switch main cameras
	if (mainGameCamera != nullptr)
		mainGameCamera->isMainCamera = false;

	cam->isMainCamera = true;

	mainGameCamera = cam;
}

ComponentCamera* ModuleRenderer3D::GetMainCamera()
{
	return mainGameCamera;
}

void ModuleRenderer3D::DrawLineSegment(float3 a, float3 b)
{
	glBegin(GL_LINES);

	glColor3fv(float3(255.0f, 0, 255.0f).ptr());


	glVertex3fv(a.ptr());
	glVertex3fv(b.ptr());

	glColor3f(255.f, 255.f, 255.f);

	glEnd();
}

void ModuleRenderer3D::BindCameraBuffer(ComponentCamera* cc)
{
	//Bind game camera framebuffer
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(cc->GetProjetionMatrix());

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(cc->GetViewMatrix());

	glBindFramebuffer(GL_FRAMEBUFFER, cc->frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glLoadMatrixf(App->camera->camera->GetProjetionMatrix());

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// Setter functions for renderer attributes with logging
void ModuleRenderer3D::SetDepthTestAttribute(bool enable) {
	App->editor->depthTest = enable;

	App->editor->depthTest ? glDisable(GL_DEPTH_TEST) : glEnable(GL_DEPTH_TEST);
}

void ModuleRenderer3D::SetCullFaceAttribute(bool enable) {
	App->editor->cullFace = enable;

	App->editor->cullFace ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);

}

void ModuleRenderer3D::SetLightingAttribute(bool enable) {
	App->editor->lighting = enable;

	App->editor->lighting ? glDisable(GL_LIGHTING) : glEnable(GL_LIGHTING);

}

void ModuleRenderer3D::SetColorMaterialAttribute(bool enable) {
	App->editor->coloMaterial = enable;

	App->editor->coloMaterial ? glEnable(GL_COLOR_MATERIAL) : glDisable(GL_COLOR_MATERIAL);

}

void ModuleRenderer3D::SetTexture2DAttribute(bool enable) {
	App->editor->texture2D = enable;

	App->editor->texture2D ? glEnable(GL_TEXTURE_2D) : glDisable(GL_TEXTURE_2D);

}

void ModuleRenderer3D::SetBlendAttribute(bool enable) {
	App->editor->blend = enable;

	App->editor->blend ? glEnable(GL_BLEND) : glDisable(GL_BLEND);

}

void ModuleRenderer3D::SetAlphaTestAttribute(bool enable) {
	App->editor->alphaTest = enable;

	App->editor->alphaTest ? glEnable(GL_ALPHA_TEST) : glDisable(GL_ALPHA_TEST);

}

void ModuleRenderer3D::SetLineSmoothAttribute(bool enable) {
	App->editor->lineSmooth = enable;

	App->editor->lineSmooth ? glEnable(GL_LINE_SMOOTH) : glDisable(GL_LINE_SMOOTH);

}

void ModuleRenderer3D::SetPointSmoothAttribute(bool enable) {
	App->editor->pointSmooth = enable;

	App->editor->pointSmooth ? glEnable(GL_POINT_SMOOTH) : glDisable(GL_POINT_SMOOTH);

}

void ModuleRenderer3D::SetPolygonSmoothAttribute(bool enable) {
	App->editor->polygonSmooth = enable;

	App->editor->polygonSmooth ? glEnable(GL_POLYGON_SMOOTH) : glDisable(GL_POLYGON_SMOOTH);

}

void ModuleRenderer3D::SetVsync(bool vsync) {

	App->editor->vsync = vsync;
	if (App->editor->vsync)
	{
		SDL_GL_SetSwapInterval(1);

	}
	else
	{
		SDL_GL_SetSwapInterval(0);
	}

}

void ModuleRenderer3D::SetWireframeMode(bool enable)
{
	App->editor->wireframeMode = enable;
	DrawWithWireframe();

}

void ModuleRenderer3D::DrawWithWireframe()
{
	App->editor->wireframeMode ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

bool ModuleRenderer3D::SaveConfig(JsonParser& node) const
{

	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "vsync", App->editor->vsync);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "wireframe", App->editor->wireframeMode);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "depthTest", App->editor->depthTest);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "cullFace", App->editor->cullFace);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "lighting", App->editor->lighting);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "colorMaterial", App->editor->coloMaterial);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "texture2D", App->editor->texture2D);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "blend", App->editor->blend);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "alphaTest", App->editor->alphaTest);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "lineSmooth", App->editor->lineSmooth);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "pointSmooth", App->editor->pointSmooth);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "polygonSmooth", App->editor->polygonSmooth);

	return true;
}

bool ModuleRenderer3D::LoadConfig(JsonParser& node)
{

	App->editor->vsync = node.JsonValToBool("vsync");
	App->editor->wireframeMode = node.JsonValToBool("wireframe");
	App->editor->depthTest = node.JsonValToBool("depthTest");
	App->editor->cullFace = node.JsonValToBool("cullFace");
	App->editor->lighting = node.JsonValToBool("lighting");
	App->editor->coloMaterial = node.JsonValToBool("colorMaterial");
	App->editor->texture2D = node.JsonValToBool("texture2D");
	App->editor->blend = node.JsonValToBool("blend");
	App->editor->alphaTest = node.JsonValToBool("alphaTest");
	App->editor->lineSmooth = node.JsonValToBool("lineSmooth");
	App->editor->pointSmooth = node.JsonValToBool("pointSmooth");
	App->editor->polygonSmooth = node.JsonValToBool("polygonSmooth");

	return true;
}
