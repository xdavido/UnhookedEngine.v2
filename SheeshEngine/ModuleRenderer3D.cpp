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
	vsync = VSYNC;
	depthTestAttribute = DEPTH_TEST_ATTRIBUTE;
	cullFaceAttribute = CULL_FACE_ATTRIBUTE;
	lightingAttribute = LIGHTING_ATTRIBUTE;
	colorMaterialAttribute = COLOR_MATERIAL_ATTRIBUTE;
	texture2DAttribute = TEXTURE_2D_ATTRIBUTE;
	blendAttribute = BLEND_ATTRIBUTE;
	alphaTestAttribute = ALPHA_TEST_ATTRIBUTE;
	lineSmoothAttribute = LINE_SMOOTH_ATTRIBUTE;
	pointSmoothAttribute = POINT_SMOOTH_ATTRIBUTE;
	polygonSmoothAttribute = POLYGON_SMOOTH_ATTRIBUTE;
	wireframeMode = WIREFRAME_MODE;
	
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
	/*lights[0].SetPos(App->camera->camera->frustum.pos.x, App->camera->camera->frustum.pos.y, App->camera->camera->frustum.pos.z);*/

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

	//(1)--- CUBO CON VÉRTICES [DIRECT MODE] ---
	//DirectModeTriangleDrawing();

	//(2)--- CUBO CON VERTEX ARRAYS Y glDrawElements() ---
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glBindBuffer(GL_ARRAY_BUFFER, test);
	//glVertexPointer(3, GL_FLOAT, 0, NULL);
	//
	//glDrawArrays(GL_TRIANGLES, 0, (sizeof(g_vertex_buffer_data) / sizeof(float)) / 3);
	//glDisableClientState(GL_VERTEX_ARRAY);


	//(3)--- CUBO CON INDICES (EBO) ---
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glVertexPointer(3, GL_FLOAT, 0, NULL);
	//glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);
	
	
	//(4)--- EJEMPLO PARA PRÁCTICAR ---
	/*glBindBuffer(GL_ARRAY_BUFFER, VBORect);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBORect);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, NULL);*/

	// la linea de abajo es lo que usariamos si quisieramos pintar los vertices, pero queremos con indices, por tanto
	//glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(float) / 3);
	/*glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
	glDisableClientState(GL_VERTEX_ARRAY);*/

	//(4)--- DRAW BAKE HOUSE ---
	App->assimpMeshes->RenderScene();

	/*DrawLineSegment(linesegment.a, linesegment.b);*/

	if (mainGameCamera != nullptr) {
		//Only polygon fill
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		//Bind buffer
		BindCameraBuffer(mainGameCamera);

		//Render Game Camera
		App->assimpMeshes->RenderGameWindow();

		/*DrawLineSegment(linesegment.a, linesegment.b);*/
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//(5)--- DRAW TRIANDLE AND ITS NORMAL IN DIRECT MODE
	//glLineWidth(2.0f);
	//glBegin(GL_TRIANGLES);
	//glVertex3d(3, 3, 3); 
	//glVertex3d(4, 4, 3); 
	//glVertex3d(4, 3, 3);
	//glEnd();
	//glLineWidth(1.0f);

	////Calcular el punto medio de la cara del triangulo
	//double xMedio = (3 + 4 + 4) / 3.0; 
	//double yMedio = (3 + 4 + 3) / 3.0;
	//double zMedio = (3 + 3 + 3) / 3.0;

	//// Calcula la normal de la cara del triángulo usando el producto cruz
	//double edge1x = 4 - 3;
	//double edge1y = 4 - 3;
	//double edge1z = 3 - 3;
	//double edge2x = 4 - 3;
	//double edge2y = 3 - 3;
	//double edge2z = 3 - 3;

	//double normalx = edge1y * edge2z - edge1z * edge2y;
	//double normaly = edge1z * edge2x - edge1x * edge2z;
	//double normalz = edge1x * edge2y - edge1y * edge2x;

	//// Normaliza la normal
	//double length = sqrt(normalx * normalx + normaly * normaly + normalz * normalz);
	//normalx /= length;
	//normaly /= length;
	//normalz /= length;

	//// Define la longitud de la línea en la dirección de la normal
	//double lineaLongitud = 3.0;

	//// Calcula el punto final de la línea
	//double xFinal = xMedio + normalx * lineaLongitud;
	//double yFinal = yMedio + normaly * lineaLongitud;
	//double zFinal = zMedio + normalz * lineaLongitud;

	//glLineWidth(2.0f);
	//glBegin(GL_LINES);
	//glVertex3d(xMedio, yMedio, zMedio);
	//glVertex3d(xFinal, yFinal, zFinal);
	//glEnd();
	//glLineWidth(1.0f);
	
	

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
	depthTestAttribute = enable;

	depthTestAttribute ? glDisable(GL_DEPTH_TEST) : glEnable(GL_DEPTH_TEST);

	std::string aux = "Renderer depthTestAttribute updated to: " + std::string(enable ? "false" : "true" );
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

	std::string aux = "Renderer lightingAttribute updated to: " + std::string(enable ? "false" : "true" );
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

bool ModuleRenderer3D::SaveConfig(JsonParser& node) const
{
	//Module::SaveConfig(node);

	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "vsync", vsync);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "wireframe", wireframeMode);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "depthTest", depthTestAttribute);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "cullFace", cullFaceAttribute);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "lighting", lightingAttribute);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "colorMaterial", colorMaterialAttribute);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "texture2D", texture2DAttribute);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "blend", blendAttribute);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "alphaTest", alphaTestAttribute);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "lineSmooth", lineSmoothAttribute);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "pointSmooth", pointSmoothAttribute);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "polygonSmooth", polygonSmoothAttribute);

	return true;
}

bool ModuleRenderer3D::LoadConfig(JsonParser& node)
{
	//Module::LoadConfig(node);

	vsync = node.JsonValToBool("vsync");
	wireframeMode = node.JsonValToBool("wireframe");
	depthTestAttribute = node.JsonValToBool("depthTest");
	cullFaceAttribute = node.JsonValToBool("cullFace");
	lightingAttribute = node.JsonValToBool("lighting");
	colorMaterialAttribute = node.JsonValToBool("colorMaterial");
	texture2DAttribute = node.JsonValToBool("texture2D");
	blendAttribute = node.JsonValToBool("blend");
	alphaTestAttribute = node.JsonValToBool("alphaTest");
	lineSmoothAttribute = node.JsonValToBool("lineSmooth");
	pointSmoothAttribute = node.JsonValToBool("pointSmooth");
	polygonSmoothAttribute = node.JsonValToBool("polygonSmooth");

	return true;
}