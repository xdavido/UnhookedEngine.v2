#pragma once
#pragma once
#include "Application.h"

using namespace std;

class GameWindows
{
public:
	static void PrintCamera(Application* app);
	GameObject* mainCamera;

private:

	static ImVec2 sizeWindScn;
};