#include "SceneWindow.h"
#include "Imgui/imgui.h"
#include "ComponentCamera.h"
#include "ComponentMesh.h"
#include "ModuleAssimpMeshes.h"
#include "ComponentTransform.h"

ImVec2 SceneWindows::sizeWindScn = { 0,0 };
bool SceneWindows::isHovered = false;

void SceneWindows::PrintScene(Application* app)
{
	//Begin scene & get size
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Scene");
	sizeWindScn = ImGui::GetContentRegionAvail();

	//Modify by aspect Ratio
	float aspectRatio = sizeWindScn.x / sizeWindScn.y;
	app->camera->camera->SetAspectRatio(aspectRatio);
	ImGui::Image((ImTextureID)app->camera->camera->cameraBuffer, sizeWindScn, ImVec2(0, 1), ImVec2(1, 0));

	if (ImGui::IsWindowHovered())
	{
		isHovered = true;
	}
	else
	{
		isHovered = false;
	}

	ImGui::End();
	ImGui::PopStyleVar();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

ImVec2 SceneWindows::NormMousePos(float x, float y, float w, float h, ImVec2 p)
{
	ImVec2 normP;

	normP.x = -1.0 + 2.0 * ((p.x - x) / w);
	normP.y = 1.0 - 2.0 * ((p.y - y) / h);

	return normP;
}