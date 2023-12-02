#pragma once

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 )

#include <windows.h>
#include <stdio.h>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define PI 3.14159265358979323846264338327950288

// Deletes a buffer
#define RELEASE( x )\
    {\
       if( x != nullptr )\
       {\
         delete x;\
	     x = nullptr;\
       }\
    }

// Deletes an array of buffers
#define RELEASE_ARRAY( x )\
	{\
       if( x != nullptr )\
       {\
           delete[] x;\
	       x = nullptr;\
		 }\
	 }

// Deletes an std::vector
#define RELEASE_VECTOR( x, s )\
	{\
		for (size_t i = 0; i < s; i++)\
		{\
			delete x[i];\
			x[i] = nullptr;\
		}\
	}



typedef unsigned int uint;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
//Window
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 1024
#define SCREEN_SIZE 1
#define WIN_FULLSCREEN false
#define WIN_RESIZABLE true
#define WIN_BORDERLESS false
#define WIN_FULLSCREEN_DESKTOP false
#define TITLE "UnhookedEngine.v2 Engine"

//Render
#define VSYNC true
#define DEPTH_TEST_ATTRIBUTE false
#define CULL_FACE_ATTRIBUTE true
#define LIGHTING_ATTRIBUTE false
#define COLOR_MATERIAL_ATTRIBUTE true
#define TEXTURE_2D_ATTRIBUTE false
#define BLEND_ATTRIBUTE false
#define ALPHA_TEST_ATTRIBUTE false
#define LINE_SMOOTH_ATTRIBUTE false
#define POINT_SMOOTH_ATTRIBUTE false
#define POLYGON_SMOOTH_ATTRIBUTE false
#define WIREFRAME_MODE false

//Colors
#define IMGUICOL_GREEN_LIGHT ImVec4(0.0f, 1.0f, 0.0f, 1.0f)
#define IMGUICOL_YELLOW_LIGHT ImVec4(1.0f, 1.0f, 0.0f, 1.0f)
#define IMGUICOL_RED_LIGHT ImVec4(1.0f, 0.0f, 0.0f, 1.0f)
#define IMGUICOL_BLUE_LIGHT ImVec4(0.2f, 0.2f, 1.0f, 1.0f)
#define IMGUICOL_GREY ImVec4(0.6f, 0.6f, 0.6f, 1.0f)
#define IMGUICOL_WHITE ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
#define IMGUICOL_BLUE_SOFT ImVec4(0.4f, 0.7f, 0.9f, 1.0f) 
#define IMGUICOL_RED_SOFT ImVec4(0.9f, 0.4f, 0.4f, 1.0f) 
#define IMGUICOL_GREEN_SOFT ImVec4(0.2f, 0.8f, 0.2f, 1.0f) 
#define IMGUICOL_BROWN_SOFT ImVec4(0.7f, 0.5f, 0.2f, 1.0f) 
#define IMGUICOL_PURPLE_SOFT ImVec4(0.5f, 0.2f, 0.7f, 1.0f) 
#define IMGUICOL_ORANGE ImVec4(1.0f, 0.5f, 0.0f, 1.0f) 
#define IMGUICOL_CYAN ImVec4(0.0f, 1.0f, 1.0f, 1.0f) 
#define IMGUICOL_MAGENTA ImVec4(1.0f, 0.0f, 1.0f, 1.0f) 
#define IMGUICOL_PINK ImVec4(1.0f, 0.4f, 0.7f, 1.0f) 
#define IMGUICOL_LIME_GREEN ImVec4(0.6f, 0.8f, 0.2f, 1.0f) 
#define IMGUICOL_SKY_BLUE ImVec4(0.4f, 0.7f, 0.9f, 1.0f) 

//Primitive Library
#define CUBE "Assets/Models/Cube.fbx"
#define PLANE "Assets/Models/Plane.fbx"
#define SPHERE "Assets/Models/Sphere.fbx"
#define ICOSPHERE "Assets/Models/IcoSphere.fbx"
#define PYRAMID "Assets/Models/Pyramid.fbx"
#define CYLINDER "Assets/Models/Cylinder.fbx"
#define TORUS "Assets/Models/Torus.fbx"
#define SUZZANE "Assets/Models/Suzanne.fbx"