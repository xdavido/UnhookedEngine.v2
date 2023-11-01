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
	
	return true;
}

uint ModuleTexture::LoadTexture(const char* path)
{
	//Generate DevIL buffers
		uint devilImageId;
	ilGenImages(1, &devilImageId);
	ilBindImage(devilImageId);

	//Load image to binded buffer
	bool success = ilLoadImage(path);

	/*if (!success) {
		LOGT(LogsType::WARNINGLOG, "Error loading texture %s", file_path);
		return 0;
	}*/

	success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	//if (!success) {
	//    LOGT(LogsType::WARNINGLOG, "Error converting texture %s", file_path);
	//    return 0;
	//}

	//Extract loaded image data
	BYTE* data = ilGetData();
	ILuint imgWidth, imgHeight;
	imgWidth = ilGetInteger(IL_IMAGE_WIDTH);
	imgHeight = ilGetInteger(IL_IMAGE_HEIGHT);
	int const type = ilGetInteger(IL_IMAGE_TYPE);
	int const format = ilGetInteger(IL_IMAGE_FORMAT);

	//Change DevIL buffer ID to Glew buffer ID (create buffer by copying binded buffer)
	uint imageId = ilutGLBindTexImage();
	glBindTexture(GL_TEXTURE_2D, imageId);

	//How texture behaves outside 0,1 range (S->x, T->y)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	//Texture behaviour after resize (MIN->smaller , MAG->bigger)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	//Create Texture
	glTexImage2D(GL_TEXTURE_2D, 0, format, imgWidth, imgHeight, 0, format, type, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	//CLEANUP

	//Delete DevIL image buffer
	ilDeleteImages(1, &devilImageId);

	//Unbind glew buffer
	glBindTexture(GL_TEXTURE_2D, 0);

	return imageId;
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
