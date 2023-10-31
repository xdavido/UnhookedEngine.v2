#include "Component.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleAssimpMeshes.h"
#include "ComponentMesh.h"
#include "ImGui/imgui.h"
#include <string>


class ComponentMaterial : public Component
{
public:
	ComponentMaterial();
	ComponentMaterial(GameObject* owner);
	~ComponentMaterial();

	void SetTexture(std::string path);
	void ResetTexture();
	void UpdateMeshTexture();

	void PrintInspector();


	GLuint textureID = 0;
	int textureSelected = 0;


};
