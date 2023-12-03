#include "Globals.h"
#include "Component.h"
#include "imgui.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float4x4.h"

class GameObject;
class Component;

class CameraComp : public Component
{
public:

	CameraComp();
	CameraComp(GameObject* owner);
	~CameraComp();

	void SetCam();
	void GenBuffer();

	float* GetViewMatrix();
	float* GetProjectionMatrix();

	void LookAt(const float3& target);

	void TransformCam();

	void Inspector();

	bool ContainsAaBox(Mesh* refBox);

	Frustum FrustumCam;
	float4x4 viewMatrix;
	float4x4 projectionMatrix;

	int printCount;

	int FOV = 60.0f;

	unsigned int cameraBuffer;
	unsigned int frameBuffer;

private:

	unsigned int renderObjBuffer;
};