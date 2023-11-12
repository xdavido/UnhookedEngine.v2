#include "ModuleScene.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleAssimpMeshes.h"
#include "Component.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"
#include "MathGeoLib/include/Math/Quat.h"
#include "MathGeoLib/include/Math/float3.h"


ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled) 
{
}

ModuleScene::~ModuleScene() 
{
}

bool ModuleScene::Init()
{
    return false;
}

bool ModuleScene::Start() {
    
    root = new GameObject(nullptr);
    root->name = ("Scene");



    //Load Baker House
    App->assimpMeshes->LoadMeshFromFile("Assets/Models/BakerHouse.fbx");
   

    return true;
}


update_status ModuleScene::PreUpdate(float dt) {
    
    return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt) {

	if (ImGui::IsMouseClicked(0) && App->input->GetKey(SDL_SCANCODE_LALT) != KEY_REPEAT && ImGui::IsWindowHovered())
	{
		//Close GO options menu
		/*App->hierarchy->openGOOptions = false;*/

		std::vector<GameObject*> PickedGO;

		ImVec2 mousePos = ImGui::GetMousePos();

		ImVec2 norm = NormMousePos(ImGui::GetWindowPos().x,
			ImGui::GetWindowPos().y + ImGui::GetFrameHeight(),
			ImGui::GetWindowSize().x,
			ImGui::GetWindowSize().y - ImGui::GetFrameHeight(), mousePos);

		//LineSegment picking = App->camera->cam->frustum.UnProjectLineSegment(norm.x, norm.y);

		//app->meshRenderer->debugRaycastA = picking.a;
		//app->meshRenderer->debugRaycastB = picking.b;

		for (size_t i = 0; i < App->assimpMeshes->meshes.size(); i++)
		{
			if (/*picking.Intersects(App->assimpMeshes->meshes[i]->OBB_box) &&*/ App->assimpMeshes->meshes[i]->owner->isEnabled)
			{
				if (App->assimpMeshes->meshes[i]->owner != nullptr)
					PickedGO.push_back(App->assimpMeshes->meshes[i]->owner);
			}
		}


		float currentDist;
		float minDist = 0;

		for (size_t i = 0; i < PickedGO.size(); i++)
		{
			ComponentMesh* cm = PickedGO[i]->GetMeshComponent();

			for (size_t k = 0; k < cm->meshes.size(); k++)
			{
				Mesh* m = cm->meshes[k];
				float4x4 mat = PickedGO[i]->transform->getGlobalMatrix().Transposed();

				if (m->indexCount > 6)
				{
					for (size_t j = 0; j < m->indexCount; j += 3)
					{
						//Get mesh vertex xyz
						float* v1 = &m->vertex[m->index[j] * VERTEX];
						float* v2 = &m->vertex[m->index[j + 1] * VERTEX];
						float* v3 = &m->vertex[m->index[j + 2] * VERTEX];

						//Transform vertex
						float4 pT1 = mat * float4(*v1, *(v1 + 1), *(v1 + 2), 1);
						float4 pT2 = mat * float4(*v2, *(v2 + 1), *(v2 + 2), 1);
						float4 pT3 = mat * float4(*v3, *(v3 + 1), *(v3 + 2), 1);

						//Get vertex position in float3
						float3 _pt1 = float3(pT1.x, pT1.y, pT1.z);
						float3 _pt2 = float3(pT2.x, pT2.y, pT2.z);
						float3 _pt3 = float3(pT3.x, pT3.y, pT3.z);

						//Set triangle
						Triangle triangle(_pt1, _pt2, _pt3);

						//Compare triangle intersecting
						if (picking.Intersects(triangle, &currentDist, nullptr))
						{
							//Set initial minDist
							if (minDist == 0) {
								minDist = currentDist;
								App->hierarchy->SetGameObject(PickedGO[i]);
								continue;
							}

							//If nearer, select
							if (minDist > currentDist) {
								minDist = currentDist;
								App->hierarchy->SetGameObject(PickedGO[i]);
							}
						}
					}
				}

			}
		}
		//If no object selected, make nullptr
		if (PickedGO.size() == 0) App->hierarchy->SetGameObject(nullptr);
		PickedGO.clear();
	}
    return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate(float dt) {
    
    return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp() {
    
    LOG("Scene Cleanup")
    delete root;
    return true;
}

GameObject* ModuleScene::CreateGameObject(GameObject* parent)
{
    GameObject* newGameObject = new GameObject(parent);



    return newGameObject;

}

ImVec2 ModuleScene::NormMousePos(float x, float y, float w, float h, ImVec2 p)
{
	ImVec2 normP;

	normP.x = -1.0 + 2.0 * ((p.x - x) / w);
	normP.y = 1.0 - 2.0 * ((p.y - y) / h);

	return normP;
}

