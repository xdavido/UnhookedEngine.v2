#pragma once
#include"Globals.h"
#include"JsonParser.h"

class Application;
struct PhysBody3D;

class Module
{
private :
	bool enabled;

public:
	Application* App;

	const char* name;

	Module(Application* parent, bool start_enabled = true) : App(parent), enabled(start_enabled), name("notNamed")
	{}

	virtual ~Module()
	{}

	virtual bool Init() 
	{
		return true; 
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp() 
	{ 
		return true; 
	}

	virtual bool LoadConfig(JsonParser& node)
	{
		return true;
	}

	virtual bool SaveConfig(JsonParser& node) const
	{
		return true;
	}


};