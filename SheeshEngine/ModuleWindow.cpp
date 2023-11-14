#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

ModuleWindow::ModuleWindow(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	window = NULL;
	screen_surface = NULL;
	name = "Window";
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	fullscreen = WIN_FULLSCREEN;
	resizable = WIN_RESIZABLE;
	borderless = WIN_BORDERLESS;
	fulldesktop = WIN_FULLSCREEN_DESKTOP;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		width = SCREEN_WIDTH * SCREEN_SIZE;
		height = SCREEN_HEIGHT * SCREEN_SIZE;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if(WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);

	std::string aux = "Window Title updated to: " + std::string(title);
	LOG(aux.c_str());
}

void ModuleWindow::SetFullscreen(bool newFullscreen) {
	fullscreen = newFullscreen;

	fullscreen ? SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN) : SDL_SetWindowFullscreen(window, 0);
	
	std::string aux = "Window Fullscreen attribute updated to: " + std::string(newFullscreen ? "true" : "false");
	LOG(aux.c_str());
}

void ModuleWindow::SetResizable(bool newResizable) {
	resizable = newResizable;

	resizable ? SDL_SetWindowResizable(window, SDL_TRUE) : SDL_SetWindowResizable(window, SDL_FALSE);
	
	std::string aux = "Window Resizable attribute updated to: " + std::string(newResizable ? "true" : "false");
	LOG(aux.c_str());
}

void ModuleWindow::SetBorderless(bool newBorderless) {
	borderless = newBorderless;

	borderless ? SDL_SetWindowBordered(window, SDL_FALSE) : SDL_SetWindowBordered(window, SDL_TRUE);
	
	std::string aux = "Window Borderless attribute updated to: " + std::string(newBorderless ? "true" : "false");
	LOG(aux.c_str());
}

void ModuleWindow::SetFulldesktop(bool newFulldesktop) {
	fulldesktop = newFulldesktop;
																						// Modo ventana
	fulldesktop ? SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP) : SDL_SetWindowFullscreen(window, 0);

	std::string aux = "Window Fulldesktop attribute updated to: " + std::string(newFulldesktop ? "true" : "false");
	LOG(aux.c_str());
}

void ModuleWindow::OnWidthChanged()
{
	SDL_SetWindowSize(window, width, height);
	App->renderer3D->OnResize(width, height);

	std::string aux = "Window Width attribute updated to: " + std::to_string(width);
	LOG(aux.c_str());
}

void ModuleWindow::OnHeightChanged()
{
	SDL_SetWindowSize(window, width, height);
	App->renderer3D->OnResize(width, height);

	std::string aux = "Window Height attribute updated to: " + std::to_string(height);
	LOG(aux.c_str());

}

bool ModuleWindow::SaveConfig(JsonParser& node) const
{
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "width", width);
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "height", height);
	node.SetNewJsonNumber(node.ValueToObject(node.GetRootValue()), "brightness", App->editor->brightness);

	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "fullscreen", fullscreen);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "fullscreen desktop", fulldesktop);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "borderless", borderless);
	node.SetNewJsonBool(node.ValueToObject(node.GetRootValue()), "resizable", resizable);

	return true;
}

bool ModuleWindow::LoadConfig(JsonParser& node)
{
	width = (int)node.JsonValToNumber("width") * SCREEN_SIZE;
	height = (int)node.JsonValToNumber("height") * SCREEN_SIZE;
	App->editor->brightness = (float)node.JsonValToNumber("brightness");

	fullscreen = node.JsonValToBool("fullscreen");
	fulldesktop = node.JsonValToBool("fullscreen desktop");
	borderless = node.JsonValToBool("borderless");
	resizable = node.JsonValToBool("resizable");

	if (fullscreen) SetFullscreen(fullscreen);
	else if (fulldesktop) SetFulldesktop(fulldesktop);
	if (borderless) SetBorderless(borderless);

	SetResizable(resizable);
	SetSize(width, height);

	return true;
}

void ModuleWindow::SetSize(int width, int height)
{
	this->height = height;
	this->width = width;

	SDL_SetWindowSize(window, width, height);
}
