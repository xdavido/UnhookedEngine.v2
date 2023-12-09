#include "Globals.h"
#include "Component.h"
#include "Imgui/imgui.h"
#include "MathGeoLib/include/MathGeoLib.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float4x4.h"

class GameObject;
class Component;
struct Mesh;

class ComponentCamera : public Component
{
public:

	ComponentCamera();
	ComponentCamera(GameObject* owner);
	~ComponentCamera();

	void SetCam();
	void GenBuffer();

	bool ContainsAaBox(Mesh* refBox);

	float* GetViewMatrix();
	float* GetProjectionMatrix();
	bool FrustumCulling(Mesh* mesh);

	void LookAt(const float3& target);

	void TransformCam();

	void Inspector();

	//bool ContainsAaBox(Mesh* refBox); <- sirve para frustrum

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