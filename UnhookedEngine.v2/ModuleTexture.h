#pragma once
#include "Module.h"
#include "Glew\include\glew.h"
#include "DevIL\include\il.h"
#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"
#pragma comment(lib,"DevIL/libx86/DevIL.lib")
#pragma comment(lib,"DevIL/libx86/ILU.lib")
#pragma comment(lib,"DevIL/libx86/ILUT.lib")
#include <string>

#define CHECKERS_HEIGHT 64
#define CHECKERS_WIDTH 64

enum class Tex_Types {
	NONE,
	CHECKERS,
	CURRENT,
};

class ModuleTexture : public Module
{
public:

	ModuleTexture(Application* app, bool start_enabled = true);
	~ModuleTexture() {}

	bool Start();
	bool CreateTexture(GLuint* imgData, GLuint width, GLuint height);
	uint LoadTexture(const char* path);
	bool CleanTexture();
	bool CleanTexture(GLuint id);
	void DestroyTexture(uint textureID);
	void NewTexture(uint imgID);
	
public:
	GLuint checkersID=0	;
	GLuint textureWidth = 0;
	GLuint textureHeight = 0;
	GLuint textureID = 0;
		
private:
	GLubyte checkerImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4]; // height width rgba
};