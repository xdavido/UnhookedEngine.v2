#include "Application.h"
#include "ModuleTexture.h"
#include "SDL/include/SDL_opengl.h"
#include <string>

#pragma comment(lib,"DevIL/libx86/DevIL.lib")
#pragma comment(lib,"DevIL/libx86/ILU.lib")
#pragma comment(lib,"DevIL/libx86/ILUT.lib")
#include <gl/GL.h>
#include <gl/GLU.h>


ModuleTexture::ModuleTexture(Application* app, bool start_enabled) : Module(app, start_enabled)
{

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

	//Generate and bind a texture buffer
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(0, &checkersID);
	glBindTexture(GL_TEXTURE_2D, checkersID);

	//Pick your texture settings with glTexParameter()

	//GL_TEXTURE_WRAP_S/T: How the texture behaves outside 0,1 range (s = x ; t = y)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//Resize the texture (MIN->make it smaller ; MAG->make it bigger)
	//Nearest -> pixelat / Linear -> borros
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Generate the texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkerImage);

	//Mipmap can be added or not
	//glGenerateMipmap(GL_TEXTURE_2D);

	//cleaning texture
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	//Initialize clear color
	glClearColor(0.f, 0.f, 0.f, 1.f);

	ilInit();
	 
	ilClearColour(255, 255, 255, 000);



	return true;
}

bool ModuleTexture::CreateTexture(GLuint* imgData, GLuint width, GLuint height)
{
	// Clear any existing textures
	CleanTexture();
    textureWidth = width;
	textureHeight = height;
		
	// Enable 2D texture and set active texture unit
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);

	//Generate and bind texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);	
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	// Define texture wrap behavior outside the 0-1 range (S->x, T->y)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); //GL_CLAMP
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// Define texture behavior after resizing (MIN->smaller, MAG->bigger)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Generate the texture with image data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height,
		0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);

	//Mipmap can be added or not
	glGenerateMipmap(GL_TEXTURE_2D);

	// Unbind the texture and disable 2D texture mode
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
	return true;
}

uint ModuleTexture::LoadTexture(std::string path)
{
	bool textureLoaded = false;
	//Generate and set current image ID
	ILuint imgID = 0;
	ilGenImages(1, &imgID);
	ilBindImage(imgID);
	//Load image
		ILboolean success = ilLoadImage(path.c_str());
	//Image loaded successfully
	if (success == IL_TRUE)
	{
		//Convert image to RGBA
		success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		if (success == IL_TRUE)
		{
			//Create texture from file pixels
			textureLoaded = CreateTexture((GLuint*)ilGetData(), (GLuint)ilGetInteger(IL_IMAGE_WIDTH), (GLuint)ilGetInteger(IL_IMAGE_HEIGHT));
		}
		//Delete file from memory
		ilDeleteImages(1, &imgID);
		//Report error
		if (!textureLoaded)
		{
			printf("Unable to load %s\n", path.c_str());
		}


		App->hierarchy->objSelected
	}
	return imgID;
}

bool ModuleTexture::CleanTexture()
{
	//Delete texture
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
