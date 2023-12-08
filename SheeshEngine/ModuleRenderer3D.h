#pragma once
#include "Module.h"
#include "Globals.h"
#include "Light.h"
#include"MathGeoLib/include/MathGeoLib.h"
#include "MathGeoLib/include/Math/float3x3.h"
#include "MathGeoLib/include/Math/float4x4.h"
#include "Primitive.h"
#include "ComponentCamera.h"
#include "Glew/include/glew.h"


#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	bool Start() override;
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	void DirectModeTriangleDrawing();
	bool CleanUp();

	void SetMainCamera(ComponentCamera* cam);

	ComponentCamera* GetMainCamera();

	void DrawLineSegment(float3 a, float3 b);

	void BindCameraBuffer(ComponentCamera* cc);

	bool SaveConfig(JsonParser& node) const;

	bool LoadConfig(JsonParser& node);

	void OnResize(int width, int height);

	// Setter functions for renderer attributes
	void SetDepthTestAttribute(bool enable);
	void SetCullFaceAttribute(bool enable);
	void SetLightingAttribute(bool enable);
	void SetColorMaterialAttribute(bool enable);
	void SetTexture2DAttribute(bool enable);
	void SetBlendAttribute(bool enable);
	void SetAlphaTestAttribute(bool enable);
	void SetLineSmoothAttribute(bool enable);
	void SetPointSmoothAttribute(bool enable);
	void SetPolygonSmoothAttribute(bool enable);
	void SetVsync(bool vsync);


	void SetWireframeMode(bool enable);


private:
	void DrawWithWireframe();
	

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	CPlane Grid;
	
	float3x3 NormalMatrix;
	float4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;


	unsigned int cameraBuffer;
	unsigned int inGameBuffer;
	unsigned int frameBuffer;
	unsigned int renderObjBuffer;

	GLuint test;
	GLuint VBO;
	GLuint EBO;
	GLuint VAO;

	//test rectangulo
	GLuint VAORect;
	GLuint VBORect;
	GLuint EBORect;

	LineSegment linesegment;
	
	
private:

	ComponentCamera* mainGameCamera;
};