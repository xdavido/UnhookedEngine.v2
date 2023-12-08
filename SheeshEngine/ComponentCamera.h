#pragma once
#include "Component.h"
#include "MathGeoLib/include/MathGeoLib.h"

struct Mesh;

class ComponentCamera : public Component
{
public:
	ComponentCamera();
	ComponentCamera(GameObject owner);
	~ComponentCamera();

	void PrintInspector();
	void Update();

	void CreateFrameBuffer();

	void Look(const float3& Position, const float3& Reference);
	void LookAt(const float3& Spot);
	void Move(const float3& Movement);
	void SetAspectRatio(float aspectRatio = 1.7);
	float* GetViewMatrix();
	float* GetProjetionMatrix();
	bool FrustumCulling(Mesh* mesh);

	Frustum frustum;
	float3 reference;

	int typeCameraSelected = 0;

	int cameraFOV = 60;
	float farDistance = 500;
	float nearDistance = 0.1;

	bool isMainCamera = false;

	unsigned int cameraBuffer;
	unsigned int frameBuffer;
	unsigned int renderObjBuffer;

	float4x4 viewMatrix;

private:
	
	float4x4 projectionMatrix;


};