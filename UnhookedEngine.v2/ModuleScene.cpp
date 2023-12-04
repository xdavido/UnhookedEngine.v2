#include "ModuleScene.h"
#include "GameObject.h"
#include "Application.h"
#include "ComponentTransform.h"
#include "ComponentMesh.h"

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

    root_Assets = new GameObject(nullptr);
    root_Assets->name = ("Assets");



    //Load Street House
    objdebug = App->assimpMeshes->LoadMeshFromFile("Assets/Scenes/scene.DAE");
    //objdebug->transform->setRotation({ -90, 0, 0 });
    root->transform->setRotation({ 0, 0, -90 });


    return true;
}


update_status ModuleScene::PreUpdate(float dt) {
    
    return UPDATE_CONTINUE;
}

update_status ModuleScene::Update(float dt) {
    
    return UPDATE_CONTINUE;
}

update_status ModuleScene::PostUpdate(float dt) {
    
    return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp() {
    
    LOG("Scene Cleanup")
    delete root;
    delete objdebug;
    return true;
}


void ModuleScene::SceneWindow()
{
	ImGui::Begin("Scene");
	//WindowSize = ImGui::GetContentRegionAvail();

	////Prevent image stretching by setting new aspect ratio
	//float aspectRatio = WindowSize.x / WindowSize.y;
	//App->camera->sceneCam->FrustumCam.verticalFov = App->camera->sceneCam->FOV * DEGTORAD;
	//App->camera->sceneCam->FrustumCam.horizontalFov = 2.0f * atanf(tanf(App->camera->sceneCam->FrustumCam.verticalFov / 2.0f) * aspectRatio);

	//ImGui::Image((ImTextureID)App->camera->sceneCam->cameraBuffer, WindowSize, ImVec2(0, 1), ImVec2(1, 0));


	//if (ImGui::IsMouseClicked(0) && ImGui::IsWindowHovered()) {

	//	ImVec2 mousePos = ImGui::GetMousePos();
	//	ImVec2 normalized = NormalizeMouse(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y + ImGui::GetFrameHeight(), ImGui::GetWindowSize().x, ImGui::GetWindowSize().y - ImGui::GetFrameHeight(), mousePos);

	//	LineSegment my_ray = App->camera->sceneCam->FrustumCam.UnProjectLineSegment(normalized.x, normalized.y);
	//	std::vector<GameObject*> interVec;

	//	//for with all the meshes triangles
	//	for (size_t i = 0; i < App->loader->meshes.size(); i++)
	//	{
	//		if (my_ray.Intersects(App->loader->meshes[i]->obb)) {
	//			if (App->loader->meshes[i]->Owner != nullptr)
	//			{
	//				if (App->loader->meshes[i]->Owner != nullptr)
	//					interVec.push_back(App->loader->meshes[i]->Owner);
	//			}
	//		}
	//	};

	//	float distLength;
	//	float minDistLenght = 0;

	//	for (size_t j = 0; j < interVec.size(); j++) {

	//		ComponentMesh* gObjMesh = interVec[j]->GetMeshComponent();
	//		if (gObjMesh != nullptr) {

	//			for (size_t i = 0; i < gObjMesh->meshes.size(); i++) {

	//				Mesh* mesh = gObjMesh->meshes[i];
	//				float4x4 matTrans = interVec[j]->transform->GetTransformMatrix().Transposed();

	//				if (mesh->num_indices > 9) {
	//					for (size_t b = 0; b < mesh->num_indices; b += 3) {

	//						float* t1 = &mesh->vertices[mesh->indices[b] * VERTICES];
	//						float* t2 = &mesh->vertices[mesh->indices[b + 1] * VERTICES];
	//						float* t3 = &mesh->vertices[mesh->indices[b + 2] * VERTICES];

	//						float4 tr1 = matTrans * float4(*t1, *(t1 + 1), *(t1 + 2), 1);
	//						float4 tr2 = matTrans * float4(*t2, *(t2 + 1), *(t2 + 2), 1);
	//						float4 tr3 = matTrans * float4(*t3, *(t3 + 1), *(t3 + 2), 1);

	//						float3 tri1 = float3(tr1.x, tr1.y, tr1.z);
	//						float3 tri2 = float3(tr2.x, tr2.y, tr2.z);
	//						float3 tri3 = float3(tr3.x, tr3.y, tr3.z);

	//						Triangle triangle(tri1, tri2, tri3);

	//						if (my_ray.Intersects(triangle, &distLength, nullptr))
	//						{
	//							if (minDistLenght == 0) {
	//								minDistLenght = distLength;
	//								App->hierarchy->SetGameObject(interVec[j]);
	//								continue;
	//							}
	//							if (distLength < minDistLenght) {
	//								minDistLenght = distLength;
	//								App->hierarchy->SetGameObject(interVec[j]);
	//							}

	//						}
	//					}
	//				}
	//			}
	//		}

	//	}
	//	interVec.clear();}
	


	ImGui::End();
}

void ModuleScene::GameWindow()
{
	
	ImGui::Begin("Game");
	
	ImGui::End();
}

GameObject* ModuleScene::CreateGameObject(GameObject* parent)
{
    GameObject* newGameObject = new GameObject(parent);



    return newGameObject;

}

