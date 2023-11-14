#include "Application.h"
#include "ModuleTexture.h"
#include "ComponentMaterial.h"
#include "SDL/include/SDL_opengl.h"
#include <string>

#pragma comment(lib,"DevIL/libx86/DevIL.lib")
#pragma comment(lib,"DevIL/libx86/ILU.lib")
#pragma comment(lib,"DevIL/libx86/ILUT.lib")
#include <gl/GL.h>
#include <gl/GLU.h>


ModuleTexture::ModuleTexture(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Texture";
}


bool ModuleTexture::Start()
{
	//Initialize checker image
	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0;	 j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkerImage[i][j][0] = (GLubyte)c;
			checkerImage[i][j][1] = (GLubyte)c;
			checkerImage[i][j][2] = (GLubyte)c;
			checkerImage[i][j][3] = (GLubyte)255;
		}
	}


	glEnable(GL_TEXTURE_2D);

	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &checkersID);
	glBindTexture(GL_TEXTURE_2D, checkersID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	glClearColor(0.f, 0.f, 0.f, 1.f);

	ilInit();
	 
	ilClearColour(255, 255, 255, 000);



	return true;
}

bool ModuleTexture::CreateTexture(GLuint* imgData, GLuint width, GLuint height)
{
	
	return true;
}

uint ModuleTexture::LoadTexture(const char* path)
{
	
	uint devilImageId;
	ilGenImages(1, &devilImageId);
	ilBindImage(devilImageId);

	bool success = ilLoadImage(path);

	success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);


	
	BYTE* data = ilGetData();
	ILuint imgWidth, imgHeight;
	imgWidth = ilGetInteger(IL_IMAGE_WIDTH);
	imgHeight = ilGetInteger(IL_IMAGE_HEIGHT);
	textureHeight = imgHeight;
	textureWidth = imgWidth;
	int const type = ilGetInteger(IL_IMAGE_TYPE);
	int const format = ilGetInteger(IL_IMAGE_FORMAT);

	
	uint imageId = ilutGLBindTexImage();
	glBindTexture(GL_TEXTURE_2D, imageId);

	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	
	glTexImage2D(GL_TEXTURE_2D, 0, format, imgWidth, imgHeight, 0, format, type, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	if (App->hierarchy->objSelected != nullptr ) {
		if (App->hierarchy->objSelected->GetMeshComponent() != nullptr) {

			App->hierarchy->objSelected->GetMeshComponent()->mesh->texture_width = imgWidth;
			App->hierarchy->objSelected->GetMeshComponent()->mesh->texture_height = imgHeight;
		}
		if (App->hierarchy->objSelected->GetComponentTexture() != nullptr) {

			App->hierarchy->objSelected->GetComponentTexture()->pathTexture = path;
		}
	}



	ilDeleteImages(1, &devilImageId);

	glBindTexture(GL_TEXTURE_2D, 0);

	return imageId;
}

bool ModuleTexture::CleanTexture()
{
	
	if (textureID != 0)
	{
		glDeleteTextures(1, &textureID);
		textureID = 0;
	}
	textureWidth = 0;
	textureHeight = 0;

	return true;
}

bool ModuleTexture::CleanTexture(GLuint id)
{
	if (textureID != 0)
	{
		glDeleteTextures(1, &textureID);
		textureID = 0;
	}

	return false;
}

void ModuleTexture::DestroyTexture(uint textureID)
{
	glDeleteBuffers(1, &textureID);
}

void ModuleTexture::NewTexture( uint imgId)
{

	if (App->hierarchy->objSelected != nullptr) {
		
		ComponentMaterial* mat = App->hierarchy->objSelected->GetComponentTexture();
		if (mat == nullptr) {
			App->hierarchy->objSelected->AddComponent(mat);
		}
		else
		{
			mat->textureID = imgId;
		}


	}
}
