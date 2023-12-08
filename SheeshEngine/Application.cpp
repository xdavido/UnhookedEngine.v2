#include "Application.h"

Application::Application()
{

	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	renderer3D = new ModuleRenderer3D(this);
	camera = new ModuleCamera3D(this);
	editor = new ModuleEditor(this);
	assimpMeshes = new ModuleAssimpMeshes(this);
	textures = new ModuleTexture(this);
	scene = new ModuleScene(this);
	hierarchy = new ModuleHierarchy(this);
	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	AddModule(hierarchy);
	AddModule(assimpMeshes);
	AddModule(textures);
	AddModule(scene);

	// Renderer last!
	AddModule(renderer3D);
	AddModule(editor);

	loadRequested = true;
	saveRequested = false;
}

Application::~Application()
{
	for (std::vector<Module*>::iterator it = list_modules.begin(); it != list_modules.end(); ++it)
	{
		delete (*it);
		(*it) = nullptr;
	}
}

bool Application::Init()
{
	bool ret = true;

	JSON_Value* root = jsonFile.FileToValue("config.json");

	if (jsonFile.GetRootValue() == NULL)
	{
		LOG("Couldn't load config.json");
		ret = false;
	}

	JsonParser application = jsonFile.GetChild(root, "App");

	maxFrameRate = application.JsonValToNumber("FPS");

	maxFrameRate = 300;

	// Call Init() in all modules
	for (std::vector<Module*>::const_iterator it = list_modules.cbegin(); it != list_modules.cend() && ret; ++it)
	{
		(*it)->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("-------------- Application Start --------------");
	for (std::vector<Module*>::const_iterator it = list_modules.cbegin(); it != list_modules.cend() && ret; ++it)
	{
		(*it)->Start();
	}
	
	ms_timer.Start();
	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{	 
	if (loadRequested)
	{
		LoadConfig();
	}

	if (saveRequested) 
	{
		SaveConfig();
	}


	if (App->editor->vsync)
	{
		msLastFrame = ms_timer.Read();

		float timeToWait = 1000.0f / (float)maxFrameRate;

		if (msLastFrame < timeToWait)
			SDL_Delay(static_cast<Uint32>(fabs(timeToWait - msLastFrame)));

		msLastFrame = ms_timer.Read();

	}

}




// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	update_status ret = UPDATE_CONTINUE;
	PrepareUpdate();
	
	for (std::vector<Module*>::const_iterator it = list_modules.cbegin(); it != list_modules.cend() && ret == UPDATE_CONTINUE; ++it)
	{
		ret = (*it)->PreUpdate(dt);
	}

	for (std::vector<Module*>::const_iterator it = list_modules.cbegin(); it != list_modules.cend() && ret == UPDATE_CONTINUE; ++it)
	{
		ret = (*it)->Update(dt);
	}

	for (std::vector<Module*>::const_iterator it = list_modules.cbegin(); it != list_modules.cend() && ret == UPDATE_CONTINUE; ++it)
	{
		ret = (*it)->PostUpdate(dt);
	}

	FinishUpdate();
	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (std::vector<Module*>::reverse_iterator it = list_modules.rbegin(); it != list_modules.rend() && ret; ++it)
	{
		ret = (*it)->CleanUp();
	}


	return ret;
}

void Application::LOGToEditor(const char* text)
{
	
	editor->LOGToConsole(text);
	
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

void Application::SaveConfig()
{
	LOG("Saving configuration");

	JSON_Value* root = jsonFile.GetRootValue();

	JsonParser application = jsonFile.SetChild(root, "App");

	application.SetNewJsonNumber(application.ValueToObject(application.GetRootValue()), "FPS", maxFrameRate);


	// Call SaveConfig() in all modules
	std::vector<Module*>::iterator item;

	for (item = list_modules.begin(); item != list_modules.end(); ++item)
	{
		(*item)->SaveConfig(jsonFile.SetChild(root, (*item)->name));
	}

	jsonFile.SerializeFile(root, "config.json");
	saveRequested = false;
}


void Application::LoadConfig()
{
	LOG("Loading configurations");

	JSON_Value* root = jsonFile.GetRootValue();

	JsonParser application = jsonFile.GetChild(root, "App");

	maxFrameRate = application.JsonValToNumber("FPS");


	std::vector<Module*>::iterator item;

	for (item = list_modules.begin(); item != list_modules.end(); ++item)
	{
		(*item)->LoadConfig(jsonFile.GetChild(root, (*item)->name));
	}

	loadRequested = false;
}